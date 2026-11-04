Mój pierwszy wiêkszy projekt

1. O co w nim chodzi:
projekt polega na nauce uczenia maszynowego.
Stworzy³em w³asn¹ sieæ neuronow¹ która obs³uguje agenta którego zadaniem jest znaleŸæ cel na planszy 2D (bêdzie to póŸniej rozwijane). Agent zaczyna na losowym miejscu, mapie oraz losowym celu i jego zadaniem jest go znaleŸæ korzystaj¹c z pamiêci podrêcznej (kasowanej co próbê i przechowuj¹cej co agent widzi).
Otoczenie które agent widzi to jego jedyne informacjê (symuluje to odkrywanie nowych nie zbadanych miejsc)

2.kod który umo¿liwia kompilacjê
g++ -O3 -march=native -fopenmp-simd *.cpp -o program.exe -lsfml-graphics -lsfml-window -lsfml-system

tworzy on plik „program.exe” który nastêpnie trzeba uruchomiæ


projekt bêdzie jeszcze rozwijany
