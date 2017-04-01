Erick Silva
11-10969

# BranchAndBoundPRPP
Just an implementation of branch and bound to solve PRPP

# Requeriments

C++ 11+ to run the algorithm
Python3 to run the script that automatically 
			checks every solution

# Files

branchAndBound.cpp contiene el principal algoritmo,
	con la heuristica probabilistica.

funciones.h contiene las declaraciones de las funciones
	usadas en branchAndBound.

lado.h contiene la declaracion de la clase lado y sus metodos

correrinstancias.py es un archivo que lee las instancias a correr
	y puede generar las tablas necesarias

InstanciasACorrer.txt contiene las instancias que necesitan 
	ser corridas y las soluciones optimas a estas instancias

Makefile contiene instrucciones para el programa make para compilar
	el programa

# Compile

Para compilar se puede usar el comando Make o

	g++ -std=c++11 branchAndBound.cpp -o branchAndBound -O3