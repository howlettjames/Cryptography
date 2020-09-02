#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <time.h>
#include <fstream>
//#include "libraryCipher.h"

using namespace std;

int **generateKey();
string cipher(int **K, char *path);
int matrixDeterminant(int **K);
void llenadoMatriz(int **K);
int validate_euclid_algorithm(int number);

int main(){
	int **K;
	char *path;
	string dir;
	//Se genera la llave
	K = generateKey();
	//Se obtiene la dirección del archivo con el texto a cifrar
	cout << "Write filename: " << endl;
	getline(cin, dir);
	cout << "Filename: " << dir << endl;
	path = const_cast<char*>(dir.c_str());
	ofstream writer("pt22hill.txt");
	if(!writer)
	{
		cout << "Can't open file: <pt22hill.txt>" << endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		writer << cipher(K, path) << endl;
		writer.close();
	}
	return 0;
}

int **generateKey(){
	int det=0, n=95;
	int **K = (int**)malloc(sizeof(int*)*3);
	//Se crea la matriz K
	for(int i = 0; i < 3; i++)
		K[i] = (int*)malloc(sizeof(int)*3);
	//Se generan llaves hasta que una sea válida
	do{
		llenadoMatriz(K);
		det = matrixDeterminant(K);
	}while(validate_euclid_algorithm(det) != 1);
	return K;
}

string cipher(int **K, char *path){
	fstream f;
	int car[3], new_val[3];
	int cont, ck=0, aux=0, cont2, cont3=0;
	string aux_msj, msj, cip_msj;
	//Lectura del archivo
	f.open(path);
	if(f.is_open()){
		while(!f.eof()){
			getline(f, aux_msj);
			msj += aux_msj; 
		}
		f.close();
	}
	cout << msj << endl;
	/*Cifrado de texto
	Ciclo que obtiene el grupo de 3 caracteres*/
	for(cont3 = 0; cont3 < msj.size(); cont3+=3){
		if(msj[cont3] >= 32 && msj[cont3] <= 126)
			car[0] = (int)msj[cont3] - 32;
		else{
			while(msj[cont3] <= 32 || msj[cont3] >= 126)
				cont3++;
			car[0] = (int)msj[cont3] - 32;
		}

		if(msj[cont3 + 1] >= 32 && msj[cont3 + 1] <= 126)
			car[1] = (int)msj[cont3 + 1] - 32;
		else{
			while(msj[cont3+1] <= 32 || msj[cont3+1] >= 126)
				cont3++;
			car[1] = (int)msj[cont3 + 1] - 32;
		}

		if(msj[cont3 + 2] >= 32 && msj[cont3 + 2] <= 126)
			car[2] = (int)msj[cont3 + 2] - 32;
		else{
			while(msj[cont3+2] <= 32 || msj[cont3+2] >= 126)
				cont3++;
			car[2] = (int)msj[cont3 + 2] - 32;
		}

		//Se realiza la multiplicacion de las matrices para obtener los nuevos valores
		for(cont = 0; cont < 3; cont++){
			for(cont2 = 0; cont2 < 3; cont2++)
			//Se obtiene Aij
				aux=aux+(car[cont2] * K[cont2][cont]);
			new_val[cont] = aux % 95;
			aux = 0;
			cip_msj += (new_val[cont] + 32);
		}
	}
	return cip_msj;
}

int validate_euclid_algorithm(int number)
{
	int n = 95, remainder = 0, previous_remainder;	
	int aux_number = number;

	printf("\n-----CALL: validate_euclid_algorithm()-----\n");
	//Validating "determinant" using the Euclid gcd algorithm
	n = 95;
	remainder = 0;
	while(1)
	{
		remainder = number % n;
		number = n;
		n = remainder;
		if(remainder == 0)
			break;
		previous_remainder = remainder;
	}
	if(previous_remainder != 1)
	{
		printf("|+|ERROR: Determinant not valid, determinant does not satisfy gdc(%d, 95) = 1.\n",aux_number);
		printf("-----RETURN ON ERROR: validate_euclid_algorithm()-----\n");
		return 0;
	}
	printf("-----RETURN ON SUCCESS: validate_euclid_algorithm()-----\n");
	return 1;
}

int matrixDeterminant(int **K){
	int det = K[0][0] * K[1][1] * K[2][2] + K[0][1] * K[1][2] * K[2][0] + K[0][2] * K[1][0] * K[2][1] - (K[0][0] * K[1][2] * K[2][1] + K[0][1] * K[1][0] * K[2][2] + K[0][2] * K[1][1] * K[2][0]);
	return det;
}

void llenadoMatriz(int **K){
	srand(time(NULL));
	int num = 0, cont, cont2;
	FILE *f;
	//Se crea el archivo donde se guardará la llave
	f = fopen("key2.txt", "w");
	for(cont = 0; cont < 3; cont++){
		fprintf(f, "(\t");
		for(cont2 = 0; cont2 < 3; cont2++){
			//Se genera aleatoriamente el elemento de la matriz y se agrega a ésta
			num = rand() % 10;
			K[cont][cont2] = num;
			fprintf(f, "%d\t", K[cont][cont2]);
		}
		fprintf(f, ")\n");
	}
	fclose(f);
}