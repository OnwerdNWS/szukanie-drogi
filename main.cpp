#include <iostream>
#include "globalne.hpp"
#include "generatorMapy.hpp"
#include "bot.hpp"
#include "siecNeuronowa.hpp"
#include <cmath>
#include <vector>
#include <algorithm>
#include <chrono>

#include <SFML/Graphics.hpp>

using namespace std;

void diagnostyka(Siec& siec, bufor& pamiec)
{
    std::vector<experience> probka = pamiec.losuj(256);
    vector<int> aktywne;
	aktywne.resize(siec.warstwy[0].wyjscia);
    int wybory[4] = {0, 0, 0, 0};
    for(experience& e : probka)
    {
        std::vector<float> q = siec.oblicz(zmianaWidoku(e.polePrzedRuchem, e.wysokoscPrzed));
        wybory[indexMax(q)]++;
        for(int i=0;i<siec.warstwy[0].wyjscia;i++)
		{
			float v = siec.warstwy[0].wyjscie[i];
			if(0.0f < v && v < 0.99f) aktywne[i]++;
		}
    }
	int iloscAktywnych=0;
	for(int i=0; i<aktywne.size();i++)
	{
		if(aktywne[i]!=0)
		{
			iloscAktywnych++;
		}
	}
    std::cout << "aktywne neurony w1: " << 100.0 * iloscAktywnych / siec.warstwy[0].wyjscia << "%"
              << ", wybory ruchow: " << wybory[0] << " " << wybory[1] << " " << wybory[2] << " " << wybory[3] << std::endl;
}

int proba(agent &test, int maxKrokow, bool nauka,bufor& pamiecDoNauki, int seed)
{
	test.clearTemporaryMemory();
	generatorMapy(seed);
	rozstawienie(test.kordy,seed);
	test.startKordy=test.kordy;
	test.startWysokosc=plansza[test.kordy.x][test.kordy.y].wysokosc;
	int licznik=0;
	bool win=false;
	while(licznik<maxKrokow and !win)
	{
		test.ruch(nauka,pamiecDoNauki);
		if(test.kordy == kordyCel) win =true;
		licznik++;
	}
	if(win)
	{
		return licznik;
	}
	else
	{
		return 0;
	}
}

void trening(int iloscSesji,int iloscProb, vector<int> architektura, float& precyzja,int fragment)
{
	float lr = 0.0001;
	bufor pamiecDoNauki;
	pamiecDoNauki.inicjalizacja();
	int ruch = pow((planszaX/fragment),2)/2;
	for(int a=0; a<iloscSesji;a++)
	{
		agent temp;
		temp.siec.inicjalizacja(architektura);
		temp.siecDocelowa=temp.siec;
		temp.siec.learningRate=lr;
		temp.precyzja = precyzja;
		int wygrane=0;
		auto start = chrono::steady_clock::now();
		for(int c=0; c<iloscProb;c++)
		{
			int nrMapy = losowanieInt(0,1000000);
			for(int f=0;f<pow(fragment,2);f++)
			{
				planszaMinX= f%fragment;
				planszaMaxX= planszaMinX+fragment-1;
				planszaMinY= f/fragment;
				planszaMaxY= planszaMinY+fragment-1;
				int wynik = proba(temp,ruch,true, pamiecDoNauki,nrMapy);
				if(wynik>0) wygrane++;
			}
		}
		auto end = chrono::steady_clock::now();
		cout << wygrane << "/" << iloscProb*pow(fragment,2) << endl;
		auto czas = std::chrono::duration_cast<std::chrono::seconds>(end-start);
		cout << "czas: " << czas.count() << "s" << endl;
		temp.siec.zapisz();
		precyzja = temp.precyzja;
		diagnostyka(temp.siec,pamiecDoNauki);
		// planszaWyswietl(temp.startKordy);
	}
}

void test(int iloscProb, vector<int> architektura,int fragment)
{
	agent bot;
	bot.precyzja=10;
	bot.siec.inicjalizacja(architektura);
	bufor pamiecDoNauki;
	pamiecDoNauki.inicjalizacja();
	bot.siecDocelowa = bot.siec;
	int ruch = pow((planszaX/fragment),2)/4;
	auto start = chrono::steady_clock::now();
	float licznik=0;
	for(int i=0; i<iloscProb;i++)
	{
		for(int f=0;f<pow(fragment,2);f++)
		{
			planszaMinX= f%fragment;
			planszaMaxX= planszaMinX+fragment-1;
			planszaMinY= f/fragment;
			planszaMaxY= planszaMinY+fragment-1;
			int wynik = proba(bot,ruch,false,pamiecDoNauki, i+1000000);
			if(wynik>0) licznik++;
		}
	}
	auto end = chrono::steady_clock::now();
	cout << "------------Test na " << iloscProb*pow(fragment,2) << " prob------------" << endl;
	cout << "zdawalnosc " << (licznik/(iloscProb*pow(fragment,2)))*100 << "% (" << licznik << ")" << endl;
	auto czas = std::chrono::duration_cast<std::chrono::seconds>(end-start);
	cout << "czas: " << czas.count() << "s" << endl;
	cout << "-----------------------------------------" << endl;
}


int main()
{
	vector<int> architektura = {600,256,64,4};
	float precyzja = 2.0f;
	int fragment = planszaX/8;
	test(1,architektura,fragment);
    for(int i=0; i<10;i++)
	{
		trening(10,1,architektura,precyzja,fragment);
		test(1,architektura,fragment);
	}
    return 0;
}
