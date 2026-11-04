#pragma once
#include "globalne.hpp"
#include "generatorMapy.hpp"
#include <cstdint>
#include <fstream>


int BFS(coordinate bot); //program szukający najkrótszej drogi oraz zwraca ilosc krokow
void planszaWyswietl(coordinate bot);
void generatorMapy(unsigned int seed); //rozstawia mape
void rozstawienie(coordinate& bot, unsigned int seed); //rozstawia agenta i cel

void zapiszPlansze(opisPola (&temp)[planszaX][planszaY], std::ofstream& plik);
void generujPlansze(int ilosc, std::string nazwaPlik);
void wczytajPlansze(opisPola (&temp)[planszaX][planszaY], int nrMapy, std::string nazwaPliku);
