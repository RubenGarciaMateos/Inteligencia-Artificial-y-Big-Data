#include <iostream>
#include <string>
#include <time.h>
#include <fstream>
#include <conio.h>
#include <cctype>
#include <math.h>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <iomanip>

using namespace std;

/*
BLACK 0 Yes Yes
BLUE 1 Yes Yes
GREEN 2 Yes Yes
CYAN 3 Yes Yes
RED 4 Yes Yes
MAGENTA 5 Yes Yes
BROWN 6 Yes Yes
LIGHTGRAY 7 Yes Yes
DARKGRAY 8 No Yes
LIGHTBLUE 9 No Yes
LIGHTGREEN 10 No Yes
LIGHTCYAN 11 No Yes
LIGHTRED 12 No Yes
LIGHTMAGENTA 13 No Yes
YELLOW 14 No Yes
WHITE 15 No Yes

SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
*/

/*
  RUBÉN GARCÍA MATEOS, 02551077D.
  INGENIERÍA DEL CONOCIMIENTO, 4E
  PRÁCTICA: MÉTODOS DE CLASIFICACIÓN (BAYES, LLOYD Y K-MEDIAS)

*/

//-----------------------------------------------CONSTANTES-------------------------------------------------------------------
const int MAX_FIL = 2;
const int MAX_COL = 100; // <---------------------------------------------CONSTANTE QUE HAY QUE MODIFICAR PARA LOS DISTINTOS EJEMPLOS.
const int MAX_CELDAS = 5;
const double razon_Aprendizaje = 0.1; //Razon de aprendizaje para LLoyd
const double toleranciaLLoyd = pow(10, -10);
const double toleranciakMeddias = 0.01;
//-----------------------------------------------------------------------------------------------------------------------------

//------------------------------------------VARIABLES GLOBALES AL PROGRAMA-----------------------------------------------------
typedef double matrizGradosP[MAX_FIL][MAX_COL];
typedef vector<double> datosMuestras;
typedef vector<double> centros;
typedef vector<double> datosX;
typedef double covarianza[MAX_CELDAS][MAX_CELDAS];

//-----------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------PROTOTIPOS-------------------------------------------------------------------------------------
int mostrarMenu();
void calculaGradosDePertenencia(matrizGradosP matriz, datosMuestras datosMuestras, int numMuestras, int numDatos, centros v1, centros v2);
double distanciaEuclidea(datosX x, centros v);
vector<double> calculaCentro(matrizGradosP matriz, datosMuestras datosMuestras, int numCentro, int maxMuestras);
void inicializaCentros(centros &v1, centros &v2, int numDatos);
void muestraCoordCentros(centros v1, centros v2, int numero);
void muestraExilon(double distanciaCentro1, double distanciaCentro2, int numero);
void muestraMedias(centros v1, centros v2);
void calculaMediasCentros(centros &v1, centros& v2, int numMuestras, datosMuestras datosMuestras, int numDatos, int contadorClase1, int contadorClase2);
void calculaC(covarianza matrizCovarianza, covarianza matrizCovarianza2, centros v1, centros v2, int numMuestras, datosMuestras datosMuestras, int numDatos, int contadorClase1, int contadorClase2);
void muestraMatrizCovarianza(covarianza matrizCovarianza1, covarianza matrizCovarianza2, int numDatos);
void calculaDistancias(centros &v1, centros&v2, int numMuestras, int numDatos, datosMuestras datosMuestras);

//------------------------------------------------------------------------------------------------------------------------------------------

int main(){
	vector<string> clases, clasesAuxiliar, auxiliar;
	centros v1,v2, nuevoCentro1, nuevoCentro2, centroInicial1, centroInicial2, vectorCoordClas;
	covarianza covarianza1, covarianza2;
	matrizGradosP matrizGrados;
	datosMuestras datosMuestras;
	ifstream archivo;
	ofstream archivoOut;
	int numMuestras = 0, numClases = 0, numDatos = 0, contador, k, datosGuardados = 0, iteraciones = 0, opcion=0, h = 0, numCentro, maximoMuestras, posicionDeCambiodeClase;
	int contadorClase1 = 0, contadorClase2 = 0;
	int d = 0, g = 0;
	string linea, str, ejemplo, strAux, nombreArch, respuesta;
	bool entradounavez = true, entroPrimeraVez = false;
	double dato, pertenencia, distanciaCentro1 = 5, distanciaCentro2= 5, mediaM1=0, mediaM2=0, sumatorioMuestras=0, coord;
	bool error = true;

	for (int i = 0; i < MAX_CELDAS; i++){
		for (int j = 0; j < MAX_CELDAS; j++){
			covarianza1[i][j] = -1;
			covarianza2[i][j] = -1;
		}
	}
	while (error){
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		cout << "Introduce nombre del archivo a abrir(ejemplo.txt): " << endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
		cout << ">";
		cin >> nombreArch;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		archivo.open(nombreArch);
		cout << endl;
		if (!archivo.is_open()){
			cout << "Error al abrir el archivo: No existe" << endl;
		}
		else{
			error = false;
			//calculamos numero de muestras que tiene el txt y el numero de datos de cada muestra y guardamos en clasesAuxiliar el nombre de las clases
			while (!archivo.eof()){
				numMuestras++;
				getline(archivo, linea);
				for (int i = 0; i < linea.length(); i++){
					if (linea[i] != ','){
						str += linea[i];
					}
					else{
						if (entradounavez){
							numDatos++;
						}
						str = "";
					}
				}
				clasesAuxiliar.push_back(str);
				entradounavez = false;
			}
			archivo.close();


			archivo.open(nombreArch);
			string aux;
			str = "";

			while (!archivo.eof()){ //guardamos en datosMuestras los datos de las muestras..los doubles
				getline(archivo, linea);
				for (int i = 0; i < linea.length(); i++){
					if (linea[i] != ','){
						str += linea[i];
					}
					else{
						if (iteraciones < numDatos){
							aux = str;
							datosMuestras.push_back(stod(aux));
							iteraciones++;
						}
						str = "";
					}
				}
				str = "";
				iteraciones = 0;
			}
			archivo.close();
			for (int i = 0; i < clasesAuxiliar.size(); i++){
				contador = 0;
				ejemplo = clasesAuxiliar[i];
				auxiliar.push_back(ejemplo);
				for (k = 0; k < auxiliar.size(); k++){
					if (auxiliar[k] == ejemplo){
						if (entroPrimeraVez && nombreArch == "Iris2Clases.txt") //Añadidas estas condiciones porque daba error al almacenar las clases
							contador++;											//En clases.push back
						else if (nombreArch != "Iris2Clases.txt") contador++;
					}
				}
				entroPrimeraVez = true;
				if (contador == 1){
					clases.push_back(ejemplo);
				}
			}
			//Guardamos cuantas muestras hay de cada clase
			for (int i = 0; i < clasesAuxiliar.size(); i++){
				if (clasesAuxiliar[i] == clases[1]){
					contadorClase2++;
				}
				else contadorClase1++;
			}

			/*-A PARTIR DE AQUI TENEMOS GUARDADO EL NUMERO DE CLASES QUE TENEMOS EN CLASES.SIZE(), Y SUS NOMBRES EN EL VECTOR CLASES
			  -TENEMOS GUARDADO EL NUMERO DE DATOS QUE TIENEN LAS MUESTRAS EN NUMDATOS
			  -TENEMOS GUARDADO EL NUMERO TOTAL DE MUESTRAS EN NUMMUESTRAS
			  -TENEMOS GUARDADO TODOS LOS DATOS DE LAS MUESTRAS EN EL VECTOR DATOSMUESTRAS
			  */

			while (opcion > -1){
				h = 0;
				cout << "Elige una opcion: " << endl;
				opcion = mostrarMenu();
				v1.clear();
				v2.clear();
				distanciaCentro1 = 20;
				distanciaCentro2 = 20;

				switch (opcion){
				case 1:
					v1.clear();
					v2.clear();
					archivoOut.open("MatrizGradosP.txt");
					inicializaCentros(v1, v2, numDatos);
					muestraCoordCentros(v1, v2, 0); //Le pasamos 0 porque es la primera vez que entramos

					while ((distanciaCentro1 > toleranciakMeddias) && (distanciaCentro2 > toleranciakMeddias)){
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
						cout << "-----------------------------------------------------------------------------" << endl;
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
						cout << "                             ITERACION " << h + 1 << "  " << endl;
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
						cout << "-----------------------------------------------------------------------------" << endl << endl;
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
						cout << "Calculando matriz de grados de pertenencia... (Se ha guardado en el archivo 'MatrizGradosP.txt')" << endl;
						//Calculamos la matriz U (grados de pertenencia)
						calculaGradosDePertenencia(matrizGrados, datosMuestras, numMuestras, numDatos, v1, v2);

						//_-----------------------------GUARDA EN EL ARCHIVO DE SALIDA LAS MATRICES DE GRADOS DE PERTENENCIA--------------------

						archivoOut << "-----------------------------------------------------------------------------" << endl;
						archivoOut << "                     MATRIZ DE GRADOS DE PERTENENCIA " << h + 1 << "  " << endl;
						archivoOut << "-----------------------------------------------------------------------------" << endl << endl;
						for (int i = 0; i < MAX_COL; i++){
							for (int j = 0; j < MAX_FIL; j++){
								archivoOut << "P(C" << j + 1 << "/" << "X" << i + 1 << ") : " << matrizGrados[j][i] << "     "; //Pertenencia de la muestra i a la clase 1 a la izquierda, y a la derecha muestra i a la clase 2
							}
							archivoOut << endl;
						}
						archivoOut << endl;
						//---------------------------------------------------------------------------------------------------------------------------

						//A partir de aqui hay que volver a calcular los centros para ver si debemos continuar iterando o no
						cout << "Calculando los nuevos centros..." << endl;
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
						//Calculamos la distancia que hay entre el centro nuevo y el anterior para ver como es exilon
						nuevoCentro1 = calculaCentro(matrizGrados, datosMuestras, 1, numDatos);
						distanciaCentro1 = sqrt(distanciaEuclidea(nuevoCentro1, v1)); //Distancia euclidea con raiz
						v1.clear();
						for (int i = 0; i < nuevoCentro1.size(); i++){
							v1.push_back(nuevoCentro1[i]);
						}

						nuevoCentro2 = calculaCentro(matrizGrados, datosMuestras, 2, numDatos);
						distanciaCentro2 = sqrt(distanciaEuclidea(nuevoCentro2, v2)); //Distancia euclidea con raiz
						v2.clear();
						for (int i = 0; i < nuevoCentro2.size(); i++){
							v2.push_back(nuevoCentro2[i]);
						}

						muestraCoordCentros(v1, v2, 1);
						muestraExilon(distanciaCentro1, distanciaCentro2, 0);
						h++;
					}
					muestraCoordCentros(v1, v2, 2);
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
					cout << ">> ";
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
					cout << "Numero de iteraciones que hemos realizado para llegar a exilon < 0.01 : ";
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
					cout << h << endl;
					cout << endl;
					cout << "---------------------------------------------------------------------------------" << endl;
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
					cout << endl << endl;
					break;
				case 2:
					h = 0;
					distanciaCentro1 = 50;
					distanciaCentro2 = 50;
					centroInicial1.clear();
					centroInicial2.clear();
					v1.clear();
					v2.clear();
					if (nombreArch == "transparenciasLLoyd.txt"){
						v1.push_back(1);
						v1.push_back(4);
						v2.push_back(7);
						v2.push_back(2);
					}
					else
						inicializaCentros(v1, v2, numDatos);

					for (int i = 0; i < v1.size(); i++){
						centroInicial1.push_back(v1[i]);
						centroInicial2.push_back(v2[i]);
					}
					muestraCoordCentros(v1, v2, 0); //Le pasamos 0 porque es la primera vez que entramos
					while (((distanciaCentro1 > toleranciaLLoyd) && (distanciaCentro2 > toleranciaLLoyd)) && (h < 10)){
						cout << "-----------------------------------------------------------------------------" << endl;
						cout << "                             ITERACION " << h + 1 << "  " << endl;
						cout << "-----------------------------------------------------------------------------" << endl << endl;
						calculaDistancias(v1, v2, numMuestras, numDatos, datosMuestras);
						//muestraCoordCentros(v1, v2, 2); //Mostramos las coordenadas finales de los centros, el argumento tercero se le pasa 0 para mostrar centros iniciales, 1 nuevos calculados, y 2 para los finales
						distanciaCentro1 = sqrt(distanciaEuclidea(centroInicial1, v1)); //Distancia euclidea con raiz
						distanciaCentro2 = sqrt(distanciaEuclidea(centroInicial2, v2)); //Distancia euclidea con raiz
						centroInicial1.clear();
						centroInicial2.clear();
						for (int i = 0; i < v1.size(); i++){
							centroInicial1.push_back(v1[i]);
							centroInicial2.push_back(v2[i]);
						}
						//muestraExilon(distanciaCentro1, distanciaCentro2, 1);
						h++;
					}
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
					cout << ">> ";
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
					cout << "Numero de iteraciones que hemos realizado para llegar a exilon < 0.01 : ";
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
					cout << h << endl;
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
					muestraCoordCentros(v1, v2, 2); //Mostramos las coordenadas finales de los centros, el argumento tercero se le pasa 0 para mostrar centros iniciales, 1 nuevos calculados, y 2 para los finales
					cout << endl;
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
					cout << "---------------------------------------------------------------------------------" << endl;
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
					cout << endl;
					cout << "Quieres clasificar una muestra facilitada por ti? (s/n) " << endl;
					cin >> respuesta;
					if (respuesta == "n"){
						v1.clear();
						v2.clear();
					}
					else{
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
						cout << ">> ";
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
						cout << "Introduce las coordenadas de la muestra que quieres clasificar " << endl;
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
						cout << ">> ";
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
						cout << "La muestra ha de tener " << numDatos << " coordenadas: " << endl;
						while (g < numDatos){
							cin >> coord;
							vectorCoordClas.push_back(coord);
							g++;
						}
						distanciaCentro1 = sqrt(distanciaEuclidea(vectorCoordClas, v1)); //Distancia euclidea con raiz
						distanciaCentro2 = sqrt(distanciaEuclidea(vectorCoordClas, v2)); //Distancia euclidea con raiz
						if ((distanciaCentro1) < distanciaCentro2){
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
							cout << "--> ";
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
							cout << "La muestra introducida pertenece a la clase 1" << endl;
						}
						else{
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
							cout << ">> ";
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
							cout << "La muestra introducida pertenece a la clase 2" << endl;
						}
						v1.clear();
						v2.clear();
						g = 0;
						vectorCoordClas.clear();
					}
					cout << endl;
					break;
				case 3:
					v1.clear();
					v2.clear();
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
					cout << endl << "Calculando Medias M1 y M2..." << endl;
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
					//Calcula el valor de los centros y lo almacena en v1 y v2
					calculaMediasCentros(v1, v2, numMuestras, datosMuestras, numDatos, contadorClase1, contadorClase2);
					muestraCoordCentros(v1, v2, 1);
					calculaC(covarianza1, covarianza2, v1, v2, numMuestras, datosMuestras, numDatos, contadorClase1, contadorClase2);
					//A partir de aqui hay que calcular las matrices de covarianza...
					muestraMatrizCovarianza(covarianza1, covarianza2, numDatos);
					cout << endl << endl;

						break;
				case 4:
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
					cout << "--------------------------------------------------------------------------------------------";
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
					cout << endl << "Autor: Ruben Garcia Mateos, 02551077D" << endl;
					cout << "   Grado en Ingenieria del software   " << endl;
					cout << "Asignatura: Ingenieria del conocimiento" << endl;
					cout << "Programa elaborado: Metodos de clasificacion-> LLoyd, Kmeddias y Bayes" << endl;
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
					cout << "--------------------------------------------------------------------------------------------" << endl;
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
					cout << endl;
					break;
				case 5:
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
					cout << "Hasta pronto y muchas gracias por la prueba!"<< endl;
					system("PAUSE");
					return 0;
					break;

				default:
					break;
				}
				archivo.close();
				archivoOut.close();
			}
		}
	}
}

int mostrarMenu(){
	int opcion;

	cout << "1- Simulador algoritmo K-meddias" << endl;
	cout << "2- Simulador algoritmo LLoyd" << endl;
	cout << "3- Simulador algoritmo Parametrizacion Bayes" << endl;
	cout << "4- Visualizar datos autor de los simuladores" << endl;
	cout << "5- Salir" << endl;


	cin >> opcion;
	cout << endl;
	return opcion;
}

void calculaGradosDePertenencia(matrizGradosP matriz, datosMuestras datosMuestras, int numMuestras, int numDatos, centros v1, centros v2){
	vector<double> vectorAuxDatos;
	double pertenencia1, pertenencia2;
	int h = 0, i= 0, j = 0;
	while (h < numMuestras){
		while (i < numDatos){
			vectorAuxDatos.push_back(datosMuestras[j]);
			j++;
			i++;
		}
		i = 0;
		for (int i = 0; i < MAX_FIL; i++){
			if (i == 0){ //muestra i a la clase 1
				pertenencia1 = (1 / distanciaEuclidea(vectorAuxDatos, v1)) / ((1 / distanciaEuclidea(vectorAuxDatos, v1)) + (1 / distanciaEuclidea(vectorAuxDatos, v2)));
				matriz[i][h] = pertenencia1;
			}
			else{ //muestra i a la clase 2
				pertenencia2 = (1 / distanciaEuclidea(vectorAuxDatos, v2)) / ((1 / distanciaEuclidea(vectorAuxDatos, v1)) + (1 / distanciaEuclidea(vectorAuxDatos, v2)));
				matriz[i][h] = pertenencia2;
			}
		}
		h = h + 1;
		vectorAuxDatos.clear();
	}
}
double distanciaEuclidea(datosX  x, centros v){
	double distancia = 0;
	for (int i = 0; i < x.size(); i++){
		distancia=distancia + (pow((x[i] - v[i]),2));
	}
	return distancia;
}
vector<double> calculaCentro(matrizGradosP matriz, datosMuestras datosMuestras, int centro, int maxMuestras){
	double sumatorio1 = 0, sumatorioPorMuestra1 = 0, multiplicacion = 0, suma=0, suma1=0, suma2=0, suma3=0;
	vector<double> auxiliar, final, calculoTotal;
	int j = 0, h=0, x = 0;
	if (centro == 1){
		for (int i = 0; i < MAX_COL; i++){
			sumatorio1 = sumatorio1 + pow(matriz[0][i], 2); //Sumatorio del grado de pertenencia de todas las muestraas a la clase 1
		}
		for (int i = 0; i < MAX_COL; i++){
			while (j < maxMuestras){
				multiplicacion = (pow(matriz[0][i], 2)*datosMuestras[h]);
				auxiliar.push_back(multiplicacion);
				j++;
				h++;
			}
			j = 0;
		}
		j = 0;
		if (maxMuestras == 2){
			while (x < MAX_COL){
				suma = suma + auxiliar[j];
				suma1 = suma1 + auxiliar[j + 1];
				j = j + maxMuestras;
				x++;
			}
			calculoTotal.push_back(suma / sumatorio1);
			calculoTotal.push_back(suma1 / sumatorio1);
		}
		if (maxMuestras == 3){
			while (x < MAX_COL){
				suma = suma + auxiliar[j];
				suma1 = suma1 + auxiliar[j + 1];
				suma2 = suma2 + auxiliar[j + 2];
				j = j + maxMuestras;
				x++;
			}
			calculoTotal.push_back(suma / sumatorio1);
			calculoTotal.push_back(suma1 / sumatorio1);
			calculoTotal.push_back(suma2 / sumatorio1);
		}
		if (maxMuestras == 4){
			while (x < MAX_COL){
				suma = suma + auxiliar[j];
				suma1 = suma1 + auxiliar[j + 1];
				suma2 = suma2 + auxiliar[j + 2];
				suma3 = suma3 + auxiliar[j + 3];
				j = j + maxMuestras;
				x++;
			}
			calculoTotal.push_back(suma / sumatorio1);
			calculoTotal.push_back(suma1 / sumatorio1);
			calculoTotal.push_back(suma2 / sumatorio1);
			calculoTotal.push_back(suma3 / sumatorio1);
		}
		return calculoTotal;
	}
	else{
		sumatorio1 = 0;
		for (int i = 0; i < MAX_COL; i++){
			sumatorio1 = sumatorio1 + pow(matriz[1][i], 2); //Sumatorio del grado de pertenencia de todas las muestraas a la clase 2
		}
		for (int i = 0; i < MAX_COL; i++){
			while (j < maxMuestras){
				multiplicacion = (pow(matriz[1][i], 2)*datosMuestras[h]);
				auxiliar.push_back(multiplicacion);
				j++;
				h++;
			}
			j = 0;
		}
		j = 0;
		x = 0;
		if (maxMuestras == 2){
			while (x < MAX_COL){
				suma = suma + auxiliar[j];
				suma1 = suma1 + auxiliar[j + 1];
				j = j + maxMuestras;
				x++;
			}
			calculoTotal.push_back(suma / sumatorio1);
			calculoTotal.push_back(suma1 / sumatorio1);
		}
		if (maxMuestras == 3){
			while (x < MAX_COL){
				suma = suma + auxiliar[j];
				suma1 = suma1 + auxiliar[j + 1];
				suma2 = suma2 + auxiliar[j + 2];
				j = j + maxMuestras;
				x++;
			}
			calculoTotal.push_back(suma / sumatorio1);
			calculoTotal.push_back(suma1 / sumatorio1);
			calculoTotal.push_back(suma2 / sumatorio1);
		}
		if (maxMuestras == 4){
			while (x < MAX_COL){
				suma = suma + auxiliar[j];
				suma1 = suma1 + auxiliar[j + 1];
				suma2 = suma2 + auxiliar[j + 2];
				suma3 = suma3 + auxiliar[j + 3];
				j = j + maxMuestras;
				x++;
			}
			calculoTotal.push_back(suma / sumatorio1);
			calculoTotal.push_back(suma1 / sumatorio1);
			calculoTotal.push_back(suma2 / sumatorio1);
			calculoTotal.push_back(suma3 / sumatorio1);
		}
		return calculoTotal;

	}

}
void inicializaCentros(centros &v1, centros &v2, int numDatos){
	cout << "Inicializando centros..." << endl;
	if (numDatos == 2){
		//Inicializamos los centros con los datos facilitados por el profesor
		v1.push_back(4.6);
		v1.push_back(3.0);
		v2.push_back(6.8);
		v2.push_back(3.4);
	}
	if (numDatos == 3){
		v1.push_back(4.6);
		v1.push_back(3.0);
		v1.push_back(4.0);
		v2.push_back(6.8);
		v2.push_back(3.4);
		v2.push_back(4.6);
	}
	if (numDatos == 4){
		v1.push_back(4.6);
		v1.push_back(3.0);
		v1.push_back(4.0);
		v1.push_back(0.0);
		v2.push_back(6.8);
		v2.push_back(3.4);
		v2.push_back(4.6);
		v2.push_back(0.7);
	}
}
void muestraCoordCentros(centros v1, centros v2, int numero){
	if (numero == 0){
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
		cout << "--> ";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		cout << "El centro Inicial V1 es: (";
		for (int i = 0; i < v1.size(); i++){
			if (i != v1.size() - 1){
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
				cout << v1[i];
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
				cout << ", ";
			}
			else{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
				cout << v1[i];
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			}
		}
		cout << ")" << endl;

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
		cout << "--> ";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		cout << "El centro Inicial V2 es: (";
		for (int i = 0; i < v2.size(); i++){
			if (i != v2.size() - 1){
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
				cout << v2[i];
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
				cout << ", ";
			}
			else{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
				cout << v2[i];
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			}
		}
		cout << ")" << endl << endl;
	}
	else if(numero == 1){
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
		cout << "--> ";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		cout << "El nuevo centro V1 tiene las coordenadas: (";
		for (int i = 0; i < v1.size(); i++){
			if (i != v1.size() - 1){
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
				cout << v1[i];
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
				cout << ", ";
			}
			else{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
				cout << v1[i];
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			}
		}
		cout << ")" << endl;

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
		cout << "--> ";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		cout << "El nuevo centro V2 tiene las coordenadas: (";
		for (int i = 0; i < v2.size(); i++){
			if (i != v2.size() - 1){
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
				cout << v2[i];
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
				cout << ", ";
			}
			else{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
				cout << v2[i];
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			}
		}
		cout << ")" << endl << endl;
	}
	else{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
		cout << "--> ";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		cout << "El centro Final V1 es: (";
		for (int i = 0; i < v1.size(); i++){
			if (i != v1.size() - 1){
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
				cout << v1[i];
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
				cout << ", ";
			}
			else{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
				cout << v1[i];
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			}
		}
		cout << ")" << endl;

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
		cout << "--> ";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		cout << "El centro Final V2 es: (";
		for (int i = 0; i < v2.size(); i++){
			if (i != v2.size() - 1){
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
				cout << v2[i];
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
				cout << ", ";
			}
			else{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
				cout << v2[i];
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			}
		}
		cout << ")" << endl << endl;
	}
}
void muestraExilon(double distanciaCentro1, double distanciaCentro2, int numero){ //numero --> 0 para k meddias, 1 para LLoyd
	cout << "E: exilon en V1 vale: € = " << distanciaCentro1 << endl;
	cout << "E: exilon en V2 vale: € = " << distanciaCentro2 << endl;
	if (numero == 0){
		if ((distanciaCentro1 > toleranciakMeddias) || (distanciaCentro2 > toleranciakMeddias))
			cout << "Como al menos uno de los centros V1 o V2 es > Exilon(" << toleranciakMeddias << "),tenemos que continuar..." << endl << endl;
		else{
			cout << "Se han encontrado los centros adecuados!!!, Hemos Terminado." << endl;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
			cout << "                      FIN" << endl;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			cout  << endl;
		}
	}
	else{
		if ((distanciaCentro1 > toleranciaLLoyd) || (distanciaCentro2 > toleranciaLLoyd))
			cout << "Como al menos uno de los centros V1 o V2 es > Exilon(" << toleranciaLLoyd << "), tenemos que continuar..." << endl << endl;
		else{
			cout << "Se han encontrado los centros adecuados!!!, Hemos Terminado." << endl;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
			cout << "                      FIN" << endl;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			cout  << endl;
		}
	}
}
void calculaMediasCentros(centros &v1, centros& v2, int numMuestras, datosMuestras datosMuestras, int numDatos, int contadorClase1, int contadorClase2){
	double mediaM1 = 0, mediaM2 = 0, sumatorioMuestras = 0;
	int d = 0, datos = 0;
	while (datos < numDatos){
		d = 0;
		sumatorioMuestras = 0;
		mediaM1 = 0;
		for (int j = 0; j < numDatos; j++){
			while (d < numDatos*contadorClase1){
				sumatorioMuestras = sumatorioMuestras + datosMuestras[d + datos];
				d = d + numDatos;
			}
		}
		mediaM1 = sumatorioMuestras / (numMuestras - contadorClase2);
		v1.push_back(mediaM1);
		datos++;
	}

	d = 0;
	datos = 0;
	sumatorioMuestras = 0;
	while (datos < numDatos){
		d = 0;
		sumatorioMuestras = 0;
		mediaM2 = 0;
		for (int j = 0; j < numDatos; j++){
			while (d < numDatos*contadorClase2){
				sumatorioMuestras = sumatorioMuestras + datosMuestras[(numDatos * contadorClase2) + datos + d];
				d = d + numDatos;
			}
		}
		mediaM2 = sumatorioMuestras / (numMuestras - contadorClase2);
		v2.push_back(mediaM2);
		datos++;
	}
}
void calculaC(covarianza matrizCovarianza1, covarianza matrizCovarianza2, centros v1, centros v2, int numMuestras, datosMuestras datosMuestras, int numDatos, int contadorClase1, int contadorClase2){
	covarianza multiplicacion;
	vector<double> restaVectores, restaVectoresTranspuesta;
	
	for (int i = 0; i < numDatos; i++){
		for (int j = 0; j < numDatos; j++){
			matrizCovarianza1[i][j] = 0; //inicializamos la matriz de covarianza a 0
			matrizCovarianza2[i][j] = 0;
		}
	}

	//---------------------------------------------PARA LA MATRIZ PRIMERA------------------------------------------------------------------------
	int contador = 0, x = 0;
	while (contador < contadorClase1){
		for (int i = 0; i < numDatos; i++){
			restaVectores.push_back(datosMuestras[x+i] - v1[i]);
			restaVectoresTranspuesta.push_back(datosMuestras[x+i] - v1[i]);
		}
		x = x + numDatos;
		for (int i = 0; i < numDatos; i++){
			for (int j = 0; j < numDatos; j++){
				multiplicacion[i][j] = restaVectoresTranspuesta[i] * restaVectores[j]; //Aqui tenemos guardado el cálculo de la (X-M)*(X-M)t
			}
		}

		for (int i = 0; i < numDatos; i++){
			for (int j = 0; j < numDatos; j++){
				matrizCovarianza1[i][j] = matrizCovarianza1[i][j] + multiplicacion[i][j]; //Aqui tenemos guardado el cálculo de la (X-M)*(X-M)t
			}
		}

		for (int i = 0; i < numDatos; i++){
			for (int j = 0; j < numDatos; j++){
				multiplicacion[i][j] = 0; //vaciamos la matriz multiplicacion
			}
		}
		restaVectores.clear();
		restaVectoresTranspuesta.clear();
		contador++;
	}
	//---------------------------------------------------------------------------------------------------------------------

	//---------------------------------------------------PARA LA SEGUNDA MATRIZ C2---------------------------------------------------------

	restaVectores.clear();
	restaVectoresTranspuesta.clear();
	contador = 0, x = numDatos*contadorClase1;
	while (contador < contadorClase2){
		for (int i = 0; i < numDatos; i++){
			restaVectores.push_back(datosMuestras[x + i] - v2[i]);
			restaVectoresTranspuesta.push_back(datosMuestras[x + i] - v2[i]);
		}
		x = x + numDatos;
		for (int i = 0; i < numDatos; i++){
			for (int j = 0; j < numDatos; j++){
				multiplicacion[i][j] = restaVectoresTranspuesta[i] * restaVectores[j]; //Aqui tenemos guardado el cálculo de la (X-M)*(X-M)t
			}
		}

		for (int i = 0; i < numDatos; i++){
			for (int j = 0; j < numDatos; j++){
				matrizCovarianza2[i][j] = matrizCovarianza2[i][j] + multiplicacion[i][j]; //Aqui tenemos guardado el cálculo de la (X-M)*(X-M)t
			}
		}

		for (int i = 0; i < numDatos; i++){
			for (int j = 0; j < numDatos; j++){
				multiplicacion[i][j] = 0; //vaciamos la matriz multiplicacion
			}
		}
		restaVectores.clear();
		restaVectoresTranspuesta.clear();
		contador++;
	}

	//------------------------------------------------------------------------------------------------------------------------------------
	for (int i = 0; i < numDatos; i++){
		for (int j = 0; j < numDatos; j++){
			matrizCovarianza1[i][j] = matrizCovarianza1[i][j]/contadorClase1; //Aqui tenemos guardado el cálculo de la (X-M)*(X-M)t
			matrizCovarianza2[i][j] = matrizCovarianza2[i][j] / contadorClase2; //Aqui tenemos guardado el cálculo de la (X-M)*(X-M)t

		}
	}


}
void calculaMatrizTranspuesta(covarianza matrizCovarianza, covarianza matrizTranspuesta){
	for (int i = 0; i < MAX_CELDAS; i++){
		for (int j = 0; j < MAX_CELDAS; j++){
			matrizTranspuesta[j][i] = matrizCovarianza[i][j];
		}
	}
}
void muestraMatrizCovarianza(covarianza covarianza1, covarianza covarianza2, int numDatos){
	cout << "La matriz de covarianza C1: " << endl << endl;
	for (int i = 0; i < numDatos; i++){
		cout << "  |  ";
		for (int j = 0; j < numDatos; j++){
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
			cout << setprecision(2) << covarianza1[i][j] << "  ";
		}
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		cout  << "|" << endl;
	}
	cout << endl << "La matriz de covarianza C2: " << endl << endl;
	for (int i = 0; i < numDatos; i++){
		cout << "  |  ";
		for (int j = 0; j < numDatos; j++){
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
			cout << setprecision(2) << covarianza2[i][j] << "  ";
		}
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		cout << "|" << endl;
	}
}
void calculaDistancias(centros &v1, centros&v2, int numMuestras, int numDatos, datosMuestras datosMuestras){
	vector<double> vectorAuxDatos, vectorCalculos;
	int i = 0, h = 0, j = 0, centro = 0;
	double distanciaE = 0, menorDistancia = 100, aux;
	while (h < numMuestras){
		while (i < numDatos){
			vectorAuxDatos.push_back(datosMuestras[j]);
			j++;
			i++;
		}
		i = 0;
		distanciaE = sqrt(distanciaEuclidea(vectorAuxDatos, v1));
		if (distanciaE <= menorDistancia){
			menorDistancia = distanciaE;
			centro = 1;
		}
		distanciaE = sqrt(distanciaEuclidea(vectorAuxDatos, v2));
		if (distanciaE <= menorDistancia){
			menorDistancia = distanciaE;
			centro = 2;
		}
		if (centro == 1){ //Centro 1 es el mas pequeño, se actuliza V1
			cout << "Como D1 es la mas pequeña, actualizamos V1: " << endl;
			for (int i = 0; i < numDatos; i++){
				aux = (vectorAuxDatos[i] - v1[i]);
				vectorCalculos.push_back(v1[i] + (razon_Aprendizaje*aux));
			}
			for (int i = 0; i < v1.size(); i++) v1[i] = vectorCalculos[i]; //Actualizamos V1
			muestraCoordCentros(v1, v2, 1);
		}
		else{ //Centro 2 es el mas pequeño, se actualiza V2
			cout << "Como D2 es la mas pequeña, actualizamos V2: " << endl;
			for (int i = 0; i < numDatos; i++){
				aux = (vectorAuxDatos[i] - v2[i]);
				vectorCalculos.push_back(v2[i] + (razon_Aprendizaje*aux));
			}
			for (int i = 0; i < v2.size(); i++) v2[i] = vectorCalculos[i]; //Actualizamos V2
			muestraCoordCentros(v1, v2, 1);
		}
		menorDistancia = 100;
		vectorAuxDatos.clear();
		vectorCalculos.clear();
		h++;
		cout << endl << endl;
	}
}
