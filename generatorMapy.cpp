#include "globalne.hpp"
#include "generatorMapy.hpp"
#include <iostream>
#include <queue>
#include <random>
#include <SFML/Graphics.hpp>



int BFS(coordinate bot) //program szukający najkrótszej drogi oraz zwraca ilosc krokow
{
	bool visited[planszaX][planszaY] = {};
	std::queue<coordinate> kolejka;
	std::queue<coordinate> kolejka2;
	bool osiagnietoCel = false;
	visited[bot.x][bot.y]=true;
	kolejka.push(bot);
	int kroki=0;
	while(!osiagnietoCel && !kolejka.empty())
	{
		while(!kolejka.empty())
		{
			kolejka2.push(kolejka.front());
			kolejka.pop();
		}
		// int licznik=0;
		while(!kolejka2.empty())
		{
			coordinate temp = kolejka2.front();
			kolejka2.pop();
			if(temp.y<planszaY-1 && visited[temp.x][temp.y+1]==false)
			{
				int roznica = plansza[temp.x][temp.y].wysokosc - plansza[temp.x][temp.y+1].wysokosc;
				if(roznica<0) roznica*= -1;
				if(roznica<2)
				{
					visited[temp.x][temp.y+1]=true;
					kolejka.push({temp.x,temp.y+1});
				}
			}
			if(temp.y>0 && visited[temp.x][temp.y-1]==false)
			{
				int roznica = plansza[temp.x][temp.y].wysokosc - plansza[temp.x][temp.y-1].wysokosc;
				if(roznica<0) roznica*= -1;
				if(roznica<2)
				{
					visited[temp.x][temp.y-1]=true;
					kolejka.push({temp.x,temp.y-1});
				}
			}
			if(temp.x<planszaX-1 && visited[temp.x+1][temp.y]==false)
			{
				int roznica = plansza[temp.x][temp.y].wysokosc - plansza[temp.x+1][temp.y].wysokosc;
				if(roznica<0) roznica*= -1;
				if(roznica<2)
				{
					visited[temp.x+1][temp.y]=true;
					kolejka.push({temp.x+1,temp.y});
				}
			}
			if(temp.x>0 && visited[temp.x-1][temp.y]==false)
			{
				int roznica = plansza[temp.x][temp.y].wysokosc - plansza[temp.x-1][temp.y].wysokosc;
				if(roznica<0) roznica*= -1;
				if(roznica<2)
				{
					visited[temp.x-1][temp.y]=true;
					kolejka.push({temp.x-1,temp.y});
				}
			}
			if(temp==kordyCel)
			{
				osiagnietoCel=true;
				return kroki;
			}
		}
		kroki++;
	}
	return -1;
}
void planszaWyswietl(coordinate bot)
{
	sf::RenderWindow window(sf::VideoMode({1024,1024}),"Plansza");

	const float size = 1024.0/planszaX;
	sf::Image obraz({planszaX,planszaY}, sf::Color::Black);

	for(int y = planszaY-1; y>=0; y--)
	{
		for(int x=0; x<planszaX; x++)
		{
			int przelicznik = (plansza[x][y].wysokosc+maxWysokosc)/(2.0*maxWysokosc)*255;
			obraz.setPixel({(unsigned)x,(unsigned)y},sf::Color(przelicznik,0,0));
		}
	}
		
	sf::Texture tekstura(obraz);	//generowanie tekstury
	// tekstura.setSmooth(true);		//wygładzenie tekstury
	sf::Sprite mapa(tekstura);		//tworzenie mapy z tekstury
	mapa.setScale({size,size});		//ustawianie wielkosci mapy
	
	while(window.isOpen())
	{
		while(auto event = window.pollEvent())
		{
			if(event->is<sf::Event::Closed>()) window.close();
		}

		window.clear(sf::Color::White);
		
		window.draw(mapa);

		sf::RectangleShape pole2(sf::Vector2f(size/2,size/2));
		pole2.setPosition({bot.x*size+size/4,bot.y*size+size/4});
		pole2.setFillColor(sf::Color(200,200,200));
		window.draw(pole2);
		pole2.setPosition({kordyCel.x*size+size/4,kordyCel.y*size+size/4});
		pole2.setFillColor(sf::Color(20,100,20));
		window.draw(pole2);

		window.display();
	}
	
}
void generatorMapy(unsigned int seed) //rozstawia mape
{
	std::mt19937 genMapy(seed);
	std::uniform_real_distribution<double> los(-1.0,1.0);
	double temp[planszaX][planszaY];
	for(int y=planszaY-1; y>=0; y--)
	{
		for(int x=0; x<planszaX; x++)
		{
			plansza[x][y].wysokosc=0;
			temp[x][y]=0;
		}
	}
	double wielkoscBloku = 32;
	double aplituda = 1.0;
	double sumaAplitudy = 0.0;
	for(int z=0;z<planszaX/32;z++)
	{
		int size = planszaX/wielkoscBloku+1;
		double oktawa[size][size];
		for(int y=size-1; y>=0;y--)
		{
			for(int x=0; x<size;x++)
			{
				oktawa[x][y] = los(genMapy);
			}
		}
		for(int y=planszaY-1; y>=0; y--)
		{
			for(int x=0; x<planszaX; x++)
			{
				int i = x/wielkoscBloku; // ustawia wspolrzędne bloczku jako x
				int j = y/wielkoscBloku; // ustawia wspolrzędne bloczku jako y

				double tx = (x-i*wielkoscBloku)/wielkoscBloku;
				double ty = (y-j*wielkoscBloku)/wielkoscBloku;
				double sx = tx*tx*(3-2*tx);
				double sy = ty*ty*(3-2*ty);

				double v00 = oktawa[i][j];	
				double v10 = oktawa[i+1][j];
				double v01 = oktawa[i][j+1];
				double v11 = oktawa[i+1][j+1];

				double krawedzDol = v00+(v10-v00)*sx;
				double krawedzGora = v01+(v11-v01)*sx;
				temp[x][y] += aplituda*(krawedzDol+(krawedzGora-krawedzDol)*sy);
			}
		}
		sumaAplitudy+=aplituda;
		wielkoscBloku/=2;
		aplituda/=2;
	}
	for(int y=planszaY-1; y>=0; y--)
	{
		for(int x=0; x<planszaX; x++)
		{
			plansza[x][y].wysokosc = std::lround(temp[x][y]/sumaAplitudy*maxWysokosc);
		}
	}
}
void rozstawienie(coordinate& bot, unsigned int seed) //rozstawia agenta i cel
{
	std::mt19937 genMapy(seed+1000000);
	std::uniform_int_distribution<int> los(0,planszaX-1);
	int krokiDoRozwiązania=-1;
	do
	{
		bot={los(genMapy),los(genMapy)};
		kordyCel={los(genMapy),los(genMapy)};
		while(kordyCel==bot) kordyCel={los(genMapy),los(genMapy)};
		krokiDoRozwiązania = BFS(bot);
	} while (krokiDoRozwiązania<0);
}

void zapiszPlansze(opisPola (&temp)[planszaX][planszaY], std::ofstream& plik)
{
	int rozmiar = sizeof(temp);
	plik.write((char*)temp,rozmiar);
}
void generujPlansze(int ilosc, std::string nazwaPlik)
{
	std::ofstream plik(nazwaPlik, std::ios::binary);
	for(int i=0; i<ilosc;i++)
	{
		generatorMapy(i);
		zapiszPlansze(plansza,plik);
	}
	plik.close();
}
void wczytajPlansze(opisPola (&temp)[planszaX][planszaY], int nrMapy, std::string nazwaPliku)
{
	std::ifstream plik(nazwaPliku, std::ios::binary);
	int rozmiar = sizeof(temp);
	plik.seekg(nrMapy*rozmiar, std::ios::beg);
	plik.read((char*)temp,rozmiar);
	plik.close();
}

