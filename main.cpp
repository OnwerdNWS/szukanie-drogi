#include <iostream>
#include "globalne.hpp"
#include "generatorMapy.hpp"
#include "bot.hpp"
#include "siecNeuronowa.hpp"
#include <cmath>
#include <vector>
#include <algorithm>
#include <chrono>

using namespace std;

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

void trening(int iloscSesji,int iloscProb, vector<int> architektura, float& precyzja)
{
	float lr = 0.001;
	bufor pamiecDoNauki;
	pamiecDoNauki.inicjalizacja();
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
			int nrMapy = losowanieInt(0,100000);
			int wynik = proba(temp,512,true, pamiecDoNauki,nrMapy);
			if(wynik>0) 
			{
				wygrane++;
			}
		}
		auto end = chrono::steady_clock::now();
		cout << wygrane << "/" << iloscProb << endl;
		auto czas = std::chrono::duration_cast<std::chrono::seconds>(end-start);
		cout << "czas: " << czas.count() << "s" << endl;
		temp.siec.zapisz();
		precyzja = temp.precyzja;
	}
}

void test(int iloscProb, vector<int> architektura)
{
	agent bot;
	bot.precyzja=10;
	bot.siec.inicjalizacja(architektura);
	bufor pamiecDoNauki;
	pamiecDoNauki.inicjalizacja();
	bot.siecDocelowa = bot.siec;

	auto start = chrono::steady_clock::now();
	double licznik=0;
	for(int i=0; i<iloscProb;i++)
	{
		int wynik = proba(bot,512,false,pamiecDoNauki, i+1000000);
		if(wynik>0) licznik++;
	}
	auto end = chrono::steady_clock::now();
	cout << "------------Test na " << iloscProb << " prob------------" << endl;
	cout << "zdawalnosc " << (licznik/iloscProb)*100 << "%" << endl;
	auto czas = std::chrono::duration_cast<std::chrono::seconds>(end-start);
	cout << "czas: " << czas.count() << "s" << endl;
	cout << "-----------------------------------------" << endl;
}

int main()
{
	vector<int> architektura = {600,256,64,4};
	float precyzja = 2.0;
	test(1000,architektura);
    trening(10,1000,architektura,precyzja);
	test(1000,architektura);
    return 0;
}
