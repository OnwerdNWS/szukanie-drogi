#include "bot.hpp"
#include "siecNeuronowa.hpp"
#include "globalne.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <SFML/Graphics.hpp>

// void testAkcji(agent* bot, int wybranyRuch)
// {
// 	std::cout << "pozycja: " << bot->kordy.x << " " << bot->kordy.y << std::endl;
// 	for(int i=0; i<4; i++)
// 	{
// 		std::cout << bot->siec.wynik[i] << " ";
// 	}
// 	std::cout << std::endl;
// 	std::cout << "wybrany ruch: " << wybranyRuch << std::endl;
// 	std::cout << "widok przed ruchem:" << std::endl;
// 	std::cout << bot->widokMapy[0] << " " << bot->widokMapy[1] << " " << bot->widokMapy[2] << std::endl;
// 	std::cout << bot->widokMapy[3] << " " << "A" << " " << bot->widokMapy[4] << std::endl;
// 	std::cout << bot->widokMapy[5] << " " << bot->widokMapy[6] << " " << bot->widokMapy[7] << std::endl;
// 	std::cout << "pamiec: " << std::endl;
// 	bot->printMemory();
// 	std::cout << std::endl;
// }
int indexMax(std::vector<float> tab) 
{
    float max = tab[0];
    int miejsce=0;

    for (int i = 1; i < 4; i++) 
	{
        if (tab[i] > max) 
		{
            max = tab[i];
            miejsce=i;
        }
    }
    return miejsce;
}
float znajdzMax(std::vector<float> tab) 
{
    float max = tab[0];
    int miejsce=0;

    for (int i = 1; i < 4; i++) 
	{
        if (tab[i] > max) 
		{
            max = tab[i];
            miejsce=i;
        }
    }
    return max;
}

double obliczReward(opisPolaPamieci pole1, opisPolaPamieci pole2, coordinate kordy)
{
	float reward;
	if(kordy==kordyCel) reward = 100;
	else if(abs(pole1.wysokosc-pole2.wysokosc)>=2) reward = -20;
	else if(pole2.czyOdwiedzone) reward = -1;
	else reward = 0.5;
	
	
	return reward;
}
float obliczTarget(std::vector<float> nextQ,float reward, bool czyKoniec)
{
	float target;
	float gamma = 0.95;
	float maxNextQ = znajdzMax(nextQ);
	if(czyKoniec) target = reward;
	else
	{
		target = reward + gamma * maxNextQ;
	}
	return target;
}


void agent::ruch(bool nauka,bufor& pamiecDoNauki)
{
	this->updateMemory();
	experience expTemp;
	std::vector<float> widok1 = zmianaWidoku(widokPamieci,wysokosc);
	przypiszTablice(expTemp.polePrzedRuchem, widokPamieci);
	expTemp.wysokoscPrzed = wysokosc;
	std::vector<float> Q = siec.oblicz(widok1);
	int strona;
	double los=losowanieDouble(0,10);
	if(los < precyzja)
	{
		strona = indexMax(Q);
	}
	else
	{
		strona = losowanieInt(0,4-1);
	}
	expTemp.strona = strona;
	// testAkcji(this,strona);
	opisPolaPamieci kolejnePole;
	opisPolaPamieci terazPole = memory[kordy-startKordy];
	coordinate kordyTemp;
	if(strona==0)
	{
		kordyTemp = {0,1};
		kolejnePole = memory[kordy-startKordy+kordyTemp];
		if(abs(kolejnePole.wysokosc-terazPole.wysokosc)<2) kordy= kordy+kordyTemp;
	}
	else if(strona==1)
	{
		kordyTemp = {1,0};
		kolejnePole = memory[kordy-startKordy+kordyTemp];
		if(abs(kolejnePole.wysokosc-terazPole.wysokosc)<2) kordy= kordy+kordyTemp;
	}
	else if(strona==2)
	{
		kordyTemp = {0,-1};
		kolejnePole = memory[kordy-startKordy+kordyTemp];
		if(abs(kolejnePole.wysokosc-terazPole.wysokosc)<2) kordy= kordy+kordyTemp;
	}
	else if(strona==3)
	{
		kordyTemp = {-1,0};
		kolejnePole = memory[kordy-startKordy+kordyTemp];
		if(abs(kolejnePole.wysokosc-terazPole.wysokosc)<2) kordy= kordy+kordyTemp;
	}
	this->updateMemory();
	
	if(licznikRuchow%2000==0) siecDocelowa = siec;
	
	if(licznikRuchow%25000==0 and precyzja<9) precyzja+=0.5;


	przypiszTablice(expTemp.polePoRuchu, widokPamieci);
	expTemp.wysokoscPo = wysokosc;
	expTemp.reward = obliczReward(terazPole,kolejnePole,kordy);
	if(kordy==kordyCel)
	{
		expTemp.czyKoniec = true;
	}
	
	if(nauka) pamiecDoNauki.dodaj(expTemp);

	if(pamiecDoNauki.dlugosc>=pamiecDoNauki.momentNauki && licznikRuchow%16==0 && nauka)
	{
		std::vector<experience> losowaneEXP = pamiecDoNauki.losuj(32);

		const int iloscPaczki = 32;
		const int rozmiarWejscia = 600;
		std::vector<float> widokPrzedPaczka(rozmiarWejscia * iloscPaczki);
		std::vector<float> widokPoPaczka(rozmiarWejscia * iloscPaczki);

		for(int i = 0; i < iloscPaczki; ++i)
		{
			std::vector<float> widokPrzed = zmianaWidoku(losowaneEXP[i].polePrzedRuchem, losowaneEXP[i].wysokoscPrzed);
			std::vector<float> widokPo = zmianaWidoku(losowaneEXP[i].polePoRuchu, losowaneEXP[i].wysokoscPo);
			std::copy(widokPrzed.begin(), widokPrzed.end(), widokPrzedPaczka.begin() + i * rozmiarWejscia);
			std::copy(widokPo.begin(), widokPo.end(), widokPoPaczka.begin() + i * rozmiarWejscia);
		}
		const std::vector<float>& Qpo = siecDocelowa.obliczPaczka(widokPoPaczka, iloscPaczki);
		std::vector<float> target(iloscPaczki);
		for(int i=0; i<iloscPaczki; i++)
		{
			std::vector<float> wiersz(Qpo.begin() + i * 4, Qpo.begin() + (i + 1) * 4);
			target[i] = obliczTarget(wiersz, losowaneEXP[i].reward, losowaneEXP[i].czyKoniec);
		}

		const std::vector<float>& Qprzed = siec.obliczPaczka(widokPrzedPaczka, iloscPaczki);
		std::vector<float> gradientStraty(iloscPaczki * 4, 0.0f);
		for(int i=0; i<iloscPaczki; i++)
		{
			int strona = losowaneEXP[i].strona;
			gradientStraty[i * 4 + strona] = (Qprzed[i * 4 + strona] - target[i])/iloscPaczki;
		}
		siec.zerujGradient();
		siec.wsteczPaczka(gradientStraty, iloscPaczki);
		siec.aktualizuj();
	}

	licznikRuchow++;
}
void agent::updateView()
{
	std::vector<bool> widok = czyWidac(kordy);
	int licznik = 0;
	int size = 3;
	for(int y=size; y>=-size; y--)
	{
		for(int x=-size; x<=size; x++)
		{
			if(x==y and x==0) continue;
			widokMapy[licznik]=opisPolaPamieci{};
			
			if(planszaMinX<= this->kordy.x+x and this->kordy.x+x<=planszaMaxX and planszaMinY<=this->kordy.y+y and this->kordy.y+y<=planszaMaxY)
			{
				if(widok[licznik])
				{
					coordinate temp = {kordy.x+x,kordy.y+y};
					widokMapy[licznik].wysokosc = plansza[this->kordy.x+x][this->kordy.y+y].wysokosc;
					widokMapy[licznik].czyZnane = true;
					if(kordyCel==temp) widokMapy[licznik].czyCel=true;
				}
			}
			else
			{
				widokMapy[licznik].granicaMapy = true;
			}
			licznik++;
		}
	}
}
void agent::updateMemory()
{
	updateView();
	wysokosc = plansza[kordy.x][kordy.y].wysokosc-startWysokosc;
	coordinate pamiecKordy = this->kordy - this->startKordy;
	int licznik=0;
	for(int y=3; y>=-3; y--)
	{
		for(int x=-3; x<=3; x++)
		{
			coordinate przesuniecie{x,y};
			if(x==0 and y==0) 
			{
				memory[pamiecKordy+przesuniecie].wysokosc=plansza[kordy.x][kordy.y].wysokosc;
				memory[pamiecKordy+przesuniecie].czyZnane=true;
				memory[pamiecKordy+przesuniecie].czyOdwiedzone=true;
				memory[pamiecKordy+przesuniecie].wysokosc-=startWysokosc;
				continue;
			}
			else if((memory.find(pamiecKordy+przesuniecie) == memory.end()))
			{
				if(widokMapy[licznik].czyZnane)
				{
					memory[pamiecKordy+przesuniecie]=widokMapy[licznik];
					memory[pamiecKordy+przesuniecie].wysokosc-=startWysokosc;
				}
				if(widokMapy[licznik].granicaMapy)
				{
					memory[pamiecKordy+przesuniecie]=widokMapy[licznik];
					memory[pamiecKordy+przesuniecie].wysokosc=120;
				}
				
			}
			licznik++;
		}
	}
	licznik=0;
	for(int y=5; y>=-5; y--)
	{
		for(int x=-5; x<=5; x++)
		{
			coordinate przesuniecie{x,y};
			if(x==0 and y==0)
			{
				continue;
			}
			widokPamieci[licznik] = opisPolaPamieci{};
			if(memory.find(pamiecKordy+przesuniecie) != memory.end())
			{
				widokPamieci[licznik] = memory[pamiecKordy+przesuniecie];
			}
			licznik++;
		}
	}
}
void agent::printMemory()
{
	coordinate pamiecKordy = this->kordy - this->startKordy;
	if(this->memory.empty())
	{
		std::cout << "Memory is empty" << std::endl;
		return;
	}
	int minX = pamiecKordy.x;
	int maxX = pamiecKordy.x;
	int minY = pamiecKordy.y;
	int maxY = pamiecKordy.y;
	for(const auto& element : this->memory)
	{
		coordinate temp = element.first;
		minX = std::min(minX, temp.x);
		maxX = std::max(maxX, temp.x);
		minY = std::min(minY, temp.y);
		maxY = std::max(maxY, temp.y);
	}
	unsigned int sizeX = maxX-minX+1;
	unsigned int sizeY = maxY-minY+1;
	sf::RenderWindow window(sf::VideoMode({sizeX*8,sizeY*8}),"Pamiec");

	const float size = float(sizeX)/planszaX;
	sf::Image obraz({sizeX,sizeY}, sf::Color::Black);
	for(int y=maxY; y>=minY; y--)
	{
		for(int x=minX; x<=maxX; x++)
		{
			coordinate temp = {x,y};
			if(memory.find(temp)!=memory.end())
			{
				int przelicznik = (memory[temp].wysokosc+maxWysokosc)/(2.0*maxWysokosc)*255;
				obraz.setPixel({(unsigned)(x-minX),(unsigned)(maxY-y)},sf::Color(przelicznik,0,0));
			}
		}
		std::cout << std::endl;
	}

	sf::Texture tekstura(obraz);	//generowanie tekstury
	// tekstura.setSmooth(true);		//wygładzenie tekstury
	sf::Sprite pamiec(tekstura);		//tworzenie mapy z tekstury
	pamiec.setScale({size,size});		//ustawianie wielkosci mapy
	
	while(window.isOpen())
	{
		while(auto event = window.pollEvent())
		{
			if(event->is<sf::Event::Closed>()) window.close();
		}

		window.clear(sf::Color::White);
		
		window.draw(pamiec);

		sf::RectangleShape pole(sf::Vector2f(size/2,size/2));
		pole.setPosition({(pamiecKordy.x-minX)*size+size/4,(maxY-pamiecKordy.y)*size+size/4});
		pole.setFillColor(sf::Color(200,200,200));
		window.draw(pole);

		window.display();
	}
}
void agent::clearTemporaryMemory()
{
	this->memory.clear();
	this->memory.rehash(0);
}

void przypiszTablice(opisPolaPamieci tab1[rozmiarPamieci], opisPolaPamieci tab2[rozmiarPamieci])
{
	for(int i=0; i<rozmiarPamieci; i++)
	{
		tab1[i] = tab2[i];
	}
}


bool przedzial(double px, double py, int cx, int cy, double& lo, double& hi)
{
    lo = 0.0;
    hi = 1.0;
    double p[2] = {px,py};
    int c[2] = {cx,cy};

    for(int i=0;i<2;i++)
    {
        double a = c[i]-0.5;
        double b = c[i]+0.5;

        if(abs(p[i])<1e-12) // sprawdza czy nie idzie wzdłóż osi (z marginesem błedu)
        {
            if(a>0 || b<0) return false;
            continue;
        }

        double t1 = a/p[i];
        double t2 = b/p[i];
        if(t1>t2) std::swap(t1,t2);
        lo = std::max(lo,t1);
        hi = std::min(hi,t2);
    }
    return (lo<=hi+1e-9);
}
promien zbudujPromien(int docelowyX, int docelowyY, double px, double py)
{
    promien pr;
    std::vector<KratkaNaPromieniu> dotkniecia;

    for(int cy = 3; cy>=-3; cy--)
    {
        for(int cx=-3; cx<=3; cx++)
        {
            if(cx==0 && cy==0) continue;
            if(cx==docelowyX && cy==docelowyY) continue;

            double lo, hi;
            if(!przedzial(px,py,cx,cy,lo,hi)) continue;

            if(hi-lo<1e-9) dotkniecia.push_back({cx,cy,lo,hi});
            else pr.kratki.push_back({cx,cy,lo,hi});
        }
    }
    for(int i=0;i<dotkniecia.size(); i++)
    {
        for(int j=i+1;j<dotkniecia.size(); j++)
        {
            if(abs(dotkniecia[i].tWe-dotkniecia[j].tWe)<1e-9)
            {
                pr.narozniki.push_back({dotkniecia[i].cx,dotkniecia[i].cy,dotkniecia[j].cx,dotkniecia[j].cy,dotkniecia[i].tWe});
            }
        }
    }
    return pr;
}
std::vector<celWidoku> zbudujTabliceWidoku()
{
    const double probki[5][2] = {{0,0},{-0.25,-0.25},{-0.25,0.25},{0.25,-0.25},{0.25,0.25}};
    std::vector<celWidoku> tab;

    for(int dy = 3; dy>=-3; dy--)
    {
        for(int dx=-3; dx<=3; dx++)
        {
            if(dx==0 && dy==0) continue;
            celWidoku cel;
            cel.dx = dx;
            cel.dy = dy;
            for(int p=0; p<5; p++)
            {
                double px = dx+probki[p][0];
                double py = dy+probki[p][1];
                cel.promienie[p] = zbudujPromien(dx,dy,px,py);
            }
            tab.push_back(cel);
        }
    }
    return tab;
}
const std::vector<celWidoku>& tablicaWidoku()
{
    static const std::vector<celWidoku> tab = zbudujTabliceWidoku();
    return tab;
}
void wysokosciOkna(coordinate agent, double h[7][7])
{
    for(int cx =-3; cx<=3; cx++)
    {
        for(int cy=-3; cy<=3; cy++)
        {
            int x = agent.x +cx;
            int y = agent.y +cy;
            if(x<planszaMinX || y<planszaMinY || x>planszaMaxX || y>planszaMaxY) h[cx+3][cy+3]=1000;
            else h[cx+3][cy+3] = plansza[x][y].wysokosc;
        }
    }
}
bool promienWolny(const promien& pr, double oko, double hCel, double h[7][7])
{
    for(const KratkaNaPromieniu& k : pr.kratki)
    {
        double liniaWe = oko + (hCel-oko)*k.tWe;
        double liniaWy = oko + (hCel-oko)*k.tWy;
        double najnizej = std::min(liniaWe,liniaWy);
        if(h[k.cx+3][k.cy+3]>=najnizej) return false;
    }
    for(const paraNaroznikow& n : pr.narozniki)
    {
        double linia = oko + (hCel-oko)*n.t;
        bool zaslaniaA = h[n.ax+3][n.ay+3] >=linia;
        bool zaslaniaB = h[n.bx+3][n.by+3] >=linia;
        if(zaslaniaA && zaslaniaB) return false;
    }
    return true;
}
std::vector<bool> czyWidac(coordinate agent)
{
    double h[7][7];
    wysokosciOkna(agent,h);
    double oko = h[3][3]+1.5;

    const std::vector<celWidoku>& tab = tablicaWidoku();
    std::vector<bool> widac(48);

    for(int i=0; i<48; i++)
    {
        const celWidoku& cel = tab[i];
        double hCel = h[cel.dx+3][cel.dy+3];
        int wolne =0;
        for(int p=0; p<5;p++)
        {
            if(promienWolny(cel.promienie[p],oko,hCel,h)) wolne++;
        }
        widac[i] = (wolne>= 3);
    }
    return widac;
}
