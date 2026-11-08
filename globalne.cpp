#include "globalne.hpp"
#include <functional>
#include <cstdint>
#include <random>

opisPola plansza[planszaX][planszaY];
int planszaMinX=0;
int planszaMaxX=planszaX-1;
int planszaMinY=0;
int planszaMaxY=planszaY-1;

coordinate kordyCel = {0,0};

std::random_device rd; //pobiera z urzadzenia potrzebne do silnika
std::mt19937 gen(rd()); //silnik generatora

double losowanieDouble(double min, double max)
{
    std::uniform_real_distribution<double> distrib(min,max); //okreslanie zakresu (lewostronnie domkniety)
    double wylosowana = distrib(gen); //losowanie 
    return wylosowana;
}
int losowanieInt(int min, int max)
{
    std::uniform_int_distribution<int> distrib(min,max); //okreslanie zakresu (obustronnie domkniety)
    int wylosowana = distrib(gen); //losowanie 
    return wylosowana;
}
void fragmentacja(int nrFragmentu,int rozmiar)
{
	int ileNaOs = planszaX/rozmiar;
	planszaMinX= (nrFragmentu%ileNaOs)*rozmiar;
	planszaMaxX= planszaMinX+rozmiar-1;
	planszaMinY= (nrFragmentu/ileNaOs)*rozmiar;
	planszaMaxY= planszaMinY+rozmiar-1;
}
