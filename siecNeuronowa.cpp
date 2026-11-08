#include "siecNeuronowa.hpp"
#include "globalne.hpp"
#include <math.h>
#include <string>
#include <filesystem>

double ReLU(double x)
{
	if(x>0) return x;
	return 0;
}
double pochodnaReLU(double x)
{
	if(x>0) return 1;
	
	return 0;
}

void WarstwaM::inicjalizacja(int we,int wy)
{
	wejscia = we;
	wyjscia = wy;
	waga.assign(wy * we, 0.0f);
	bias.assign(wy, 0.0f);
	gradientWaga = mWaga = vWaga = waga;
	wyjscie = gradientBias = mBias = vBias = bias;
	int maxPaczka = 32;
	wyjsciePrzedAktywacjaPaczki.assign(maxPaczka * wy, 0.0f);
	wyjsciePaczki = dZ = wyjsciePrzedAktywacjaPaczki;
	dX.assign(maxPaczka * we, 0.0f);
}
void WarstwaM::losuj()
{
	for(int i=0; i<waga.size();i++)
	{
		waga[i] = losowanieDouble(-0.1,0.1);
	}
	for(int i=0; i<bias.size();i++)
	{
		bias[i] = losowanieDouble(-0.1,0.1);
	}
}
std::vector<float> WarstwaM::oblicz(const std::vector<float>& x)
{
	wejscie = x;
	wyjsciePrzedAktywacja.assign(wyjscia, 0.0f);
	for(int i=0; i<wyjscia; i++)
	{
		wyjsciePrzedAktywacja[i] = bias[i];
		for(int j=0; j<wejscia; j++)
		{
			wyjsciePrzedAktywacja[i] += waga[i * wejscia + j] * x[j];
		}
		if(ostatnia) // Apply ReLU for all but the last layer
		{
			wyjscie[i] = wyjsciePrzedAktywacja[i];
		}
		else
		{
			wyjscie[i] = ReLU(wyjsciePrzedAktywacja[i]); // No activation for the last layer
		}
	}
	return wyjscie;
}
std::vector<float> WarstwaM::obliczGradient(const std::vector<float>& delta)
{
	std::vector<float> delty(wejscia, 0.0f);
	for(int i=0; i<wyjscia; i++)
	{
		float deltaValue = delta[i];
		if(!ostatnia) // Apply derivative of ReLU for all but the last layer
		{
			deltaValue *= pochodnaReLU(wyjsciePrzedAktywacja[i]);
		}
		for(int j=0; j<wejscia; j++)
		{
			delty[j] += deltaValue * waga[i * wejscia + j];
			gradientWaga[i * wejscia + j] += deltaValue * wejscie[j];
		}
		gradientBias[i] += deltaValue;
	}
	return delty;
}
void WarstwaM::zerujGradient()
{
	std::fill(gradientWaga.begin(), gradientWaga.end(), 0.0f);
	std::fill(gradientBias.begin(), gradientBias.end(), 0.0f);
}
void WarstwaM::aktualizuj(float learningRate)
{
	t++;
	double potegaM = pow(0.9,t);
	double potegaV = pow(0.999,t);
	float roznicaM = 1-0.9;
	float roznicaV = 1-0.999;
	double e = 0.000000001;
	for(int i=0; i<waga.size();i++)
	{
		mWaga[i] = 0.9*mWaga[i]+(roznicaM)*gradientWaga[i];
		vWaga[i] = 0.999*vWaga[i]+(roznicaV)*gradientWaga[i]*gradientWaga[i];
		double tempM = mWaga[i]/(1-potegaM);
		double tempV = vWaga[i]/(1-potegaV);
		waga[i] -= learningRate*tempM/(sqrt(tempV)+e);
	}
	for(int i=0; i<bias.size();i++)
	{
		mBias[i] = 0.9*mBias[i]+(roznicaM)*gradientBias[i];
		vBias[i] = 0.999*vBias[i]+(roznicaV)*gradientBias[i]*gradientBias[i];
		double tempM = mBias[i]/(1-potegaM);
		double tempV = vBias[i]/(1-potegaV);
		bias[i] -= learningRate*tempM/(sqrt(tempV)+e);
	}
}
void WarstwaM::zapisz(std::ofstream& plik)
{
	for(int i=0; i<wejscia*wyjscia; i++)
	{
		plik.write(reinterpret_cast<char*>(&waga[i]), sizeof(waga[i]));
		plik.write(reinterpret_cast<char*>(&mWaga[i]), sizeof(mWaga[i]));
		plik.write(reinterpret_cast<char*>(&vWaga[i]), sizeof(vWaga[i]));
	}
	for(int i=0; i<wyjscia; i++)
	{
		plik.write(reinterpret_cast<char*>(&bias[i]), sizeof(bias[i]));
		plik.write(reinterpret_cast<char*>(&mBias[i]), sizeof(mBias[i]));
		plik.write(reinterpret_cast<char*>(&vBias[i]), sizeof(vBias[i]));
	}
	plik.write(reinterpret_cast<char*>(&t), sizeof(t));
}
void WarstwaM::odczyt(std::ifstream& plik)
{
	for(int i=0; i<wejscia*wyjscia; i++)
	{
		plik.read(reinterpret_cast<char*>(&waga[i]), sizeof(waga[i]));
		plik.read(reinterpret_cast<char*>(&mWaga[i]), sizeof(mWaga[i]));
		plik.read(reinterpret_cast<char*>(&vWaga[i]), sizeof(vWaga[i]));
	}
	for(int i=0; i<wyjscia; i++)
	{
		plik.read(reinterpret_cast<char*>(&bias[i]), sizeof(bias[i]));
		plik.read(reinterpret_cast<char*>(&mBias[i]), sizeof(mBias[i]));
		plik.read(reinterpret_cast<char*>(&vBias[i]), sizeof(vBias[i]));
	}
	plik.read(reinterpret_cast<char*>(&t), sizeof(t));
}

void WarstwaM::obliczPaczke(const std::vector<float>& x, int iloscStanowPaczki)
{
	wejsciePaczka = &x;
	for(int i=0; i<iloscStanowPaczki; i++)
	{
		const float* xPaczki = &x[i * wejscia];
		for(int j=0; j<wyjscia; j++)
		{
			float suma = bias[j];
			const float* wagaPaczki = &waga[j * wejscia];
			#pragma omp simd reduction(+:suma)
			for(int k=0; k<wejscia; k++) suma += wagaPaczki[k] * xPaczki[k];
			wyjsciePrzedAktywacjaPaczki[i * wyjscia + j] = suma;
			if(ostatnia) // Apply ReLU for all but the last layer
			{
				wyjsciePaczki[i * wyjscia + j] = suma;
			}
			else
			{
				wyjsciePaczki[i * wyjscia + j] = ReLU(suma); // No activation for the last layer
			}
		}
	}
}
void WarstwaM::wsteczPaczka(const std::vector<float>& delta, int iloscStanowPaczki)
{
	const std::vector<float>& xPaczki = *wejsciePaczka;

	for(int i=0; i<iloscStanowPaczki*wyjscia;i++)
	{
		if(ostatnia || wyjsciePrzedAktywacjaPaczki[i] > 0) dZ[i] = delta[i];
		else dZ[i] = 0.0f;
	}

	for(int i=0; i<wyjscia; i++)
	{
		float* gradientWagaPaczki = &gradientWaga[i * wejscia];
		for(int j=0; j<iloscStanowPaczki; j++)
		{
			float deltaValue = dZ[j * wyjscia + i];
			if(deltaValue == 0.0f) continue;
			const float* x = &xPaczki[j * wejscia];
			for(int k=0; k<wejscia; k++) gradientWagaPaczki[k] += deltaValue * x[k];
			gradientBias[i] += deltaValue;
		}
	}
	std::fill(dX.begin(), dX.end(), 0.0f);
	for(int i=0; i<iloscStanowPaczki; i++)
	{
		float* dXRow = &dX[i * wejscia];
		for(int j=0; j<wyjscia; j++)
		{
			float deltaValue = dZ[i * wyjscia + j];
			if(deltaValue == 0.0f) continue;
			const float* wagaRow = &waga[j * wejscia];
			for(int k=0; k<wejscia; k++) dXRow[k] += deltaValue * wagaRow[k];
		}
	}
}

void Siec::inicjalizacja(std::vector<int> architektura)
{
	std::filesystem::path dir("dane z sieci");
	if(!std::filesystem::exists(dir))
	{
		std::filesystem::create_directory(dir);
	}
	warstwy.resize(architektura.size()-1);
	for(int i=0;i<warstwy.size();i++)
	{
		warstwy[i].inicjalizacja(architektura[i],architektura[i+1]);
	}
	warstwy.back().ostatnia = true;
	nazwaPliku ="dane z sieci/siec";
	for(int i=0; i<architektura.size(); i++)
	{
		nazwaPliku+="_"+std::to_string(architektura[i]);
	}
	nazwaPliku+=".dat";
	odczyt();
}
void Siec::losuj()
{
	for(int i=0; i<warstwy.size();i++)
	{
		warstwy[i].losuj();
	}
}
std::vector<float> Siec::oblicz(const std::vector<float>& x)
{
	wynik = x;
	for(int i=0; i<warstwy.size();i++)
	{
		wynik = warstwy[i].oblicz(wynik); //zwraca wynik ktry przechodzi do kolejnej warstwy (lub koca sieci)
	}
	return wynik;
}
void Siec::obliczGradient(std::vector<float> target, int iloscPaczki)
{
	std::vector<float> delta;
	for(int i=0;i<target.size();i++)
	{
		delta.push_back((wynik[i]-target[i])/iloscPaczki);
	}
	if(delta.size()>1)
	{
		for(int i=warstwy.size()-1; i>=0;i--)
		{
			delta = warstwy[i].obliczGradient(delta);//powinno oddawac delt� z danej warstwy aby przekaza� j� do poprzedniej
		}
	}
	
}
void Siec::zerujGradient()
{
	for(int i=0; i<warstwy.size();i++)
	{
		warstwy[i].zerujGradient();
	}
}
void Siec::aktualizuj()
{
	for(int i=0; i<warstwy.size();i++)
	{
		warstwy[i].aktualizuj(learningRate);
	}
}
void Siec::zapisz()
{
	std::ofstream plik;
	plik.open(nazwaPliku, std::ios::binary);
	for(int i=0; i<warstwy.size();i++)
	{
		warstwy[i].zapisz(plik);
	}
	plik.close();
}
void Siec::odczyt()
{
	std::ifstream plik;
	plik.open(nazwaPliku, std::ios::binary);
	if(!plik)
	{
		losuj();
		return;
	}
	for(int i=0; i<warstwy.size();i++)
	{
		warstwy[i].odczyt(plik);
	}
	plik.close();
}

const std::vector<float>& Siec::obliczPaczka(const std::vector<float>& x, int iloscStanowPaczki)
{
	const std::vector<float>* wejsciePaczki = &x;
	for(int i=0; i<warstwy.size();i++)
	{
		warstwy[i].obliczPaczke(*wejsciePaczki, iloscStanowPaczki);
		wejsciePaczki = &warstwy[i].wyjsciePaczki;
	}
	return warstwy.back().wyjsciePaczki;
}
void Siec::wsteczPaczka(const std::vector<float>& delta, int iloscStanowPaczki)
{
	const std::vector<float>* deltaPaczki = &delta;
	for(int i=warstwy.size()-1; i>=0;i--)
	{
		warstwy[i].wsteczPaczka(*deltaPaczki, iloscStanowPaczki);
		deltaPaczki = &warstwy[i].dX;
	}
}


std::vector<float> zmianaWidoku(opisPolaPamieci tab[rozmiarPamieci], int wysokoscAgenta)
{
	std::vector<float> widok;
	for(int i=0;i<rozmiarPamieci;i++)
	{
		float roznica = (tab[i].wysokosc-wysokoscAgenta)/maxWysokosc;
		if(tab[i].czyZnane and !tab[i].granicaMapy) widok.push_back(roznica);
		else widok.push_back(0.0f);
		widok.push_back(tab[i].czyOdwiedzone);
		widok.push_back(tab[i].granicaMapy);
		widok.push_back(tab[i].czyZnane);
		widok.push_back(tab[i].czyCel);
	}
	return widok;
}


void bufor::inicjalizacja()
{
    doswiadczenia.resize(dlugoscMax);
}
void bufor::dodaj(experience noweDoswiadczenie)
{
    if(wskaznik>=dlugoscMax) wskaznik=0;
    if(dlugosc<dlugoscMax) dlugosc++;

    doswiadczenia[wskaznik] = noweDoswiadczenie;
    wskaznik++;
}
std::vector<experience> bufor::losuj(int ilosc)
{
	std::vector<experience> temp;
	for(int i=0; i<ilosc; i++)
	{
		temp.push_back(doswiadczenia[losowanieInt(0,dlugosc-1)]);
	}
	return temp;
}