#include "globalne.hpp"
#include <functional>
#include <cstdint>
#include <random>

opisPola plansza[planszaX][planszaY];

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