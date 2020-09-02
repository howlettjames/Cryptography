#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

typedef unsigned char BYTE;

int main(void)
{
	int option = 0, key[3][3], x = 0, n = 0, remainder = 0, aux = 0, previousRemainder = 0, k_index = 0, index = 0;
	int i = 0, j = 0, k = 0, l = 0, m = 0, n1 = 0, minor[2][2];
	int array[100][4], array1[100][2], factor1 = 0, factor2 = 0, product1 = 0, product2 = 0, detInverse = 0, matrix_minors[3][3];
	BYTE msg[3];
	long determinant = 0;
	char *key_file = "key.txt", file_name[FILENAME_MAX], k_file_read[FILENAME_MAX], ch;
	FILE *read_fp, *write_fp;

	srand((unsigned) time(NULL));
	while(1)
	{
		printf("\n|+|+|+| HILL CIPHER |+|+|+|\n");
		printf("1. Generate key\n");
		printf("2. Encipher\n");
		printf("3. Decipher\n");
		printf("4. Exit\n");
		printf("Choose an option: ");
		scanf(" %d", &option);
		getchar();
		switch(option)
		{
			case 1:
				tryagainkey:
				printf("Generating key...\n");		
				//Creating new file
				write_fp = fopen(key_file, "w");
				for(i = 0; i < 3; i++)
				{
					for(j = 0; j < 3; j++)
					{
						key[i][j] = rand() % 95;
						fprintf(write_fp, "%d ", key[i][j]);	
					}
					fprintf(write_fp, "\n");	
				}
				fclose(write_fp);
				printf("Key generated.\n");	
				printf("Calculating determinant...\n");
				determinant = 0;
				for(x = 0; x < 6; x++)
				{
					if(x == 0)
						determinant += key[0][0] * key[1][1] * key[2][2];
					else if(x == 1)
						determinant += key[0][1] * key[1][2] * key[2][0];	
					else if(x == 2)
						determinant += key[0][2] * key[1][0] * key[2][1];
					else if(x == 3)
						determinant += -(key[2][0] * key[1][1] * key[0][2]);		
					else if(x == 4)
						determinant += -(key[2][1] * key[1][2] * key[0][0]);
					else if(x == 5)
						determinant += -(key[2][2] * key[1][0] * key[0][1]);
				}
				if(determinant < 0)
					determinant = 95 - (-determinant % 95);
				else
					determinant = determinant % 95;
				
				printf("Determinant = %d\n", determinant);
				printf("Validating gcd(%d, 95) = 1\n", determinant);
				//Validating "a" using the Euclid gcd algorithm
				n = 95;
				remainder = 0;
				aux = determinant;
				while(1)
				{
					remainder = aux % n;
					aux = n;
					n = remainder;
					if(remainder == 0)
						break;
					previousRemainder = remainder;
				}
				if(previousRemainder != 1)
				{
					printf("|+|+|+|ERROR: Key not valid. Trying again.\n\n\n");
					fclose(write_fp);
					goto tryagainkey;
				}
				printf("Validated.\n");
			break;
			case 2:
				tryagainkeyfile:
				printf("|+|Enter the file name with the key (it may be an absolute path): ");
				scanf("%s", k_file_read);
				if((read_fp = fopen(k_file_read, "rb")) == NULL)
				{
					printf("|+|+|+|ERROR: Can't open: %s. Try again.\n", k_file_read);
					goto tryagainkeyfile;
				}

				printf("Reading matrix...\n");
				for(i = 0; i < 3; i++)
					for(j = 0; j < 3; j++)
						fscanf(read_fp, "%d", &key[i][j]);
				printf("Matrix readed\n");	
				fclose(read_fp);

				tryagainptfile:
				printf("|+|Enter the file name with the plaintext (it may be an absolute path): ");
				scanf("%s", file_name);
				if((read_fp = fopen(file_name, "rb")) == NULL)
				{
					printf("|+|+|+|ERROR: Can't open: %s. Try again.\n", file_name);
					goto tryagainptfile;
				}
				//Appending "hill" to what will be the new file with the ciphertext
				i = 0;
				while(file_name[i++] != '.')
					;
				i--;
				file_name[i++] = 'h';
				file_name[i++] = 'i';
				file_name[i++] = 'l';
				file_name[i++] = 'l';
				file_name[i++] = '.';
				file_name[i++] = 't';
				file_name[i++] = 'x';
				file_name[i++] = 't';
				file_name[i++] = '\0';
				//Creating new file to write ciphertext
				write_fp = fopen(file_name, "w");
				//ENCIPHERING
				i = 0;
				printf("Enciphering...\n");
				while((ch = getc(read_fp)) != EOF) 
				{
					if(ch >= 32 && ch < 127)
					{
						msg[i++] = ch - 32;	
						if(i > 2)
						{
							i = 0;
							for(j = 0; j < 3; j++)
								putc((((msg[0] * key[0][j]) + (msg[1] * key[1][j]) + (msg[2] * key[2][j])) % 95) + 32, write_fp);
						}
					}
					//else
						//putc(ch, write_fp);
				}
				printf("Enciphered\n");
				fclose(write_fp);
				fclose(read_fp);
			break;
			case 3:
				// ------------------------------------------------- READING KEY -------------------------------------------
				tryagainkeyfile1:
				printf("|+|Enter the file name with the key (it may be an absolute path): ");
				scanf("%s", k_file_read);
				if((read_fp = fopen(k_file_read, "rb")) == NULL)
				{
					printf("|+|+|+|ERROR: Can't open: %s. Try again.\n", k_file_read);
					goto tryagainkeyfile1;
				}

				printf("Reading matrix...\n");
				for(i = 0; i < 3; i++)
					for(j = 0; j < 3; j++)
						fscanf(read_fp, "%d", &key[i][j]);
				printf("Matrix readed\n");	
				fclose(read_fp);

				// ------------------------------------------------- CALCULATING KEY INVERSE-------------------------------------------
				printf("Calculating inverse of key\n");
				printf("Step One: Calculating determinant...\n");
				determinant = 0;
				for(x = 0; x < 6; x++)
				{
					if(x == 0)
						determinant += key[0][0] * key[1][1] * key[2][2];
					else if(x == 1)
						determinant += key[0][1] * key[1][2] * key[2][0];	
					else if(x == 2)
						determinant += key[0][2] * key[1][0] * key[2][1];
					else if(x == 3)
						determinant += -(key[2][0] * key[1][1] * key[0][2]);		
					else if(x == 4)
						determinant += -(key[2][1] * key[1][2] * key[0][0]);
					else if(x == 5)
						determinant += -(key[2][2] * key[1][0] * key[0][1]);
				}
				if(determinant < 0)
					determinant = 95 - (-determinant % 95);
				else
					determinant = determinant % 95;
				
				printf("Determinant = %d\n", determinant);
				printf("Step Two: Validating gcd(%d, 95) = 1\n", determinant);
				//Validating "determinant" using the Euclid gcd algorithm
				n = 95;
				remainder = 0;
				aux = determinant;
				while(1)
				{
					remainder = aux % n;
					aux = n;
					n = remainder;
					if(remainder == 0)
						break;
					previousRemainder = remainder;
				}
				if(previousRemainder != 1)
				{
					printf("|+|+|+|ERROR: Key not valid. Trying again.\n\n\n");
					fclose(write_fp);
					exit(EXIT_FAILURE);
				}
				printf("Validated.\n");
				printf("Step Three: Finding inverse of determinant using Extended Euclid Algorithm\n");
				n = 95;
				index = 0;
				while(1) 
				{
					remainder = n % determinant;
					if(remainder == 0)
						break;

					array[index][0] = remainder;
					array[index][1] = n;
					array[index][2] = determinant;
					array[index][3] = - (n - remainder) / determinant; 

					n = determinant;
					determinant = remainder;
					index++;
				}
				array1[0][0] = 1;
				array1[0][1] = array[0][3];
				factor1 = array[0][1];
				factor2 = array[0][2];
				for(i = 1; i < index; i++)
				{
					product1 = array[i][3] * array1[i - 1][0];
					product2 = array[i][3] * array1[i - 1][1];

					if(array[i][1] == factor1)
						product1++;
					else if(array[i][1] == factor2)
						product2++;
					else
					{
						for(j = 0; j < index; j++)
							if(array[j][0] == array[i][1])
								break;
						product1 += array1[j][0];
						product2 += array1[j][1];
					}
					array1[i][0] = product1;
					array1[i][1] = product2;
				}
				detInverse = array1[index - 1][1];
				if(detInverse < 0)
					detInverse = 95 - (-detInverse % 95);

				printf("Determinant Inverse: %d\n", detInverse);
				printf("Step Four: Transpose Matrix\n");
				aux = key[0][1];
				key[0][1] = key[1][0];
				key[1][0] = aux;

				aux = key[0][2];
				key[0][2] = key[2][0];
				key[2][0] = aux;

				aux = key[1][2];
				key[1][2] = key[2][1];
				key[2][1] = aux;

				printf("Step Five: Matrix of Minors\n");
				for(i = 0; i < 3; i++)
				{
					for(j = 0; j < 3; j++)
					{
						//Building minor for a_ij
						m = 0;
						for(k = 0; k < 3; k++)
						{	
							if(k != i)
							{
								n1 = 0;
								for(l = 0; l < 3; l++)
									if(l != j)
									{
										minor[m][n1++] = key[k][l];
									}
								m++;	
							}
						}
						matrix_minors[i][j] = minor[0][0] * minor[1][1] - minor[1][0] * minor[0][1];
					}
				}
				
				printf("Step Six: Adjugate, Matrix of Cofactors\n");
				for(i = 0; i < 3; i++)
					for(j = 0; j < 3; j++)
						matrix_minors[i][j] *= (int) pow(-1.0, (double) (i + j));

				printf("Step Seven: Applying mod 95 to all entries in the Adjugate Matrix\n");	
				for(i = 0; i < 3; i++)
					for(j = 0; j < 3; j++)
						if(matrix_minors[i][j] < 0)
							matrix_minors[i][j] = 95 - (-matrix_minors[i][j] % 95);
						else if(matrix_minors[i][j] > 95)
							matrix_minors[i][j] = matrix_minors[i][j] % 95;

				printf("Step Eight and Final: Multiplying each entry by the inverse of the determinant and applying mod 95\n");		
				for(i = 0; i < 3; i++)
					for(j = 0; j < 3; j++)
						matrix_minors[i][j] = (matrix_minors[i][j] * detInverse) % 95;		

				for(i = 0; i < 3; i++)
				{
					for(j = 0; j < 3; j++)
						printf("%d ", matrix_minors[i][j]);
					printf("\n");
				}
	
				// ------------------------------------------------- DECIPHERING-------------------------------------------
				tryagainhillfile:
				printf("|+|Enter the file name with the ciphertext (it may be an absolute path): ");
				scanf("%s", file_name);
				if((read_fp = fopen(file_name, "rb")) == NULL)
				{
					printf("|+|+|+|ERROR: Can't open: %s. Try again.\n", file_name);
					goto tryagainhillfile;
				}
				//Creating new file to write plaintext
				write_fp = fopen("deciphered.txt", "w");
				//ENCIPHERING
				i = 0;
				printf("Deciphering...\n");
				while((ch = getc(read_fp)) != EOF)
				{
					if(ch > 32 && ch < 127)
					{
						msg[i++] = ch - 32;	
						if(i > 2)
						{
							i = 0;
							for(j = 0; j < 3; j++)
								putc(((msg[0] * matrix_minors[0][j] + msg[1] * matrix_minors[1][j] + msg[2] * matrix_minors[2][j]) % 95) + 32, write_fp);
						}
					}
					else
						putc(ch, write_fp);
				}
				printf("Deciphered\n");
				fclose(write_fp);
				fclose(read_fp);
			break;
			case 4:
				exit(EXIT_SUCCESS);
			break;
			default:
				printf("Choose a correct option.\n\n");
				Sleep(2000);
			break;
		}
	}
	return 0;
}