#pragma once
#include <cstdint>
#include <functional>

const int maxWysokosc = 32;
const int planszaX=128;
const int planszaY=128;
extern int planszaMinX;
extern int planszaMaxX;
extern int planszaMinY;
extern int planszaMaxY;

const int rozmiarPamieci = 120;

struct opisPola{
    int8_t wysokosc=0.0;
};
struct opisPolaPamieci{
    int8_t wysokosc=0.0;
	bool czyOdwiedzone = false;
	bool czyZnane = false;
	bool granicaMapy = false;
	bool czyCel = false;
};
extern opisPola plansza[planszaX][planszaY];
struct coordinate{ //przechowywuje kordy x,y
	int x;
	int y;
	
	bool operator==(const coordinate& other) const
	{ 
		return x == other.x && y == other.y;
	}
	coordinate operator+(const coordinate& other) const
	{
		return {x + other.x , y + other.y};
	}
	coordinate operator-(const coordinate& other) const
	{ 
		return {x - other.x , y - other.y};
	}
};
struct coordinateHash{ //hashowanie potrzebne do mapy agenta
	size_t operator()(const coordinate& c) const
	{
		return std::hash<int>{}(c.x)^(std::hash<int>{}(c.y) << 1);
	}
};

extern coordinate kordyCel;


double losowanieDouble(double min, double max);
int losowanieInt(int min, int max);