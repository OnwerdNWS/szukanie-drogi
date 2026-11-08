#pragma once
#include "globalne.hpp"
#include "siecNeuronowa.hpp"
#include <cmath>
#include <unordered_map>



struct agent{
	coordinate kordy;
	coordinate startKordy;
	int8_t wysokosc;
	int8_t startWysokosc;
	Siec siec;
	Siec siecDocelowa;
	void ruch(bool nauka,bufor& pamiecDoNauki);
	int licznikRuchow = 0;
	float precyzja=0;
	opisPolaPamieci widokMapy[48];
	opisPolaPamieci widokPamieci[rozmiarPamieci];
	void updateView();
	std::unordered_map<coordinate, opisPolaPamieci, coordinateHash> memory;
	void updateMemory();
	void printMemory();
	void clearTemporaryMemory();
};
// void testAkcji(agent* bot, int wybranyRuch);

int indexMax(std::vector<float> tab);
float znajdzMax(std::vector<float> tab);

double obliczReward(opisPolaPamieci pole1, opisPolaPamieci pole2, coordinate kordy);
float obliczTarget(std::vector<float> nextQ,float reward, bool czyKoniec);



void przypiszTablice(opisPolaPamieci tab1[rozmiarPamieci], opisPolaPamieci tab2[rozmiarPamieci]);


struct KratkaNaPromieniu
{
    int cx, cy;
    double tWe, tWy;
};
struct paraNaroznikow
{
    int ax, ay;
    int bx, by;
    double t;
};
struct promien
{
    std::vector<KratkaNaPromieniu> kratki;
    std::vector<paraNaroznikow> narozniki;
};
struct celWidoku
{
    int dx, dy;
    promien promienie[5];
};
bool przedzial(double px, double py, int cx, int cy, double& lo, double& hi);
promien zbudujPromien(int docelowyX, int docelowyY, double px, double py);
std::vector<celWidoku> zbudujTabliceWidoku();
const std::vector<celWidoku>& tablicaWidoku();
void wysokosciOkna(coordinate agent, double h[7][7]);
bool promienWolny(const promien& pr, double oko, double hCel, double h[7][7]);
std::vector<bool> czyWidac(coordinate agent);

