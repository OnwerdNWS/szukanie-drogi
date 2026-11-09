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
