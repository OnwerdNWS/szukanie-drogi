#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include "globalne.hpp"

double ReLU(double x);
double pochodnaReLU(double x);

struct WarstwaM{
    int wejscia, wyjscia;
	std::vector<float> wejscie;
	std::vector<float> wyjsciePrzedAktywacja;
	std::vector<float> wyjscie;

	const std::vector<float>* wejsciePaczka = nullptr;
	std::vector<float> wyjsciePaczki, wyjsciePrzedAktywacjaPaczki;
	std::vector<float> dZ, dX;

    std::vector<float> waga, bias;
    std::vector<float> gradientWaga, gradientBias;

    std::vector<float> mWaga, mBias;
    std::vector<float> vWaga, vBias;
    int t = 0;
	
	bool ostatnia = false;

    void inicjalizacja(int we, int wy);
	void losuj();
	std::vector<float> oblicz(const std::vector<float>& x);
	std::vector<float> obliczGradient(const std::vector<float>& delta);
	void obliczPaczke(const std::vector<float>& x, int iloscStanowPaczki);
	void wsteczPaczka(const std::vector<float>& delta, int iloscStanowPaczki);
	void zerujGradient();
	void aktualizuj(float learningRate);
	void zapisz(std::ofstream& plik);
	void odczyt(std::ifstream& plik);
};

struct Siec{
	std::vector<WarstwaM> warstwy;
	std::vector<float> wynik;
	std::string nazwaPliku;
	float learningRate = 0.0;
	
	void inicjalizacja(std::vector<int> architektura); //ustawia wielkosc sieci i parametry
	void losuj(); //losuje wartosci neuronow dla sieci
	std::vector<float> oblicz(const std::vector<float>& x); //pobiera wstepne dane i daje wynik calosci
	void obliczGradient(std::vector<float> target, int iloscPaczki);
	const std::vector<float>& obliczPaczka(const std::vector<float>& x, int iloscStanowPaczki);
	void wsteczPaczka(const std::vector<float>& delta, int iloscStanowPaczki);
	void zerujGradient();
	void aktualizuj();
	void zapisz();
	void odczyt();
};

std::vector<float> zmianaWidoku(opisPolaPamieci tab[rozmiarPamieci],int wysokoscAgenta);

struct experience{
	opisPolaPamieci polePrzedRuchem[rozmiarPamieci];
	opisPolaPamieci polePoRuchu[rozmiarPamieci];
	int8_t wysokoscPrzed;
	int8_t wysokoscPo;
	uint8_t strona;
	float reward;
	bool czyKoniec = false;
};
struct bufor{
	std::vector<experience> doswiadczenia;
	int dlugoscMax = 50000;
	int dlugosc = 0;
	int wskaznik = 0;	//wskazuje aktualne miejsce w tablicy nastepnego pola które się nadpisze/dopisze
	int momentNauki = 1000;
	void inicjalizacja();
	void dodaj(experience noweDoswiadczenie);
	std::vector<experience> losuj(int ilosc);
};

void redo(Siec& siec, bufor& pamiec);
