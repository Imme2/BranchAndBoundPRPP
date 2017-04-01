#ifndef LADO_H
#define LADO_H

#include <string>

// A small class in order to contain each side of the graph
class lado
{
public:
	int vi,vf,beneficio,costo;
	lado(int verticeInicial, int verticeFinal, int b, int c){
		vi = verticeInicial;
		vf = verticeFinal;
		beneficio = b;
		costo = c;
	}

	lado(){
		vi = 0;
		vf = 0;
		beneficio = 0;
		costo = 0;
	}

	// for debugging
	std::string toString(){
		std::string aux = "";
		aux += std::to_string(vi) + " " + std::to_string(vf) + " ";
		aux += "b " + std::to_string(beneficio) + " c " + std::to_string(costo);
		return aux;
	}

	/* data */
};


#endif