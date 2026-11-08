# Mój pierwszy większy projekt

projekt polega na nauce uczenia maszynowego.
Stworzyłem własną sieć neuronową która obsługuje agenta którego zadaniem jest znaleźć cel na planszy 2D. Agent zaczyna na losowym miejscu, mapie oraz losowym celu i jego zadaniem jest go znaleźć korzystając z pamięci podręcznej (kasowanej co próbę i przechowującej co agent widzi).
Otoczenie które agent widzi to jego jedyne informacje (symuluje to odkrywanie nowych nie zbadanych miejsc)

## Wymagania
- Windows + MSYS2 (UCRT64) z kompilatorem g++ (C++17)
- SFML 3

## Kompilacja
```
g++ -O3 -march=native -fopenmp-simd *.cpp -o program.exe -lsfml-graphics -lsfml-window -lsfml-system
```

## Uruchomienie 
Program trenuje agenta, a potem testuje go na różnych mapach. Wagi zapisują się w folderze "dane z sieci" (program go tworzy o ile go nie ma)


## Plany
- wizualizacja przebiegu testu
- możliwość operowania agentem z tymi samymi danymi co on
- ruch w 360 stopniach

## Update
- naprawione uczenie (program się uczy poprawnie choć potrzebuje zacząć od mniejszych map)
- dodana fragmentacja map
- dodano regenerację neuronów (funkcja redo)
- dodano diagnostykę