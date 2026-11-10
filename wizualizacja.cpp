#include "wizualizacja.hpp"
#include "globalne.hpp"
#include "generatorMapy.hpp"
#include "bot.hpp"
#include <SFML/Graphics.hpp>
#include <string>

static sf::Color kolorWysokosci(int h)
{
	int jasnosc = (h + maxWysokosc)*255 / (2*maxWysokosc);
	if(jasnosc < 0) jasnosc = 0;
	if(jasnosc > 255) jasnosc = 255;
	return sf::Color(jasnosc, jasnosc/2, 0);
}
static sf::Vector2u piksel(int x, int y)
{
    return {(unsigned)(x - planszaMinX), (unsigned)(y - planszaMinY)};
}

// pokazuje jeden epizod testowy krok po kroku
// sterowanie: spacja - pauza, strzalka w gore / w dol - szybciej / wolniej, zamkniecie okna - koniec
void pokazTest(agent& bot, unsigned int seed,unsigned int rozmiar, int nrMapy)
{
	fragmentacja(nrMapy,rozmiar);
	bufor pusty;	// przy nauka=false nic do niego nie trafia
	bot.clearTemporaryMemory();
	generatorMapy(seed);
	rozstawienie(bot.kordy, seed);
	bot.startKordy = bot.kordy;
	bot.startWysokosc = plansza[bot.kordy.x][bot.kordy.y].wysokosc;
	int ruch = (rozmiar*rozmiar)/2;

	const float size = 1024.0f/rozmiar;
	sf::RenderWindow window(sf::VideoMode({rozmiar*int(size),rozmiar*int(size)}),"Test agenta");
	
	// warstwa 1: mapa wysokosci (budowana raz)
	sf::Image obrazMapy({rozmiar, rozmiar}, sf::Color::Black);
	for(int y = planszaMaxY-planszaMinY; y>=0; y--)
	{
		for(int x=0; x<=planszaMaxX-planszaMinX; x++)
		{
			int przelicznik = (plansza[x+planszaMinX][y+planszaMinY].wysokosc+maxWysokosc)/(2.0*maxWysokosc)*255;
			obrazMapy.setPixel({(unsigned)x,(unsigned)y},sf::Color(przelicznik,przelicznik/2,0));
		}
	}
	sf::Texture teksturaMapy(obrazMapy);
	sf::Sprite mapa(teksturaMapy);
	mapa.setScale({size, size});

	// warstwa 2: "mgla" - to, czego agent jeszcze nie zna (aktualizowana co krok)
	sf::Image obrazMgly({rozmiar, rozmiar}, sf::Color(0, 0, 0, 200));
	sf::Texture teksturaMgly(obrazMgly);
	sf::Sprite mgla(teksturaMgly);
	mgla.setScale({size, size});

	sf::RectangleShape znacznik({size, size});
	sf::Clock zegar;
	int opoznienieMs = 50;
	bool pauza = false;
	int krok = 0;
	bool koniec = false;

	while(window.isOpen())
	{
		while(auto event = window.pollEvent())
		{
			if(event->is<sf::Event::Closed>()) window.close();
			if(const auto* klawisz = event->getIf<sf::Event::KeyPressed>())
			{
				if(klawisz->code == sf::Keyboard::Key::Space) pauza = !pauza;
				if(klawisz->code == sf::Keyboard::Key::Up && opoznienieMs > 5) opoznienieMs /= 2;
				if(klawisz->code == sf::Keyboard::Key::Down && opoznienieMs < 1000) opoznienieMs *= 2;
			}
		}

		// jeden krok agenta co opoznienieMs milisekund
		if(!pauza && !koniec && zegar.getElapsedTime().asMilliseconds() >= opoznienieMs)
		{
			zegar.restart();
			bot.ruch(false, pusty);
			krok++;
			if(bot.kordy == kordyCel)
			{
				koniec = true;
				window.setTitle("Cel znaleziony w " + std::to_string(krok) + " krokach");
			}
			else if(krok >= ruch)
			{
				koniec = true;
				window.setTitle("Nie znaleziono celu (" + std::to_string(krok) + " krokow)");
			}

			// odslanianie mgly na podstawie pamieci agenta
			for(const auto& element : bot.memory)
			{
				unsigned int x = bot.startKordy.x + element.first.x;
				unsigned int y = bot.startKordy.y + element.first.y;
				if(x < planszaMinX || y < planszaMinY || x > planszaMaxX || y > planszaMaxY) continue;
				if(element.second.czyOdwiedzone) obrazMgly.setPixel(piksel(x, y), sf::Color(80, 160, 255, 140));
				else if(element.second.czyZnane) obrazMgly.setPixel(piksel(x, y), sf::Color(0, 0, 0, 0));
			}
			teksturaMgly.update(obrazMgly);
		}

		window.clear();
		window.draw(mapa);
		window.draw(mgla);

		znacznik.setFillColor(sf::Color(40, 220, 40));
		sf::Vector2u celPixel = piksel(kordyCel.x, kordyCel.y);
		znacznik.setPosition({celPixel.x * size, celPixel.y * size});
		window.draw(znacznik);

		znacznik.setFillColor(sf::Color::White);
		sf::Vector2u botPiksel = piksel(bot.kordy.x, bot.kordy.y);
		znacznik.setPosition({botPiksel.x * size, botPiksel.y * size});
		window.draw(znacznik);

		window.display();
	}
}


void przejdzMape(unsigned int seed,unsigned int rozmiar, int nrMapy)
{
	generatorMapy(seed);
	fragmentacja(nrMapy,rozmiar);
	agent gracz;
	rozstawienie(gracz.kordy, seed);
	int ruch = (rozmiar*rozmiar)/2;
	gracz.startKordy = gracz.kordy;
	gracz.startWysokosc = plansza[gracz.kordy.x][gracz.kordy.y].wysokosc;

	int kroki = 0;
	float size = 100.0f;
	sf::RenderWindow window(sf::VideoMode({11*unsigned(int(size)),11*unsigned(int(size))}),"Test jako gracz");
	sf::Image obrazPamieci({11, 11}, sf::Color::Black);
	gracz.updateMemory();
	for(int i=5; i>=-5; i--)
	{
		for(int j=-5; j<=5; j++)
		{
			coordinate temp = {gracz.kordy.x - gracz.startKordy.x+j, gracz.kordy.y - gracz.startKordy.y+i};
			if(gracz.memory.find(temp) == gracz.memory.end())
			{
				obrazPamieci.setPixel({(unsigned)(j+5),(unsigned)(5-i)},sf::Color::Black);
			}
			else
			{
				if(gracz.memory[temp].granicaMapy)
				{
					obrazPamieci.setPixel({(unsigned)(j+5),(unsigned)(5-i)},sf::Color(200,200,200));
				}
				else if(gracz.memory[temp].czyZnane)
				{
					int przelicznik = (gracz.memory[temp].wysokosc+maxWysokosc)/(2.0*maxWysokosc)*255;
					obrazPamieci.setPixel({(unsigned)(j+5),(unsigned)(5-i)},sf::Color(przelicznik,przelicznik/2,0));
				}
				else
				{
					obrazPamieci.setPixel({(unsigned)(j+5),(unsigned)(5-i)},sf::Color(100,100,100));
				}
			}
		}
	}
	sf::Texture teksturaPamieci(obrazPamieci);
	sf::Sprite pamiec(teksturaPamieci);
	pamiec.setScale({size, size});

	sf::RectangleShape celWyswietl({size/2, size/2});
	sf::RectangleShape graczWyswietl({size/2, size/2});
	sf::RectangleShape ruchWyswietl({size/4, size/4});
	ruchWyswietl.setFillColor(sf::Color::Blue);
	bool czyCelWyswietlony = false;
	while(window.isOpen())
	{
		coordinate kordyGracza = gracz.kordy - gracz.startKordy;
		if(kroki >= ruch)
		{
			std::cout << "Nie znaleziono celu (" << ruch << " krokow)" << std::endl;
			window.close();
		}
		if(gracz.kordy == kordyCel)
		{
			std::cout << "Cel znaleziony w " << kroki << " krokach" << std::endl;
			window.close();
		}
		while(auto event = window.pollEvent())
		{
			if(event->is<sf::Event::Closed>()) window.close();
			if(const auto* klawisz = event->getIf<sf::Event::KeyPressed>())
			{
				if(klawisz->code == sf::Keyboard::Key::Up)
				{
					int roznica = gracz.memory[kordyGracza].wysokosc - gracz.memory[kordyGracza + coordinate{0,1}].wysokosc;
					if(gracz.kordy.y < planszaMaxY and abs(roznica) < 2)
					{
						gracz.kordy.y++;
						gracz.updateMemory();
					}
					kroki++;
				}
				else if(klawisz->code == sf::Keyboard::Key::Down)
				{
					int roznica = gracz.memory[kordyGracza].wysokosc - gracz.memory[kordyGracza + coordinate{0,-1}].wysokosc;
					if(gracz.kordy.y > planszaMinY and abs(roznica) < 2)
					{
						gracz.kordy.y--;
						gracz.updateMemory();
					}
					kroki++;
				}
				else if(klawisz->code == sf::Keyboard::Key::Left)
				{
					int roznica = gracz.memory[kordyGracza].wysokosc - gracz.memory[kordyGracza + coordinate{-1,0}].wysokosc;
					if(gracz.kordy.x > planszaMinX and abs(roznica)<2)
					{
						gracz.kordy.x--;
						gracz.updateMemory();
					}
					kroki++;
				}
				else if(klawisz->code == sf::Keyboard::Key::Right)
				{
					int roznica = gracz.memory[kordyGracza].wysokosc - gracz.memory[kordyGracza + coordinate{1,0}].wysokosc;
					if(gracz.kordy.x < planszaMaxX and abs(roznica)<2)
					{
						gracz.kordy.x++;
						gracz.updateMemory();
					}
					kroki++;
				}
			}
		}
		czyCelWyswietlony = false;
		for(int i=5; i>=-5; i--)
		{
			for(int j=-5; j<=5; j++)
			{
				coordinate temp = {gracz.kordy.x - gracz.startKordy.x+j, gracz.kordy.y - gracz.startKordy.y+i};
				if(gracz.memory.find(temp) == gracz.memory.end())
				{
					obrazPamieci.setPixel({(unsigned)(j+5),(unsigned)(5-i)},sf::Color::Black);
				}
				else
				{
					if(gracz.memory[temp].granicaMapy)
					{
						obrazPamieci.setPixel({(unsigned)(j+5),(unsigned)(5-i)},sf::Color(200,200,200));
					}
					else if(gracz.memory[temp].czyZnane)
					{
						int przelicznik = (gracz.memory[temp].wysokosc+maxWysokosc)/(2.0*maxWysokosc)*255;
						obrazPamieci.setPixel({(unsigned)(j+5),(unsigned)(5-i)},sf::Color(przelicznik,przelicznik/2,0));
					}
					else
					{
						obrazPamieci.setPixel({(unsigned)(j+5),(unsigned)(5-i)},sf::Color(100,100,100));
					}
					if(gracz.memory[temp].czyCel)
					{
						celWyswietl.setFillColor(sf::Color::Green);
						celWyswietl.setPosition({(j+5+0.25f) * size, (5-i+0.25f) * size});
						czyCelWyswietlony = true;
					}
				}
			}
		}
		teksturaPamieci.update(obrazPamieci);

		window.clear();
		window.draw(pamiec);
		if(czyCelWyswietlony)
		{
			window.draw(celWyswietl);
		}
		if(abs(gracz.memory[kordyGracza].wysokosc-gracz.memory[kordyGracza+coordinate{0,1}].wysokosc)<2)
		{
			ruchWyswietl.setFillColor(sf::Color::Blue);
			ruchWyswietl.setPosition({5.375f*size,4.375f*size});
			window.draw(ruchWyswietl);
		}
		if(abs(gracz.memory[kordyGracza].wysokosc-gracz.memory[kordyGracza+coordinate{0,-1}].wysokosc)<2)
		{
			ruchWyswietl.setFillColor(sf::Color::Blue);
			ruchWyswietl.setPosition({5.375f*size,6.375f*size});
			window.draw(ruchWyswietl);
		}
		if(abs(gracz.memory[kordyGracza].wysokosc-gracz.memory[kordyGracza+coordinate{-1,0}].wysokosc)<2)
		{
			ruchWyswietl.setFillColor(sf::Color::Blue);
			ruchWyswietl.setPosition({4.375f*size,5.375f*size});
			window.draw(ruchWyswietl);
		}
		if(abs(gracz.memory[kordyGracza].wysokosc-gracz.memory[kordyGracza+coordinate{1,0}].wysokosc)<2)
		{
			ruchWyswietl.setFillColor(sf::Color::Blue);
			ruchWyswietl.setPosition({6.375f*size,5.375f*size});
			window.draw(ruchWyswietl);
		}
		graczWyswietl.setFillColor(sf::Color::White);
		graczWyswietl.setPosition({5.25f * size, 5.25f * size});
		window.draw(graczWyswietl);

		window.display();
	}
}