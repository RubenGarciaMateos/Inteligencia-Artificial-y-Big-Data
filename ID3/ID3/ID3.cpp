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

using namespace std;

double calculaInformacion(double p, double n);
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
int main(){
	ifstream archivo1, archivo2;
	double numAtributos = 0, numCasos = 0, contador;
	string linea;
	string str = "";
	vector<string> atributos;
	vector<string> ejemplos;
	vector<string> auxiliar;
	vector<string> columna;
	vector<string> final;
	vector<string> vectorDecision;
	double j = 0, k, z = 0, casosPositivos = 0, casosNegativos = 0;
	string ejemplo, nombreArch1, nombreArch2;
	double porcentajePositivos, porcentajeNegativos, meritoAcumulado = 0, meritoActual;
	int contadorColumna = 0;
	double meritoMasPequeño = 3.0;
	int numeroColumnas, columnadelMeritomasPequeño;
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	cout << "Introduce el nombre del archivo que contiene los atributos del juego" << endl;
	cin >> nombreArch1;
	cout << "Introduce el nombre del archivo que contiene los datos" << endl;
	cin >> nombreArch2;
	archivo1.open(nombreArch1);
	archivo2.open(nombreArch2);
	if (!archivo1.is_open() || !archivo2.is_open()){
		cout << "Error al abrir alguno de los archivos" << endl;
	}
	else{
		//Contamos el numero de casos que hay en la tabla
		while (!archivo2.eof()){
			getline(archivo2, linea);
			numCasos++;
		}
		archivo2.close();

		//Nos quedamos con los diferentes atributos y los guardamos en el array atributos
		getline(archivo1, linea);
		for (int i = 0; i < linea.length(); i++){
			if (linea[i] == ','){
				atributos.push_back(str);
				numAtributos++;
				str = "";
			}
			else{
				str += linea[i];
			}
		}
		atributos.push_back(str);
		numAtributos++;

		str = "";
		archivo2.open(nombreArch2);
		//Nos quedamos con los ejemplos y los guardamos en el array ejemplos
		getline(archivo2, linea);
		while (!archivo2.eof()){
			for (int i = 0; i < linea.length(); i++){
				if (linea[i] != ','){
					str += linea[i];
				}
				else{
					ejemplos.push_back(str);
					str = "";
				}
			}
			getline(archivo2, linea);
			ejemplos.push_back(str);
			str = "";
		}
		//Aqui se vulve a realizar el for para coger la ultima linea del archivo
		for (int i = 0; i < linea.length(); i++){
			if (linea[i] != ','){
				str += linea[i];
			}
			else{
				ejemplos.push_back(str);
				str = "";
			}
		}
		ejemplos.push_back(str);
		str = "";

		// A PARTIR DE AQUI YA TENEMOS TODOS LOS DATOS GUARDADOS EN LOS ARRAYS EJEMPLOS Y ATRIBUTOS
		numeroColumnas = numAtributos;

		//ESTO PARA MOSTRAR LA TABLA INICIAL DEL ARCHIVO TXT
		//-------------------------------------------------------------------------------

		cout << endl << endl << "1: MOSTRAMOS LA TABLA QUE SE DA EN LOS ARCHIVOS TXT CON LOS ATRIBUTOS " << endl << endl;


		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
		cout << "----------------------------------------------------------------" << endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		for (int i = 0; i < atributos.size(); i++){
			cout << atributos[i] << "  ";
		}
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
		cout << endl <<  "----------------------------------------------------------------" << endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		cout << endl;
		int j = 0, contador = 0;
		for (int i = 0; i < numCasos; i++){
			j = 0;
			while(j < numeroColumnas){
				cout << "  " << ejemplos[contador] << "    ";
				j++;
				contador++;
			}
			cout << endl;
		}
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
		cout << endl << "----------------------------------------------------------------" << endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

		//------------------------------------------------------------------------------

		cout << endl << endl << "2: VAMOS CALCULANDO EL MERITO DE LOS DISTINTOS ATRIBUTOS PARA OBTENER EL PRIMER NODO " << endl << endl;

		//En columna guardamos los valores de la columna, en nuestro caso se corresponde con el tiempo la primera
		for (int i = contadorColumna; i < ejemplos.size(); i = i + numeroColumnas){
			columna.push_back(ejemplos[i]);
		}
		contadorColumna++;

		//Guardamos en el vectordecision los valores de la ultima columna
		for (int i = numeroColumnas-1; i < ejemplos.size(); i = i + numeroColumnas){
			vectorDecision.push_back(ejemplos[i]);
		}
		
		//Con este algoritmo nos quedamos con los diferentes valores que hay en el array de la columna y los guardamos en el vector final
		for (int i = contadorColumna; i < columna.size(); i++){
			contador = 0;
			ejemplo = columna[i];
			auxiliar.push_back(ejemplo);

			for (k = 0; k < auxiliar.size(); k++){
				if (auxiliar[k] == ejemplo){
					contador++;
				}
			}
			if (contador == 1){
				final.push_back(ejemplo);
			}
		}

		//Ahora tenemos en el array columna, todos los valores de la columna
		//En el array final los valores diferentes que hay sin repetir
		cout << endl << endl;
		while ((numAtributos - 1) > 0){
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
			cout << "----------------------------------------------------------------" << endl;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			cout << "                ATRIBUTO " << contadorColumna << ": ";
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
			cout << atributos[contadorColumna - 1] << endl;
			cout << "----------------------------------------------------------------" << endl << endl;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			for (int i = 0; i < final.size(); i++){
				contador = 0;
				for (int j = 0; j < columna.size(); j++){
					if (columna[j] == final[i]){
						contador++;
					}
				}
				for (int h = 0; h < columna.size(); h++){
					if ((columna[h] == final[i]) && (vectorDecision[h] == "si")){
						casosPositivos++;
					}
					else
						if ((columna[h] == final[i]) && (vectorDecision[h] == "no")){
							casosNegativos++;
						}
				}
				cout << "El numero de casos positivos de " << "'";
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
				cout << final[i];
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
				cout << "'" << " es " << casosPositivos << " y negativos " << casosNegativos << endl;
				porcentajePositivos = casosPositivos / contador;
				cout << "El porcentaje de los positivos: " << porcentajePositivos << endl;
				porcentajeNegativos = casosNegativos / contador;
				cout << "El porcentaje de los negativos: " << porcentajeNegativos << endl;
				//El merito actual es el merito de cada sumatorio de los casos dentro del atributo
				meritoActual = (contador / numCasos) * calculaInformacion(porcentajePositivos, porcentajeNegativos);
				meritoAcumulado = meritoAcumulado + meritoActual;

				casosPositivos = 0;
				casosNegativos = 0;
				cout << endl << endl;
			}
			cout << endl << "Merito del Nodo " << atributos[contadorColumna-1] << ":  " << meritoAcumulado << endl << endl << endl;
			if (meritoAcumulado < meritoMasPequeño){
				meritoMasPequeño = meritoAcumulado;
				columnadelMeritomasPequeño = contadorColumna;
			}

			columna.clear();
			final.clear();
			auxiliar.clear();
			numAtributos--;
			meritoAcumulado = 0;
			//Volvemos a introducir los datos de la siguiente columna en el array columna
			for (int i = contadorColumna; i < ejemplos.size(); i = i + numeroColumnas){
				columna.push_back(ejemplos[i]);
			}
			//Guardamos en el array final los datos que hay en la columna sin repetir
			for (int i = contadorColumna; i < columna.size(); i++){
				contador = 0;
				ejemplo = columna[i];
				auxiliar.push_back(ejemplo);

				for (k = 0; k < auxiliar.size(); k++){
					if (auxiliar[k] == ejemplo){
						contador++;
					}
				}
				if (contador == 1){
					final.push_back(ejemplo);
				}
			}
			contadorColumna++;
		}
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
	cout << "----------------------------------------------------------------" << endl << endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

	cout << endl << "el merito mas pequeño es : " << meritoMasPequeño << " y su columna es: " << columnadelMeritomasPequeño << endl;
	cout << "PRIMER NODO DEL ID3: ";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
	cout << " ---->  " << atributos[columnadelMeritomasPequeño - 1] << "  <----" << endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	cout << endl;
	archivo1.close();
	archivo2.close();
	system("PAUSE");

}


double calculaInformacion(double p, double n){
	double retorno;
	if ((p == 0) || (n == 0)) return 0;
	else retorno = ((-p*log2(p)) - (n*log2(n)));
	return retorno;
}