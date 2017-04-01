all: branchAndBound.cpp lado.h funciones.h
	g++ -std=c++11 branchAndBound.cpp -o branchAndBound -O3