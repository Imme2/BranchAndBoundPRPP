#ifndef F_AUXILIAR_H
#define F_AUXILIAR_H

vector<lado> branchAndBound(int,vector<lado>);
void dfs();
bool ladoCompare(lado ,lado );
vector<lado> obtenerListaSucesores(int );
bool cicloNegativo(lado ,vector<lado> &);
bool estaEnSolParcial(lado ,vector<lado> &);
bool repiteCiclo(lado ,vector<lado> &);
void agregarLado(lado ,vector<lado> &);
void eliminarUltimoLado(lado ,vector<lado> &);
bool cumpleAcotamiento(lado ,vector<lado> &);
bool usadoEnSolParcial(lado l,vector<lado> &solParcial);


vector<int> construirSolucion(int, vector<vector<int> >&);
vector < vector <int> > obtenerGrafoT(int);
pair< vector<int>, int > dijkstra(int,int,bool = true);
void agregarLadoASolucion(int, int, vector <int> &);
int obtenerTamanoGrafo(int,vector < vector <int> >&);
int borrarLado(int, int, vector<vector<int> > &);
void unirCaminoASolucion(vector<int> &, vector<int> &);
vector<int> elimCiclosNegativos(vector<int>);
void reSetBeneficio();

#endif