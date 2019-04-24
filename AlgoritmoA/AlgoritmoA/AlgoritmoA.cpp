#include <iostream>
#include <string>
#include <time.h>
#include <fstream>
#include <conio.h>
#include <cctype>
#include <math.h>

using namespace std;

/*
	RUBÉN GARCÍA MATEOS, 02551077D, INGENIERÍA DEL CONOCIMIENTO. PRÁCTICA 1 ALGORITMO A*.
*/

//************Declaracion de variables y listas*****************

const int MAX_NODOS = 500;
const int DIM = 6;
const int MAX_MOVS = 8;
typedef struct{
	int fila;
	int columna;
	int filaPadre;
	int colPadre;
	double f; //Distancia a la meta + distancia recorrida
}tAbierta;
typedef struct{
	int fila;
	int columna;
	int filaPadre;
	int colPadre;
}tCerrada;
typedef struct{
	int fila;
	int columna;
}tFinal;
typedef tAbierta listaAbierta[MAX_NODOS];
typedef tCerrada listaCerrada[MAX_NODOS];
typedef tFinal listaFinal[MAX_NODOS];
typedef char tablero[DIM][DIM];
typedef struct{
	string letra;
	int fila;
	int columna;
}tNodo;
typedef struct{
	int fila;
	int columna;
}tMovimiento;
typedef tMovimiento posiblesMovs[MAX_MOVS];

//************Prototipos****************

void mostrarMatriz(tablero lista);
int obtenerMenorValorMatriz(tablero tablero, int &fila, int &columna);
double obtenerMenorDistancia(listaAbierta lista, int contA, int &fila, int &columna, int &posiciondelMenor);
void iniciarMatriz(tablero tablero);
void ponerObstaculo(tablero tablero);
bool comprobarRango(int fila, int columna);
int posiblesMovimientos(tablero tablero, posiblesMovs pM, int fil, int col, int filaSalida, int colSalida);
void fijarSalida(tablero tablero, int &fil, int &col);
void fijarMeta(tablero tablero, int &filaMeta, int &colMeta);
int obtenerFila(tablero tablero);
int obtenerColumna(tablero tablero);
int mostrarMenu();
void añadirElementoListaAbierta( int fila, int columna, listaAbierta listaAbierta, int &cont, double f, int filaPadre, int colPadre);
void añadirElementoListaCerrada(int fila, int columna, listaCerrada listaCerrada, int &cont, int filaPadre, int colPadre);
double distancia(int filaInicio, int colInicio, int filaDestino, int colDestino); //Nos devuelve la distancia estimada a la meta de un determinado nodo
void eliminarElementoLista(listaAbierta listaAbierta, int posiciondelMenor, int &contA);
int posicionCerrada(listaCerrada listaCerrada, int contC, int contA,  listaAbierta listaAbierta);
int posicionCerradadeMeta(listaCerrada listaCerrada, int contC, int filaMeta, int colMeta);
int ultimaPosicionCerradaIgual(listaCerrada listaCerrada, int contC);
void eliminarElementoListaCerrada(listaCerrada listaAbierta, int posicion, int &contC);

//*********************Codigo principal********************

int main(){
	tablero tablero;
	posiblesMovs pM;
	listaAbierta listaAbierta;
	listaCerrada listaCerrada;
	listaFinal listaFinal;
	char respuesta;
	int opcion = 1, numExpansiones, contA = 0, contC = 0, filaSalida, colSalida, filaMeta, colMeta, filaPadre = -1, colPadre = -1;
	int fila, columna, posiciondelMenor = 0, filaActual = -1, columnaActual = -1, posicionC;
	double g; //Distancia que llevamos reccorrida
	double h; //Distancia que nos queda hasta la meta estimada
	double f; //Suma de ambas distancias f + g
	double menorDistancia;
	bool encontrado = false, encontradosIguales = false, encontradoInicio = false, encontradoIgualCerrada = false;
	int filaAux = 1, x = 0, contF = 0;
	int filaFinal, colFinal, posiciondeMeta, ultPosicionCerradaIgual;
	ifstream archivo1;
	string linea;
	int entradas = 1, colMetaInicial, filaMetaInicial, filaMetaFinal, colMetaFinal;

	while (opcion > -1){
		cout << "Elige una opcion: " << endl;
		opcion = mostrarMenu();

		switch (opcion){
		case 1:
			//Inicializamos todos los valores de los arrays y de las variables para que cuando vuelva a entrar al simulador este todo vacio
			for (int i = 0; i < MAX_MOVS; i++){
				pM[i].fila = -1;
				pM[i].columna = -1;
			}

			for (int i = 0; i < MAX_NODOS; i++){
				listaCerrada[i].fila = -1;
				listaCerrada[i].columna = -1;
				listaCerrada[i].filaPadre = -1;
				listaCerrada[i].colPadre = -1;
			}
			for (int i = 0; i < MAX_NODOS; i++){
				listaAbierta[i].fila = -1;
				listaAbierta[i].columna = -1;
				listaAbierta[i].filaPadre = -1;
				listaAbierta[i].colPadre = -1;
			}
			for (int i = 0; i < MAX_NODOS; i++){
				listaFinal[i].fila = -1;
				listaFinal[i].columna = -1;
			}
			contC = 0;
			contA = 0;
			contF = 0;
			encontrado = false;
			filaActual = -1;
			columnaActual = -1;
			filaPadre = -1;
			colPadre = -1;
			posiciondelMenor = 0;
			contF = 0;
			filaAux = 1;
			encontradoInicio = false;
			filaSalida = -1;
			colSalida = -1;
			filaMeta = -1;
			colMeta = -1;

			//Comienza el programa tras la inicializacion de las variables

			iniciarMatriz(tablero); //Pone la matriz o tablero con valores vacios para inicializarla
			fijarSalida(tablero, filaSalida, colSalida); //Coloca el inicio de la ruta, se expresa con una c, y se guarda la posicion en filainicio y colinicio
			fijarMeta(tablero, filaMeta, colMeta);

			cout << "-->Quieres poner obstaculos? (S/N)" << endl << endl;
			cin >> respuesta;
			if (respuesta == 's'){
				ponerObstaculo(tablero);
				cout << endl;
			}

			filaPadre = filaSalida;
			colPadre = colSalida;

			//Ponemos en la lista cerrada el nodo inicial (La salida del avion..)
			añadirElementoListaCerrada(filaSalida, colSalida, listaCerrada, contC, -1, -1);

			//Expandimos el nodo inicial
			numExpansiones = posiblesMovimientos(tablero, pM, filaSalida, colSalida, filaSalida, colSalida);
			mostrarMatriz(tablero);

			filaActual = obtenerFila(tablero) + 1;
			columnaActual = obtenerColumna(tablero) + 1;

			//Comprobamos si l lista de osibles expansiones esta vacia, y si esta es vacia salimos directamente.
			if (numExpansiones <= 0){
				encontrado = true;
				cout << "No hay posibles movimientos para ejecutar, tenemos que salir1" << endl;
			}

			while (!encontrado){
				//Si encontramos dos elementos iguales en la lista cerrada, eliminamos el que esta en la ultima posicion de la listaCerrada ya que tendra 
				//Un padre incorrecto
				for (int i = 0; i < contC; i++){
					for (int j = i + 1; j < contC; j++){
						if ((listaCerrada[i].fila == listaCerrada[j].fila) && (listaCerrada[i].columna == listaCerrada[j].columna)){
							ultPosicionCerradaIgual = ultimaPosicionCerradaIgual(listaCerrada, contC);
							eliminarElementoListaCerrada(listaCerrada, ultPosicionCerradaIgual, contC);
						}
					}
				}

				//Si encontramos un posible movimiento que sea la meta, hemos terminado y encontrado se hace true que es la meta
				for (int i = 0; i < numExpansiones; i++){
					if ((pM[i].fila == filaMeta) && (pM[i].columna == colMeta)){
						encontrado = true;
						añadirElementoListaCerrada(fila, columna, listaCerrada, contC, fila, columna );
						//Se vuelve a realizar la comprobaciond
					}

					if ((pM[i].fila > 0) && (pM[i].columna > 0)){
						if ((pM[i].fila != filaActual) || (pM[i].columna != columnaActual)){
							//Si encontramos dos elementos iguales en la lista abierta y cerrada, eliminamos de la list abierta el elemento ya que ya ha pasado a cerrada en algun momento
							for (int i = 0; i < contA; i++){
								for (int j = 0; j < contC; j++){
									if ((listaAbierta[i].fila == listaCerrada[j].fila) && (listaAbierta[i].columna == listaCerrada[j].columna)){
										posicionC = posicionCerrada(listaCerrada, contC, contA, listaAbierta);
										eliminarElementoLista(listaAbierta, posicionC, contA);
										encontradosIguales = true;
									}
								}
							}
							if (!encontradosIguales){
								//Calculamos distancia estimada a la meta de cada nodo expandido H 
								h = distancia(pM[i].fila, pM[i].columna, filaMeta, colMeta);
								//Calculamos la distancia al nodo inicial G o distancia que llevamos recorrida
								g = distancia(filaSalida, colSalida, pM[i].fila, pM[i].columna);
								//Calculamos la distancia total F y añadimos el elemento a la lista abierta con su correpsondiente valor de F
								f = g + h;
								//Si no ha encontrdo dos elementos iguales en la lista baierta y cerrada añadimos el elemento a la lista abierta junto con su padre
								añadirElementoListaAbierta(pM[i].fila, pM[i].columna, listaAbierta, contA, f, filaPadre, colPadre);
							}
							encontradosIguales = false;
						}
					}

				}

				if (!encontrado){
					//Obtiene la menor distancia f que hay en las expansiones(Lista abierta) y devuelve en fila y columna el nodo que corresponde
					menorDistancia = obtenerMenorDistancia(listaAbierta, contA, fila,columna, posiciondelMenor);

					añadirElementoListaCerrada(fila, columna, listaCerrada, contC, listaAbierta[posiciondelMenor].filaPadre, listaAbierta[posiciondelMenor].colPadre);

					//Eliminamos el elemento menor de la lista abierta  ha pasado a la lista cerrada
					eliminarElementoLista(listaAbierta, posiciondelMenor, contA);

					//al encontrar dos elementos iguales en la lista abierta y cerrada copiamos el padre de la lista abierta a la lista cerrada
					//Añadimos el padre en la lista cerrada
					for (int i = 0; i < contA; i++){
						for (int j = 0; j < contC; j++){
							if ((listaAbierta[i].fila == listaCerrada[j].fila) && (listaAbierta[i].columna == listaCerrada[j].columna)){
								listaCerrada[j].filaPadre = listaAbierta[i].filaPadre;
								listaCerrada[j].colPadre = listaAbierta[i].colPadre;

							}
						}
					}

					//Añadimos es eelemento a la lista cerrada y incluimos su padre
					filaActual = obtenerFila(tablero) + 1;
					columnaActual = obtenerColumna(tablero) + 1;

					filaPadre = fila;
					colPadre = columna;

					//Actualizamos el tablero
					tablero[filaActual-1][columnaActual-1] = ' ';
					tablero[fila-1][columna-1] = 'c';
					tablero[filaMeta-1][colMeta-1] = 'm';

					//Vaciamos la lista de posibles expansiones
					for (int i = 0; i < MAX_MOVS; i++){
						pM[i].fila = -1;
						pM[i].columna = -1;
					}

					mostrarMatriz(tablero);

					//Volvemos a obtener la expansion del nodo  que hemos incluido a la lista cerrada
					numExpansiones = posiblesMovimientos(tablero, pM, fila, columna, filaSalida, colSalida);

					if (numExpansiones <= 0){
							encontrado = true;
							cout << "No hay posibles movimientos para ejecutar, tenemos que salir2" << endl;
					}
					
				}
				else{
					listaCerrada[contC - 1].fila = filaMeta;
					listaCerrada[contC - 1].columna = colMeta;
					listaCerrada[contC - 1].filaPadre = fila;
					listaCerrada[contC - 1].colPadre = columna;

				}

			}

			for (int i = 0; i < contA; i++){
				for (int j = 0; j < contC; j++){
					if ((listaAbierta[i].fila == listaCerrada[j].fila) && (listaAbierta[i].columna == listaCerrada[j].columna)){
						posicionC = posicionCerrada(listaCerrada, contC, contA, listaAbierta);
						eliminarElementoLista(listaAbierta, posicionC, contA);
						encontradosIguales = true;
					}
				}
			}

			cout << endl << endl << endl;

			if (numExpansiones >= 1){

				cout << "MOSTRAMOS LA LISTA CERRADA DEFINITIVA: " << endl << endl;

				//Mostramos la lista cerrada final
				for (int i = 0; i < contC; i++){
					cout << "(" << listaCerrada[i].fila << "," << listaCerrada[i].columna << ")  " ;
				}

				cout << endl << endl;

				cout << "MOSTRAMOS LA LISTA ABIERTA DEFINITIVA: " << endl << endl;

				//Mostramos la lista cerrada final
				for (int i = 0; i < contA; i++){
					cout << "(" << listaAbierta[i].fila << "," << listaAbierta[i].columna << ")  ";
				}

				cout << endl << endl;

				filaFinal = filaMeta;
				colFinal = colMeta;
				listaFinal[contF].fila = filaFinal;
				listaFinal[contF].columna = colFinal;
				contF++;
				posiciondeMeta = posicionCerradadeMeta(listaCerrada, contC, filaMeta, colMeta);


				//Mientras no encontremos la salida del dispositivo, sigue guardando los datos en el array final a mostrar del camino
				while (filaFinal != filaSalida || colFinal != colSalida){
					filaFinal = listaCerrada[posiciondeMeta].filaPadre;
					colFinal = listaCerrada[posiciondeMeta].colPadre;
					listaFinal[contF].fila = filaFinal;
					listaFinal[contF].columna = colFinal;
					contF++;
					//Ahora la filameta y columnameta es el padre de la meta anterior
					filaMeta = listaCerrada[posiciondeMeta].filaPadre;
					colMeta = listaCerrada[posiciondeMeta].colPadre;
					//obtiene la posicion de la meta en ese momento (La meta va cambiando en base a los padres que va teniendo)
					posiciondeMeta = posicionCerradadeMeta(listaCerrada, contC, filaMeta, colMeta);
				}

				cout << "RECORRIDO FINAL QUE SE LLEVA A CABO: " << endl << endl;
				for (int i = contF - 1; i > 0; i--){
					cout << "(" << listaFinal[i].fila << "," << listaFinal[i].columna << ")" << " -> ";
				}
				cout << "(" << listaFinal[0].fila << "," << listaFinal[0].columna << ")";
			}


			cout << endl << endl << endl;

			break;
		case 2: ////////////OPCION 2

			//Inicializamos todos los valores de los arrays y de las variables para que cuando vuelva a entrar al simulador este todo vacio
			for (int i = 0; i < MAX_MOVS; i++){
				pM[i].fila = -1;
				pM[i].columna = -1;
			}

			for (int i = 0; i < MAX_NODOS; i++){
				listaCerrada[i].fila = -1;
				listaCerrada[i].columna = -1;
				listaCerrada[i].filaPadre = -1;
				listaCerrada[i].colPadre = -1;
			}
			for (int i = 0; i < MAX_NODOS; i++){
				listaAbierta[i].fila = -1;
				listaAbierta[i].columna = -1;
				listaAbierta[i].filaPadre = -1;
				listaAbierta[i].colPadre = -1;
			}
			for (int i = 0; i < MAX_NODOS; i++){
				listaFinal[i].fila = -1;
				listaFinal[i].columna = -1;
			}
			contC = 0;
			contA = 0;
			contF = 0;
			encontrado = false;
			filaActual = -1;
			columnaActual = -1;
			filaPadre = -1;
			colPadre = -1;
			posiciondelMenor = 0;
			contF = 0;
			filaAux = 1;
			encontradoInicio = false;
			filaSalida = -1;
			colSalida = -1;
			filaMeta = -1;
			colMeta = -1;

			//Comienza el programa tras la inicializacion de las variables

			iniciarMatriz(tablero); //Pone la matriz o tablero con valores vacios para inicializarla
			fijarSalida(tablero, filaSalida, colSalida); //Coloca el inicio de la ruta, se expresa con una c, y se guarda la posicion en filainicio y colinicio
			cout << endl;
			cout << " Donde ponemos el punto por el que tiene que pasar el dispositivo? " << endl;
			cout << " fila: ";
			cin >> filaMeta;
			cout << " columna: ";
			cin >> colMeta;

			cout << endl;

			cout << " Donde ponemos la meta del dispositivo? " << endl;
			cout << " fila: ";
			cin >> filaMetaFinal;
			cout << " columna: ";
			cin >> colMetaFinal;


			colMetaInicial = colMeta;
			filaMetaInicial = filaMeta;

			cout << "-->Quieres poner obstaculos? (S/N)"  << endl;
			cin >> respuesta;
			if (respuesta == 's'){
				ponerObstaculo(tablero);
				cout << endl;
			}

			filaPadre = filaSalida;
			colPadre = colSalida;

			//Ponemos en la lista cerrada el nodo inicial (La salida del avion..)
			añadirElementoListaCerrada(filaSalida, colSalida, listaCerrada, contC, -1, -1);

			//Expandimos el nodo inicial
			numExpansiones = posiblesMovimientos(tablero, pM, filaSalida, colSalida, filaSalida, colSalida);

			filaActual = obtenerFila(tablero) + 1;
			columnaActual = obtenerColumna(tablero) + 1;

			//Comprobamos si l lista de osibles expansiones esta vacia, y si esta es vacia salimos directamente.
			if (numExpansiones <= 0){
				encontrado = true;
				cout << "No hay posibles movimientos para ejecutar, tenemos que salir1" << endl;
			}

			while (entradas <= 2){
				while (!encontrado){
					//Si encontramos dos elementos iguales en la lista cerrada, eliminamos el que esta en la ultima posicion de la listaCerrada ya que tendra 
					//Un padre incorrecto
					for (int i = 0; i < contC; i++){
						for (int j = i + 1; j < contC; j++){
							if ((listaCerrada[i].fila == listaCerrada[j].fila) && (listaCerrada[i].columna == listaCerrada[j].columna)){
								ultPosicionCerradaIgual = ultimaPosicionCerradaIgual(listaCerrada, contC);
								eliminarElementoListaCerrada(listaCerrada, ultPosicionCerradaIgual, contC);
							}
						}
					}

					//Si encontramos un posible movimiento que sea la meta, hemos terminado y encontrado se hace true que es la meta
					for (int i = 0; i < numExpansiones; i++){
						if ((pM[i].fila == filaMeta) && (pM[i].columna == colMeta)){
							encontrado = true;
							añadirElementoListaCerrada(fila, columna, listaCerrada, contC, fila, columna);
							//Se vuelve a realizar la comprobaciond
						}

						if ((pM[i].fila > 0) && (pM[i].columna > 0)){
							if ((pM[i].fila != filaActual) || (pM[i].columna != columnaActual)){
								//Si encontramos dos elementos iguales en la lista abierta y cerrada, eliminamos de la list abierta el elemento ya que ya ha pasado a cerrada en algun momento
								for (int i = 0; i < contA; i++){
									for (int j = 0; j < contC; j++){
										if ((listaAbierta[i].fila == listaCerrada[j].fila) && (listaAbierta[i].columna == listaCerrada[j].columna)){
											posicionC = posicionCerrada(listaCerrada, contC, contA, listaAbierta);
											eliminarElementoLista(listaAbierta, posicionC, contA);
											encontradosIguales = true;
										}
									}
								}
								if (!encontradosIguales){
									//Calculamos distancia estimada a la meta de cada nodo expandido H 
									h = distancia(pM[i].fila, pM[i].columna, filaMeta, colMeta);
									//Calculamos la distancia al nodo inicial G o distancia que llevamos recorrida
									g = distancia(filaSalida, colSalida, pM[i].fila, pM[i].columna);
									//Calculamos la distancia total F y añadimos el elemento a la lista abierta con su correpsondiente valor de F
									f = g + h;
									//Si no ha encontrdo dos elementos iguales en la lista baierta y cerrada añadimos el elemento a la lista abierta junto con su padre
									añadirElementoListaAbierta(pM[i].fila, pM[i].columna, listaAbierta, contA, f, filaPadre, colPadre);
								}
								encontradosIguales = false;
							}
						}

					}

					if (!encontrado){
						//Obtiene la menor distancia f que hay en las expansiones(Lista abierta) y devuelve en fila y columna el nodo que corresponde
						menorDistancia = obtenerMenorDistancia(listaAbierta, contA, fila, columna, posiciondelMenor);

						añadirElementoListaCerrada(fila, columna, listaCerrada, contC, listaAbierta[posiciondelMenor].filaPadre, listaAbierta[posiciondelMenor].colPadre);

						//Eliminamos el elemento menor de la lista abierta  ha pasado a la lista cerrada
						eliminarElementoLista(listaAbierta, posiciondelMenor, contA);

						//al encontrar dos elementos iguales en la lista abierta y cerrada copiamos el padre de la lista abierta a la lista cerrada
						//Añadimos el padre en la lista cerrada
						for (int i = 0; i < contA; i++){
							for (int j = 0; j < contC; j++){
								if ((listaAbierta[i].fila == listaCerrada[j].fila) && (listaAbierta[i].columna == listaCerrada[j].columna)){
									listaCerrada[j].filaPadre = listaAbierta[i].filaPadre;
									listaCerrada[j].colPadre = listaAbierta[i].colPadre;

								}
							}
						}

						//Añadimos es eelemento a la lista cerrada y incluimos su padre
						filaActual = obtenerFila(tablero) + 1;
						columnaActual = obtenerColumna(tablero) + 1;

						filaPadre = fila;
						colPadre = columna;

						//Actualizamos el tablero
						tablero[filaActual - 1][columnaActual - 1] = ' ';
						tablero[fila - 1][columna - 1] = 'c';
						tablero[filaMeta - 1][colMeta - 1] = 'm';

						//Vaciamos la lista de posibles expansiones
						for (int i = 0; i < MAX_MOVS; i++){
							pM[i].fila = -1;
							pM[i].columna = -1;
						}

						//Volvemos a obtener la expansion del nodo  que hemos incluido a la lista cerrada
						numExpansiones = posiblesMovimientos(tablero, pM, fila, columna, filaSalida, colSalida);

						if (numExpansiones <= 0){
							encontrado = true;
							cout << "No hay posibles movimientos para ejecutar, tenemos que salir" << endl;
						}

					}
					else{
						listaCerrada[contC - 1].fila = filaMeta;
						listaCerrada[contC - 1].columna = colMeta;
						listaCerrada[contC - 1].filaPadre = fila;
						listaCerrada[contC - 1].colPadre = columna;

					}

				}

				for (int i = 0; i < contA; i++){
					for (int j = 0; j < contC; j++){
						if ((listaAbierta[i].fila == listaCerrada[j].fila) && (listaAbierta[i].columna == listaCerrada[j].columna)){
							posicionC = posicionCerrada(listaCerrada, contC, contA, listaAbierta);
							eliminarElementoLista(listaAbierta, posicionC, contA);
							encontradosIguales = true;
						}
					}
				}

				//cout << endl << endl << endl;

				if (numExpansiones >= 1){

					filaFinal = filaMeta;
					colFinal = colMeta;
					listaFinal[contF].fila = filaFinal;
					listaFinal[contF].columna = colFinal;
					contF++;
					posiciondeMeta = posicionCerradadeMeta(listaCerrada, contC, filaMeta, colMeta);

					//Mientras no encontremos la salida del dispositivo, sigue guardando los datos en el array final a mostrar del camino
					while (filaFinal != filaSalida || colFinal != colSalida){
						filaFinal = listaCerrada[posiciondeMeta].filaPadre;
						colFinal = listaCerrada[posiciondeMeta].colPadre;
						listaFinal[contF].fila = filaFinal;
						listaFinal[contF].columna = colFinal;
						contF++;
						//Ahora la filameta y columnameta es el padre de la meta anterior
						filaMeta = listaCerrada[posiciondeMeta].filaPadre;
						colMeta = listaCerrada[posiciondeMeta].colPadre;
						//obtiene la posicion de la meta en ese momento (La meta va cambiando en base a los padres que va teniendo)
						posiciondeMeta = posicionCerradadeMeta(listaCerrada, contC, filaMeta, colMeta);
					}

					if (entradas == 1){
						cout << "RECORRIDO FINAL QUE SE LLEVA A CABO: " << endl << endl;
						for (int i = contF - 1; i > 0; i--){
							cout << "(" << listaFinal[i].fila << "," << listaFinal[i].columna << ")" << " -> ";
						}
						cout << "(" << listaFinal[0].fila << "," << listaFinal[0].columna << ")" << " -> ";
					}
					else{
						for (int i = contF - 2; i > 0; i--){
							cout << "(" << listaFinal[i].fila << "," << listaFinal[i].columna << ")" << " -> ";
						}
						cout << "(" << listaFinal[0].fila << "," << listaFinal[0].columna << ")";
					}
				}


				for (int i = 0; i < MAX_MOVS; i++){
					pM[i].fila = -1;
					pM[i].columna = -1;
				}

				for (int i = 0; i < MAX_NODOS; i++){
					listaCerrada[i].fila = -1;
					listaCerrada[i].columna = -1;
					listaCerrada[i].filaPadre = -1;
					listaCerrada[i].colPadre = -1;
				}
				for (int i = 0; i < MAX_NODOS; i++){
					listaAbierta[i].fila = -1;
					listaAbierta[i].columna = -1;
					listaAbierta[i].filaPadre = -1;
					listaAbierta[i].colPadre = -1;
				}
				for (int i = 0; i < MAX_NODOS; i++){
					listaFinal[i].fila = -1;
					listaFinal[i].columna = -1;
				}
				contC = 0;
				contA = 0;
				contF = 0;
				encontrado = false;
				filaActual = -1;
				columnaActual = -1;
				filaPadre = -1;
				colPadre = -1;
				posiciondelMenor = 0;
				contF = 0;
				filaAux = 1;
				encontradoInicio = false;
				entradas++;
				//tablero[filaMeta - 1][colMeta - 1] = ' ';

				if (entradas <= 2){
					tablero[filaMeta - 1][colMeta - 1] = 'c';
					filaSalida = filaMetaInicial;
					colSalida = colMetaInicial;
					filaMeta = filaMetaFinal;
					colMeta = colMetaFinal;
					filaPadre = filaSalida;
					colPadre = colSalida;
					//Ponemos en la lista cerrada el nodo inicial (La salida del avion..)
					añadirElementoListaCerrada(filaSalida, colSalida, listaCerrada, contC, -1, -1);

					//Expandimos el nodo inicial
					numExpansiones = posiblesMovimientos(tablero, pM, filaSalida, colSalida, filaSalida, colSalida);
					//mostrarMatriz(tablero);

					filaActual = obtenerFila(tablero) + 1;
					columnaActual = obtenerColumna(tablero) + 1;
				}
				tablero[filaSalida - 1][colSalida - 1] = 'p';
				tablero[filaMetaFinal - 1][colMetaFinal - 1] = 'm';

			}
			cout << endl << endl;
			mostrarMatriz(tablero);

			entradas = 1;
			cout << endl << endl << endl;

			break;
		case 3:
			archivo1.open("autor.txt");
			if (!archivo1.is_open()) cout << "Error al abrir el archivo..." << endl;
			else{
				getline(archivo1, linea);
				while (linea != "final"){
					cout << linea << endl;
					getline(archivo1, linea);
				}
			}
			archivo1.close();
			cout << endl << endl;
			break;
		case 4:
			cout << "Hasta pronto!!" << endl;
			system("PAUSE");
			return 0;
			break; 
		default:
			cout << "Opcion no valida " << endl;
		}
	}
}

//*****************************Desarrollo de prototipos***************************

void mostrarMatriz(tablero lista){

	for (int h = 0; h < DIM; h++){
		cout << "    " <<  h + 1 << " ";
	}
	cout << endl;
	cout << "  ";
	for(int i = 0; i < (DIM*6)-1; i++){
		cout << "-" ;
	}
	cout << endl;
	for (int i = 0; i < DIM; i++){
		cout << i+1 << " ";
		for (int j = 0; j < DIM ; j++){
			cout <<"| " << lista[i][j] << " | ";
		}
		cout << endl;
	}
	cout << "  ";
	for (int i = 0; i < (DIM * 6) - 1; i++){
		cout << "-";
	}
	cout << endl << endl;
}
int obtenerMenorValorMatriz(tablero tablero, int &fila, int &columna){
	int menor;

	for (int i = 0; i < DIM - 1; i++){
		for (int j = 0; j < DIM - 1; j++){
			if (tablero[i][j] >= 0){
				menor = tablero[i][j];
			}
		}
	}
	for (int i = 0; i < DIM - 1; i++){
		for (int j = 0; j < DIM - 1; j++){
			if (tablero[i][j] <= menor && tablero[i][j] >= 0){
				menor = tablero[i][j];
				fila = i;
				columna = j;
			}
		}
	}
	return menor;
}
void iniciarMatriz(tablero tablero){
	for (int i = 0; i < DIM; i++){
		for (int j = 0; j < DIM; j++){
			tablero[i][j] = ' ';
		}
	}
}
void ponerObstaculo(tablero tablero){
	int fil, col;
	char teclaPulsada = 's';

	while (teclaPulsada == 's'){
		cout << "En que casilla quieres poner el obstaculo? " << endl;
		cout << "Fila: ";
		cin >> fil;
		cout << "Columna: ";
		cin >> col;

		if (comprobarRango(fil, col)){
			tablero[fil-1][col-1] = 'X';
		}
		else {
			cout << "Rango no valido, la fila y columna tiene que ser menor o igual que: " << DIM << endl;
		}

		cout << "¿Quieres poner mas obstaculos? (S/N): ";

		cin >> (teclaPulsada);
	}
}
bool comprobarRango(int fila, int columna){
	bool devolver = true;
	if ((fila > DIM) || (columna > DIM) || (fila < 0) || (columna < 0)){
		devolver = false;
	}
	return devolver;
}
int posiblesMovimientos(tablero tablero, posiblesMovs pM, int fil, int col, int filaSalida, int colSalida){
	int valoresInsertados = 0;
	fil--;
	col--;

	for (int x = -1; x < 2; x++){
		for (int y = -1; y < 2; y++){														//No se añade la posicion actual como p.Movimiento
			if ((tablero[fil + x][col + y] == ' ') || (tablero[fil + x][col + y] == 'm') && (pM[valoresInsertados].fila != fil + 1) && (pM[valoresInsertados].columna != col + 1)){ //Comprueba que haya un hueco libre
				if (comprobarRango(fil + x, col + y)){ //Comprueba que este dentro del tablero
					if ((fil+x+1 != filaSalida) || (col+y+1 != colSalida) ){
						pM[valoresInsertados].fila = fil + 1 + x;
						pM[valoresInsertados].columna = col + 1 + y;
						valoresInsertados++;
					}
				}
			}
		}
	}

	/*for (int i = 0; i < MAX_MOVS; i++){
		cout << "Posibles movimientos: " << "fila: " << pM[i].fila << " " << "columna: " << pM[i].columna << endl;
	}*/
	return valoresInsertados;
}
void fijarSalida(tablero tablero, int &fil, int &col){
	cout << "Donde colocamos el punto de partida?: " << endl;
	while (fil < 0 || col < 0 || fil > DIM || col > DIM){
		cout << "Fila: ";
		cin >> fil;
		cout << "Columna: ";
		cin >> col;
		if (fil < 0 || col < 0 || fil > DIM || col > DIM) cout << "introduce un valor valido" << endl;
	}
	tablero[fil-1][col-1] = 'c';
}
int mostrarMenu(){
	int opcion;

	cout << "1- Simulador algoritmo A* con tablero" << endl;
	cout << "2- Simulador algoritmo A* con paso por un punto intermedio " << endl;
	cout << "3- Autor y aplicaciones en la vida real " << endl;
	cout << "4- Salir " << endl;

	cin >> opcion;
	return opcion;
}
void fijarMeta(tablero tablero, int &filaMeta, int &colMeta){

	cout << "Donde colocamos la meta?: " << endl;
	while (filaMeta < 0 || colMeta < 0 || filaMeta > DIM || colMeta > DIM){
		cout << "Fila: ";
		cin >> filaMeta;
		cout << "Columna: ";
		cin >> colMeta;
		if (filaMeta < 0 || colMeta < 0 || filaMeta > DIM || colMeta > DIM) cout << "introduce un valor valido" << endl;
	}

	tablero[filaMeta-1][colMeta-1] = 'm';
}
int obtenerFila(tablero tablero){
	int fila;
	for (int i = 0; i < DIM; i++){
		for (int j = 0; j < DIM; j++){
			if (tablero[i][j] == 'c') fila = i;
		}
	}

	return fila;
}
int obtenerColumna(tablero tablero){
	int columna;
	for (int i = 0; i < DIM; i++){
		for (int j = 0; j < DIM; j++){
			if (tablero[i][j] == 'c') columna = j;
		}
	}
	return columna;
}
void añadirElementoListaAbierta(int fila, int columna, listaAbierta listaAbierta, int &cont, double f, int filaPadre, int colPadre){
	bool igual = false;
	for (int i = 0; i < cont; i++){
		if ((fila == listaAbierta[i].fila) && (columna == listaAbierta[i].columna)) igual = true; //Hemos encontrado que hay un nodo igual añadido
	}	// a la lista abierta

	if (!igual){
		listaAbierta[cont].fila = fila;
		listaAbierta[cont].columna = columna;
		listaAbierta[cont].f = f;
		listaAbierta[cont].filaPadre = filaPadre;
		listaAbierta[cont].colPadre = colPadre;
		cont++;
	}
}
void añadirElementoListaCerrada(int fila, int columna, listaCerrada listaCerrada, int &cont, int filaPadre, int colPadre){
	listaCerrada[cont].fila = fila;
	listaCerrada[cont].columna = columna;
	listaCerrada[cont].filaPadre = filaPadre;
	listaCerrada[cont].colPadre = colPadre;
	cont++;
}
double distancia(int filaInicio, int colInicio, int filaDestino, int colDestino){
	double distanciaH;
	distanciaH = sqrt((pow((filaInicio-filaDestino), 2))+ (pow((colInicio-colDestino), 2)));
	return distanciaH;
}
double obtenerMenorDistancia(listaAbierta lista, int contA, int &fila, int &columna, int &posiciondelMenor){
	double menor;
	menor = 50;

	for (int i = 0; i < contA; i++){
		if ((lista[i].fila >= 0) && (lista[i].columna >= 0)){
			if (lista[i].f < menor){
				menor = lista[i].f;
				posiciondelMenor = i;
			}
		}
	}
	
	fila = lista[posiciondelMenor].fila;
	columna = lista[posiciondelMenor].columna;
	return menor;
}
void eliminarElementoLista(listaAbierta listaAbierta, int posiciondelMenor, int &contA){
	for (int i = posiciondelMenor; i < contA ; i++){
		listaAbierta[i].fila = listaAbierta[i + 1].fila;
		listaAbierta[i + 1].fila = -1;

		listaAbierta[i].columna = listaAbierta[i + 1].columna;
		listaAbierta[i + 1].columna = -1;

		listaAbierta[i].f = listaAbierta[i + 1].f;
		listaAbierta[i + 1].f = -1;

		listaAbierta[i].filaPadre = listaAbierta[i + 1].filaPadre;
		listaAbierta[i + 1].filaPadre = -1;

		listaAbierta[i].colPadre = listaAbierta[i + 1].colPadre;
		listaAbierta[i + 1].colPadre = -1;
	}
	contA--;
}
int posicionCerrada(listaCerrada listaCerrada, int contC, int contA, listaAbierta listaAbierta){
	int posiciondelElemento=0;
	for (int i = 0; i < contA; i++){
		for (int j = 0; j < contC; j++){
			if (listaAbierta[i].fila == listaCerrada[j].fila && listaAbierta[i].columna == listaCerrada[j].columna){
				posiciondelElemento = i;
			}
		}
	}
	return posiciondelElemento;
}
int posicionCerradadeMeta(listaCerrada listaCerrada, int contC, int filaMeta, int colMeta){
	int posiciondelElemento = 0;
		for (int i = 0; i < contC; i++){
			if (listaCerrada[i].fila == filaMeta && listaCerrada[i].columna == colMeta){
				posiciondelElemento = i;
			}
		}
	return posiciondelElemento;
}
int ultimaPosicionCerradaIgual(listaCerrada listaCerrada, int contC){
	int posicion;
	for (int i = 0; i < contC; i++){
		for (int j = i + 1; j < contC; j++){
			if ((listaCerrada[i].fila == listaCerrada[j].fila) && (listaCerrada[i].columna == listaCerrada[j].columna)){
				posicion = j;
			}
		}
	}
	return posicion;
}
void eliminarElementoListaCerrada(listaCerrada listaCerrada, int posicion, int &contC){
	for (int i = posicion; i < contC; i++){
		listaCerrada[i].fila = listaCerrada[i + 1].fila;
		listaCerrada[i + 1].fila = -1;

		listaCerrada[i].columna = listaCerrada[i + 1].columna;
		listaCerrada[i + 1].columna = -1;

		listaCerrada[i].filaPadre = listaCerrada[i + 1].filaPadre;
		listaCerrada[i + 1].filaPadre = -1;

		listaCerrada[i].colPadre = listaCerrada[i + 1].colPadre;
		listaCerrada[i + 1].colPadre = -1;
	}
	contC--;
}