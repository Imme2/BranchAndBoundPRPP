// Autor: Erick Silva
// Carnet: 11-10969

#include <vector>
#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>
#include <queue>
#include <string>
#include <string.h>
#include <fstream>
#include <algorithm>
#include <limits.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "lado.h"
using namespace std;

#include "funciones.h"


vector<lado> mejorSol, solParcial;
int beneficioDisponible, valorMejorSol,valorSolParcial;

#define mp make_pair
#define ff first
#define ss second
#define DEPOSITO 1


typedef pair<int,int> pii;


// Usamos variables globales para el grafo por comodidad y rapidez.
vector< vector< int > > grafo;
int beneficio[105][105];
int costo[105][105];
int beneficioOriginal[105][105];

// Para el dijkstra
int padre[105];

int main(int argc, char* argv[]){
	int nroVertices;
	int nroLados;
	int nroLadosR;

	string nombre_instancia;
	ifstream input;
	ofstream output;

	if (argc > 2){
		cout << "El programa espera el path al archivo de la instancia" << endl;
		exit(0);
	}
	if (argc == 1){
		cout << "Coloque el nombre o path a la instancia" << endl;
		cin >> nombre_instancia;
	}
	if (argc == 2){
		nombre_instancia = string(argv[1]);
	}

	input.open(nombre_instancia);

	// Lectura:

	// Variable auxiliar para deshacernos de parte del input innecesaria.
	string aux;
	
	// Reading the input, we use aux to ignore certain string tokens.
	// number of vertices : {int}
	input >> aux >> aux >> aux >> aux >> nroVertices;
	// number of required edges {int}
	input >> aux >> aux >> aux >> aux >> nroLadosR;

	// inicializamos los arreglos y el grafo.
	grafo.resize(nroVertices+1);
	memset(beneficio,-1,sizeof(beneficio));
	memset(costo,-1,sizeof(costo));
	beneficioDisponible = 0;
	int v0,v1,c,b;
	for (int i = 0; i < nroLadosR; i++){
		input >> v0 >> v1 >> c >> b;
		grafo[v0].push_back(v1);
		grafo[v1].push_back(v0);
		beneficioDisponible += max(0,b - c);
		beneficioOriginal[v0][v1] = b;
		beneficioOriginal[v1][v0] = b;
		beneficio[v0][v1] = b;
		costo[v0][v1] = c;
		beneficio[v1][v0] = b;
		costo[v1][v0] = c;
	}

	// number of non required edges  {int} 
	input >> aux >> aux >> aux >> aux >> aux >> nroLados;
	for (int i = 0; i < nroLados; i++){
		input >> v0 >> v1 >> c >> b;
		grafo[v0].push_back(v1);
		grafo[v1].push_back(v0);
		beneficioDisponible += max(0,b - c);
		beneficioOriginal[v0][v1] = b;
		beneficioOriginal[v1][v0] = b;	
		beneficio[v0][v1] = b;
		costo[v0][v1] = c;
		beneficio[v1][v0] = b;
		costo[v1][v0] = c;
	}

	// Cerramos el archivo de input.
	input.close();

	// Fin Lectura.

	// Populamos el grafo T (Lados que tienen beneficio).

	vector < vector <int> > grafoT = obtenerGrafoT(nroVertices);

	// Inicializamos la semilla del al azar que se va a usar en el greedy.
	srand(time(NULL));

	// Con esto sacamos una solucion principal, se usa el algoritmo
	//  propuesto en el enunciado.
	vector<int> solucion = construirSolucion(nroVertices,grafoT);
	int valorSolucion = 0;

	// Mejora de solucion.
	solucion = elimCiclosNegativos(solucion);

	// Calculamos el beneficio de la solucion inicial
	reSetBeneficio();
	for (unsigned int i = 0 ; i < solucion.size() - 1;i++){
		valorSolucion += beneficio[solucion[i]][solucion[i+1]];
		valorSolucion -= costo[solucion[i]][solucion[i+1]];
		beneficio[solucion[i]][solucion[i+1]] = 0;
		beneficio[solucion[i+1]][solucion[i]] = 0;
	}

	// Si la solucion greedy no lo logra. Vamos a 0.
	if (valorSolucion < 0){
		valorSolucion = 0;
		solucion.resize(1);
		solucion[0] = 1;
	}

	cout << "Valor Solucion Heuristica\t" << valorSolucion << endl;

	// Branch and bound


	// Se resetea el valor de la matriz de beneficios
	reSetBeneficio();

	// Se construye un vector a partir de la solucion que sea
	//  compatible con las estructuras en el branch and bound
	vector<lado> solucionLados;
	solucionLados.push_back(lado(1,1,0,0));
	for (int i = 1 ; i < (int)solucion.size(); i++){
		solucionLados.push_back(lado(solucion[i-1],solucion[i],0,0));
	}

	// Se empieza el tiempo
	clock_t t;
	// Se corre branch and bound
	branchAndBound(valorSolucion,solucionLados);
	// Se termina el tiempo
	t = clock() - t;
	// Se imprime la solucion por la salida estandar
	cout << "Valor Solucion\t" <<valorMejorSol << endl;
	double secs = (double)t/CLOCKS_PER_SEC;
	cout << "Tiempo\t" << secs << endl;
}


// Funcion de branch and bound.
// Inicia las variables globales y llama al backtracking.
vector<lado> branchAndBound(int valorSolInicial, vector<lado> solInicial){
	solParcial.push_back(lado(DEPOSITO,DEPOSITO,0,0));
	mejorSol = solInicial;
	valorSolParcial = 0;
	valorMejorSol = valorSolInicial;
	dfs();
	return mejorSol;
}



//Backtracking del branch and bound
// Usa los acotamientos y ciertos prunes de soluciones
// para reducir el espacio de busqueda de un dfs normal
void dfs(){
	lado ultimo = solParcial[solParcial.size()-1];


	// Si se enuentra una mejor solucion, se guarda.
	if (ultimo.vf == DEPOSITO){
		if (valorSolParcial > valorMejorSol){
			valorMejorSol = valorSolParcial;
			mejorSol = solParcial;
		}
	}

	// Se buscan los sucesores al estado actual
	vector<lado> listaSucesores = obtenerListaSucesores(ultimo.vf);
	bool usado;


	// Se recorren en estilo dfs
	for (lado i: listaSucesores){
		usado = usadoEnSolParcial(i,solParcial);
		// Si el lado ya ha sido usado y el beneficio del mismo
		//  no es 0, este lado se debe eliminar.
		if (usado and i.beneficio != 0){
			continue;
		}
		// pruning para lados que aparecen en la lista con beneficio 0
		// sin haber sido usados todavia. (consecuencia de representacion de grafo)
		if (!usado and i.beneficio == 0 and beneficio[i.vi][i.vf] != 0){
			continue;
		}
		// Principal backtracking que decide si el lado
		// debe ser escogido.
		if (!cicloNegativo(i,solParcial) and 
			!estaEnSolParcial(i,solParcial) and 
			!repiteCiclo(i,solParcial) and
			cumpleAcotamiento(i,solParcial))
		{
			agregarLado(i,solParcial);
			beneficioDisponible -= max(0,i.beneficio - i.costo);
			dfs();
		}
	}

	// Al salir de una de las recursiones se debe
	//  eliminar el ultimo para mantener el valor de l
	ultimo = solParcial[solParcial.size()-1];
	beneficioDisponible += max(0,ultimo.beneficio - ultimo.costo);
	eliminarUltimoLado(ultimo, solParcial);
}


// Comparador de lados que se usa para ordenar.
bool ladoCompare(lado lhs,lado rhs){
	return lhs.beneficio - lhs.costo > rhs.beneficio - rhs.costo;
}

// obtencion de la lista de sucesores.
vector<lado> obtenerListaSucesores(int vertice){
	vector<lado> listaSuc;
	for (int i: grafo[vertice]){
		listaSuc.push_back(lado(vertice,i,beneficio[vertice][i],costo[vertice][i]));
		listaSuc.push_back(lado(vertice,i,0,costo[vertice][i]));
	}
	sort(listaSuc.begin(),listaSuc.end(),ladoCompare);
	return listaSuc;
}

// verificacion de ciclos negativos, solo se chequea el ciclo
//  mas peque;o formado
bool cicloNegativo(lado l,vector<lado> &solParcial){
	lado aux;
	int valorCiclo = 0;
	int ciclo = (int)solParcial.size();
	for (int i = (int)solParcial.size()-1; i >= 0; i--){
		aux = solParcial[i];
		if (aux.vi == l.vf){
			ciclo = i;
			break;
		}
	}
	for (int i = ciclo; i < (int)solParcial.size(); i++){
		aux = solParcial[i];
		valorCiclo += aux.beneficio - aux.costo;
	}
	if (ciclo != (int)solParcial.size()){
		valorCiclo += l.beneficio - l.costo;
	}
	if (valorCiclo < 0){
		return true;
	}
	return false;
}

// funcion que chequea si un lado ha sido usado en la solucion parcial.
bool usadoEnSolParcial(lado l,vector<lado> &solParcial){
	for (lado i: solParcial){
		if (i.vf == l.vf and i.vi == l.vi){
			return true;
		}
		if (i.vi == l.vf and i.vf == l.vi){
			return true;
		}
	}
	return false;
}

// Funcion que chequea si un lado ha sido usado por segunda vez
//  en la solucion parcial
// Siempre que el lado exista con beneficio = 0 entonces es que
//  se ha usado ya dos veces.
bool estaEnSolParcial(lado l,vector<lado> &solParcial){
	for (lado i: solParcial){
		if (i.vf == l.vf and i.vi == l.vi and i.beneficio == 0){
			return true;
		}
		if (i.vi == l.vf and i.vf == l.vi and i.beneficio == 0){
			return true;
		}
	}
	return false;
}

// Funcion que verifica si un lado esta repitiendo un ciclo
//  ya explorado.
bool repiteCiclo(lado l,vector<lado> &solParcial){
	int blado = l.beneficio - l.costo;
	for (lado lsol: solParcial){
		if (lsol.vi == l.vf){
			if (lsol.beneficio - lsol.costo < blado){
				return true;
			}
		}
	}
	return false;
}


// funcion auxiliar para agregar un lado a la solucion parcial
//  actualiza tambien el valor de la misma.
void agregarLado(lado l,vector<lado> &solParcial){
	solParcial.push_back(l);
	valorSolParcial += l.beneficio - l.costo;
}

// funcion auxiliar para eliminar el ultimo lado de la solucion parcial
//  actualiza tambien el valor de la misma.
void eliminarUltimoLado(lado l,vector<lado> &solParcial){
	solParcial.pop_back();
	valorSolParcial -= l.beneficio - l.costo;
}

// Funcion que chequea si se cumple el acotamiento (el beneficio maximo).
bool cumpleAcotamiento(lado l,vector<lado> &solParcial){
	int bLado = l.beneficio - l.costo;
	int beneficioSolParcial = bLado + valorSolParcial;
	int maxBeneficio = beneficioDisponible - max(0,bLado) + beneficioSolParcial;

	if (maxBeneficio <= valorMejorSol) return false;
	return true;
}



// Funcion auxiliar que coloca los valores originales en la 
//  matriz de beneficios
void reSetBeneficio(){
	for (int i = 0; i < 105; i++){
		memcpy(beneficio[i],beneficioOriginal[i],sizeof(beneficioOriginal[i]));
	}
}

// Funcion auxiliar que une un camino dado a la solucion.
void unirCaminoASolucion(vector<int> &camino, vector<int> &solucion){
	// Empezamos desde camino.size()-2 porque el ultimo es el primero
	//  de solucion.
	reverse(camino.begin(),camino.end());
	int aux = solucion[solucion.size()-1];
	for (int i = 1; i < (int)camino.size(); i++){
		beneficio[aux][camino[i]] = 0;
		beneficio[camino[i]][aux] = 0;
		solucion.push_back(camino[i]);
		aux = camino[i];
	}

}


// funcion que usa numeros aleatorios pesados para retornar un lado
//  a usar del vertice.
// Para el numero aleatorio pesado solo crea un arreglo de pesos y
//  se pide un numero aleatorio entre 0 y TotalPesos.
int obtenerLado(vector< vector<int>  > grafo,int vertice){
	vector<int> listaLados = grafo[vertice];
	vector<int> pesos;
	int pesoActual = 0;
	// Asignacion de pesos.
	for (int i = 0 ; i < (int)listaLados.size(); i++){
		pesos.push_back(pesoActual);
		pesoActual +=  beneficio[vertice][listaLados[i]] - costo[vertice][listaLados[i]];
	}
	int randomNumber = rand() % (pesoActual + 1);
	// Eleccion del lado ganador.
	for (int i = 1 ; i < (int)pesos.size();i++){

		if (randomNumber <= pesos[i]){
			return listaLados[i-1];
		}
	}
	return listaLados[listaLados.size()-1];
}

// funcion que usa numeros aleatorios pesados para retornar un camino
//  a usar segun sus costos.
// Para el numero aleatorio pesado solo crea un arreglo de pesos y
//  se pide un numero aleatorio entre 0 y TotalPesos.
vector<int> obtenerCamino(vector< pair< vector<int>, int > > caminos){
	vector<int> pesos;
	pesos.resize(caminos.size());
	int pesoActual = 0;
	int menor = 0;
	// Calculamos el menor para no trabajar con pesos negativos
	for (int i = 0 ; i < (int)caminos.size(); i++){
		pesos[i] = caminos[i].ss;
		menor = min(menor,caminos[i].ss);
	}

	// sumamos a todos los valores el menor
	pesos[0] = pesos[0] - menor;
	pesoActual = pesos[0];
	for (int i = 1 ; i < (int)pesos.size(); i++){
		pesos[i] = pesos[i] - menor;
		pesoActual += pesos[i];
		pesos[i] = pesos[i] + pesos[i-1];
	}
	int randomNumber = rand() % (pesoActual + 1);
	// Se elige el ganador.
	for (int i = 1 ; i < (int)pesos.size();i++){
		if (randomNumber <= pesos[i]){
			return caminos[i-1].ff;
		}
	}
	return caminos[caminos.size()-1].ff;


}

// Funcion que borra un lado del grafo.
int borrarLado(int verticeInicial, int verticeFinal, vector<vector<int> > &grafo){
	int borrados = 0;
	for (vector<int>::iterator i = grafo[verticeInicial].begin(); i != grafo[verticeInicial].end(); ++i){
		if (*i == verticeFinal){
			grafo[verticeInicial].erase(i);
			borrados++;
			break;
		}
	}
	for (vector<int>::iterator i = grafo[verticeFinal].begin(); i != grafo[verticeFinal].end(); ++i){
		if (*i == verticeInicial){
			borrados++;
			grafo[verticeFinal].erase(i);
			break;
		}
	}
	return borrados;
}

// Funcion auxiliar que actualiza los beneficios y agrega
//  un lado a la solucion.
void agregarLadoASolucion(int verticeInicial, int verticeFinal, vector <int> &solucion){
	beneficio[verticeInicial][verticeFinal] = 0;
	beneficio[verticeFinal][verticeInicial] = 0;
	solucion.push_back(verticeFinal);
}

// Funcion auxiliar que obtiene el tama;o del grafo.
int obtenerTamanoGrafo(int nroVertices,vector < vector <int> > &grafoT){
	int tam = 0;
	for (int i = DEPOSITO; i < nroVertices;i++){
		tam += grafoT[i].size();
	}
	return tam;
}

// Funcion principal de la heuristica donde se construye la solucion.
vector<int> construirSolucion(int nroVertices, vector < vector <int> > &grafoT){
	vector<int> solucion;
	vector< pair<vector<int>,int> > caminos;
	vector<int> camino;
	int tamGrafoT = obtenerTamanoGrafo(nroVertices,grafoT);
	int ultimo = DEPOSITO;
	int menorCosto,menorLado,costoActual,proxVertice;

	// El primer vertice es el deposito.
	solucion.push_back(DEPOSITO);

	// Si no hay ningun lado en el grafo T que contenga al deposito
	// buscamos al de menor costo.

	if (grafoT[DEPOSITO].size() == 0){
		menorCosto = INT_MIN;
		menorLado = -1;
		for (unsigned int i = 0; i < grafo[DEPOSITO].size();i++){
			costoActual = beneficio[DEPOSITO][i] - costo[DEPOSITO][i];
			if (costoActual < menorCosto){
				menorLado = grafo[DEPOSITO][i];
				menorCosto = costoActual;
			}
		}
		if (menorLado == -1){
			return solucion;
		}
		else{
			ultimo = menorLado;
			agregarLadoASolucion(DEPOSITO,menorLado,solucion);
		}
	}

	// Ciclo principal donde se intenta agregar
	pair< vector<int>, int > aux;

	while (tamGrafoT > 0){
		// Si podemos enontrar un lado en el grafo T lo tomamos
		// de otra forma encontramos caminos y elegimos de forma
		// al azar entre ellos.
		if (grafoT[ultimo].size() != 0){
			proxVertice = obtenerLado(grafoT,ultimo);
			tamGrafoT -= borrarLado(ultimo,proxVertice,grafoT);
			agregarLadoASolucion(ultimo,proxVertice,solucion);
			ultimo = proxVertice;
		}
		else{
			caminos.resize(0);

			for (int i = DEPOSITO;i < nroVertices;i++){
				for (int j = 0; j < (int)grafoT[i].size();j++){
					aux = dijkstra(grafoT[i][j],ultimo);
					if (aux.ff.size() != 0){
						caminos.push_back(aux);
					}
				}
			}

			camino = obtenerCamino(caminos);
			borrarLado(ultimo,camino[0],grafoT);
			for (int i = 0; i < (int)camino.size()-1;i++){
				tamGrafoT -= borrarLado(camino[i],camino[i+1],grafoT);
			}
			unirCaminoASolucion(camino,solucion);
			ultimo = solucion[solucion.size()-1];
		}
	}
	// Si al terminar el ultimo no es el deposito, vamos
	//  a el con dijkstra.
	if (ultimo != DEPOSITO){
		camino = dijkstra(DEPOSITO,ultimo).ff;
		unirCaminoASolucion(camino,solucion);
	}


	return solucion;

}


// Funcion que obtiene el Grafo T (dado por lados que dan beneficio)
vector< vector<int> > obtenerGrafoT(int nroVertices){
	vector< vector<int> > grafoT;
	grafoT.resize(nroVertices+1);
	for (int i = 1; i <= nroVertices; i++){
		for (int j = 1; j <= nroVertices; j++){
			if (i != j and costo[i][j] >= 0 and beneficio[i][j] - costo[i][j] >= 0){
				grafoT[i].push_back(j);
			}
		}
	}
	return grafoT;
}

// Retorna el menor camino basado unicamente en costo, solo
// recorre una vez cada nodo para evitar ciclos negativos.
pair< vector<int>, int > dijkstra(int start, int end, bool useNegative){
	priority_queue< pii, vector<pii>, greater<pii> > cola;

	cola.push(mp(0,start));
	pii actual;
	int nodoActual;
	int costoActual;
	// Se resettea el arreglo de padres
	memset(padre,-1,sizeof(padre));
	// variables auxiliares para costo y beneficio.
	int c,b; 

	padre[start] = start;

	// Vector donde guardaremos el camino
	vector<int> camino;
	// nodo auxiliar
	int nodo,ady,ganancia;


	while (!cola.empty()){
		actual = cola.top();
		cola.pop();
		nodoActual = actual.ss;
		costoActual = actual.ff;

		// Si llegamos al final, reconstruimos el camino
		if (nodoActual == end){
			// usamos el nodo para iterar hasta que encontremos el final.
			ganancia = 0;
			nodo = nodoActual;
			while (padre[nodo] != nodo){
				camino.push_back(nodo);
				ganancia += beneficio[padre[nodo]][nodo] - costo[padre[nodo]][nodo];
				nodo = padre[nodo];
			}
			camino.push_back(nodo);
			reverse(camino.begin(), camino.end());
			return mp(camino,ganancia);
		}

		// Si no, seguimos iterando.
		for (unsigned int i = 0; i < grafo[nodoActual].size();i++){
			ady = grafo[nodoActual][i];
			c = costo[nodoActual][ady];
			b = beneficio[nodoActual][ady];
			if ((padre[ady] == -1) and (useNegative or c <= b)){
				padre[ady] = nodoActual;
				cola.push(mp(costoActual+c - b,ady));
			}
		}
	}

	return mp(camino,0);
}

// Funcion de mejoramiento de soluciones, intenta eliminar ciclos
// negativos de la solucion dada.
vector<int> elimCiclosNegativos(vector<int> solucion){
	int v1,v3;
	int i1,i3;
	int valorSolucion = 0, valorSolucionCiclo;
	int valorMenorCiclo = 0;
	pair<int,int> menorCiclo;
	bool change =true;
	reSetBeneficio();
	for (unsigned int i = 0 ; i < solucion.size() - 1;i++){
		valorSolucion += beneficio[solucion[i]][solucion[i+1]];
		valorSolucion -= costo[solucion[i]][solucion[i+1]];
		beneficio[solucion[i]][solucion[i+1]] = 0;
		beneficio[solucion[i+1]][solucion[i]] = 0;
	}
	// iterativamente se eliminan ciclos negativos.
	while(change){
		valorMenorCiclo = 0;
		change = false;
		reSetBeneficio();
		// buscamos el ciclo de menor valor.
		for (int i = 1; i < (int)solucion.size(); i++){
			if (change) break;
			v1 = solucion[i];
			i1 = i;
			for (int j = i+1; j < (int)solucion.size();j++){
				if (change) break;
				v3 = solucion[j];
				i3 = j;
				if (v3 == v1){
					valorSolucionCiclo = 0;
					reSetBeneficio();
					for (int k = 0; k < i1-1; k++){
						beneficio[solucion[k]][solucion[k+1]] = 0;
						beneficio[solucion[k+1]][solucion[k]] = 0;
					}
					for (int k = i1; k < i3 - 1; k++){
						valorSolucionCiclo += beneficio[solucion[k]][solucion[k+1]];
						valorSolucionCiclo -= costo[solucion[k+1]][solucion[k]];
						beneficio[solucion[k]][solucion[k+1]] = 0;
						beneficio[solucion[k+1]][solucion[k]] = 0;
					}
					if (valorSolucionCiclo < valorMenorCiclo){
						valorMenorCiclo = valorSolucionCiclo;
						menorCiclo.ff = i1;
						menorCiclo.ss = i3;
					}
				}
			}
		}
		// Eliminamos el ciclo de menor valor.
		if (valorMenorCiclo < 0){
			solucion.erase(solucion.begin()+menorCiclo.ff,solucion.begin()+menorCiclo.ss);
			change = true;
		}
	}
	return solucion;
}
