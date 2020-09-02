// Date: March 3rd 2019
// Programmers: Bastida Jaime and Solorio Daniel
// Abstract: Program that implements an analysis over the Hill Cipher to find a key from a plaintext and an ciphertext.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>

typedef unsigned char BYTE;

int ** find_inverse_matrix(int **x, bool *valid_determinant);
bool validate_euclid_algorithm(int number);
int find_det_inverse(int number);
int ** allocate_memory();
void open_file(const char * prompt, char **file_name, FILE **read_fp);
void move_offset(FILE **file_pointer, long offset);
int * read_sample(FILE **file_pointer, long sample_len);

int main(void)
{
	int option = 0, msg[3], *plaintext, *ciphertext;
	int **x, **y, **x_inverse;
	int **key, **key_inverse;
	int j, k, no_attempts;
	long i, low;
	char *plaintext_fn, *ciphertext_fn, ch;
	char decipheredtext[FILENAME_MAX], plaintext_test[FILENAME_MAX];
	const char *deciphered_fn = "deciphered.txt";
	const char *key_fn = "key.txt";
	FILE *plaintext_fp, *plaintext_fp2, *ciphertext_fp, *ciphertext_fp2, *write_fp;
	bool input_correct, valid_determinant;

	srand((unsigned) time(NULL));
	//Allocating memory for x, y and x_inverse
	x = allocate_memory();
	y = allocate_memory();
	key = allocate_memory();
	x_inverse = allocate_memory();
	key_inverse = allocate_memory();
	plaintext_fn = malloc(FILENAME_MAX);
	ciphertext_fn = malloc(FILENAME_MAX);
	//plaintext = malloc(FILENAME_MAX * sizeof(int));
	//ciphertext = malloc(FILENAME_MAX * sizeof(int));
	while(1)
	{
		printf("\n|+|+|+| HILL CRYPTANALYSIS |+|+|+|\n");
		printf("1. Cryptanalysis\n");
		printf("2. Exit\n");
		printf("Choose an option: ");
		scanf(" %d", &option);
		getchar();
		switch(option)
		{
			case 1:
				//Opening files for plaintext and ciphertext
				open_file("Enter the file name with the plaintext (it may be an absolute path): ", &plaintext_fn, &plaintext_fp);
				open_file("Enter the file name with the ciphertext (it may be an absolute path): ", &ciphertext_fn, &ciphertext_fp);

				low = 0;
				no_attempts = 0;
				valid_determinant = false;
				while(!valid_determinant)
				{
					if(no_attempts != 0)
						low += 9;
					
					printf("\n-/-/-/-/-/-/-/-/-/ %d ATTEMPT /-/-/-/-/-/-/-/-/\n", no_attempts + 1);
					//Moving offset in plaintext and ciphertext file					
					move_offset(&plaintext_fp, low);
					move_offset(&ciphertext_fp, low);

					//Reading a sample of 9 chars from plaintext and ciphertext file
					plaintext = read_sample(&plaintext_fp, 9L);	
					ciphertext = read_sample(&ciphertext_fp, 9L);	

					printf("Plaintext sample: ");
					for(i = 0; i < 9; i++)
						printf("%c ", plaintext[i] + 32);
					printf("\n");

					printf("Plaintext sample matrix X ---->  ");
					for(i = 0; i < 9; i++)
					{
						if(i % 3 == 0)
							printf("\n");
						printf("%d ", plaintext[i]);
					}
					printf("\n");

					printf("Ciphertext sample: ");
					for(i = 0; i < 9; i++)
						printf("%c ", ciphertext[i] + 32);
					printf("\n");

					// ---------------------------------------------- CALCULATING KEY ------------------------------------
					//Storing plaintext and ciphertext into arrays
					k = 0;
					for(i = 0; i < 3; i++)
						for(j = 0; j < 3; j++)
						{
							x[i][j] = plaintext[k];
							y[i][j] = ciphertext[k++];
						}

					printf("Finding inverse of Matrix X...\n");			
					//Finding x inverse
					valid_determinant = true;
					x_inverse = find_inverse_matrix(x, &valid_determinant);

					if(!valid_determinant)
					{
						printf("|+|Trying again...\n\n");
						no_attempts++;
					}
					else
					{
						printf("Finding K by doing  ->  X^(-1)Y = K\n");
						for(i = 0; i < 3; i++)
							for(j = 0; j < 3; j++)
								key[i][j] = 0;

						for(i = 0; i < 3; i++)
							for(j = 0; j < 3; j++)
							{
								for(k = 0; k < 3; k++)
								{
									key[i][j] += x_inverse[i][k] * y[k][j];
								}
								key[i][j] = key[i][j] % 95;
							}

						printf("Proposed key: \n");
						for(i = 0; i < 3; i++)
						{
							for(j = 0; j < 3; j++)
								printf("%d ", key[i][j]);
							printf("\n");
						}

						// -------------------------------------------- CALCULATING KEY INVERSE--------------------------
						printf("Finding inverse of Matrix K...\n");			
						valid_determinant = true;
						key_inverse = find_inverse_matrix(key, &valid_determinant);
						
						if(!valid_determinant)
						{
							printf("|+|Trying again...\n\n");
							no_attempts++;
						}
						else
						{
							// ------------------------------------------------- DECIPHERING-------------------------------------------
							if((ciphertext_fp2 = fopen(ciphertext_fn, "rb")) == NULL)
								printf("|+|ERROR: Can't open: %s. Try again.\n", ciphertext_fn);
							//Creating new file to write plaintext
							write_fp = fopen(deciphered_fn, "w");
							//DECIPHERING
							i = 0;
							printf("Deciphering...\n");
							while((ch = getc(ciphertext_fp2)) != EOF)
							{
								if(ch >= 32 && ch < 127)
								{
									msg[i++] = ch - 32;	
									if(i > 2)
									{
										i = 0;
										for(j = 0; j < 3; j++)
											putc(((msg[0] * key_inverse[0][j] + msg[1] * key_inverse[1][j] + msg[2] * key_inverse[2][j]) % 95) + 32, write_fp);
									}
								}
								else
									putc(ch, write_fp);
							}
							printf("Deciphered\n");
							fclose(write_fp);
							fclose(ciphertext_fp2);

							//Testing
							if((ciphertext_fp2 = fopen(deciphered_fn, "rb")) == NULL)
								printf("|+|ERROR: Can't open: %s. Try again.\n", deciphered_fn);

							if((plaintext_fp2 = fopen(plaintext_fn, "rb")) == NULL)
								printf("|+|ERROR: Can't open: %s. Try again.\n", plaintext_fn);

							valid_determinant = true;
							i = 0;
							while(i < 9)
							{
								decipheredtext[i] = getc(ciphertext_fp2);
								plaintext_test[i] = getc(plaintext_fp2);
								printf("%c | %c \n", decipheredtext[i], plaintext_test[i]);
								if(decipheredtext[i] != plaintext_test[i])
								{
									printf("|+|ERROR: Key found is not the right one. Trying again...\n\n");
									//exit(EXIT_FAILURE);
									no_attempts++;
									valid_determinant = false;
									break;
								}
								else
									i++;
							}
							fclose(ciphertext_fp2);
							fclose(plaintext_fp2);
						}
					}
				}
				while(!valid_determinant);
				
				fclose(plaintext_fp);	
				fclose(ciphertext_fp);	
				//Creating new file to write key
				write_fp = fopen(key_fn, "w");
					
				printf("Key: \n");
				for(i = 0; i < 3; i++)
				{
					for(j = 0; j < 3; j++)
					{
						printf("%d ", key[i][j]);
						fprintf(write_fp, "%d ", key[i][j]);
					}
					printf("\n");
					fprintf(write_fp, "\n");
				}
				fclose(write_fp);
			break;
			case 2:
				exit(EXIT_SUCCESS);
			break;
			default:
				printf("Choose a correct option.\n\n");
			break;
		}
	}
	return 0;
}

int ** find_inverse_matrix(int **x, bool *valid_determinant)
{
	int **matrix, i, j, k, m, n, l;
	int aux, minor[2][2], **matrix_minors;
	int det_inverse;
	long determinant; 

	printf("\n-----CALL: find_inverse_matrix()-----\n");

	matrix = malloc(3 * sizeof(int *));
	matrix_minors = malloc(3 * sizeof(int *));
	for(i = 0; i < 3; i++)
	{
		matrix[i] = malloc(3 * sizeof(int));
		matrix_minors[i] = malloc(3 * sizeof(int));
	}

	for(i = 0; i < 3; i++)
		for(j = 0; j < 3; j++)
			matrix[i][j] = x[i][j];

	determinant = (matrix[0][0] * matrix[1][1] * matrix[2][2]) + (x[0][1] * x[1][2] * x[2][0]) + (x[0][2] * x[1][0] * x[2][1]) - (x[2][0] * x[1][1] * x[0][2]) - (x[2][1] * x[1][2] * x[0][0]) - (x[2][2] * x[1][0] * x[0][1]);
	if(determinant < 0)
		determinant = 95 - (-determinant % 95);
	else
		determinant = determinant % 95;
	printf("Step One: Determinant = %ld\n", determinant);
	printf("Step Two: Validating determinant gcd(%ld, 95) = 1\n", determinant);
	*valid_determinant = validate_euclid_algorithm(determinant);
	if(!(*valid_determinant))
	{
		printf("-----RETURN ON ERROR: find_inverse_matrix()-----\n");
		return matrix_minors;
	}
	printf("Step Three: Finding inverse of determinant using Extended Euclid Algorithm\n");
	det_inverse = find_det_inverse(determinant);
	printf("Determinant Inverse: %d\n", det_inverse);
	printf("Step Four: Transpose Matrix\n");
	aux = matrix[0][1];
	matrix[0][1] = matrix[1][0];
	matrix[1][0] = aux;

	aux = matrix[0][2];
	matrix[0][2] = matrix[2][0];
	matrix[2][0] = aux;

	aux = matrix[1][2];
	matrix[1][2] = matrix[2][1];
	matrix[2][1] = aux;

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
					n = 0;
					for(l = 0; l < 3; l++)
						if(l != j)
						{
							minor[m][n++] = matrix[k][l];
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
			matrix_minors[i][j] = (matrix_minors[i][j] * det_inverse) % 95;		

	printf("Inverse of the matrix: \n");	
	for(i = 0; i < 3; i++)
	{
		for(j = 0; j < 3; j++)
			printf("%d ", matrix_minors[i][j]);
		printf("\n");
	}

	printf("-----RETURN ON SUCCESS: find_inverse_matrix()-----\n");
	return matrix_minors;
}

bool validate_euclid_algorithm(int number)
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
		return false;
	}
	printf("-----RETURN ON SUCCESS: validate_euclid_algorithm()-----\n");
	return true;
}

int find_det_inverse(int number)
{
	int n = 95, index = 0, remainder = 0;
	int array[100][4], array1[100][2], factor1, factor2;
	int i, j, product1, product2;
	int number_inverse;

	printf("\n-----CALL: find_det_inverse()-----\n");
	n = 95;
	index = 0;
	while(1) 
	{
		remainder = n % number;
		if(remainder == 0)
			break;

		array[index][0] = remainder;
		array[index][1] = n;
		array[index][2] = number;
		array[index][3] = - (n - remainder) / number; 

		n = number;
		number = remainder;
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
	number_inverse = array1[index - 1][1];
	if(number_inverse < 0)
		number_inverse = 95 - (-number_inverse % 95);

	printf("-----RETURN ON SUCCESS: find_det_inverse()-----\n");
	return number_inverse;
}

int ** allocate_memory()
{
	int **matrix;
	matrix = malloc(3 * sizeof(int *));
	for(int i = 0; i < 3; i++)
		matrix[i] = malloc(3 * sizeof(int));
	return matrix;
}

void open_file(const char * prompt, char **file_name, FILE **read_fp)
{
	bool input_correct = false;
	while(!input_correct)
	{
		printf("%s", prompt);
		scanf("%s", *file_name);
		if((*read_fp = fopen(*file_name, "rb")) == NULL)
			printf("|+|ERROR: Can't open: %s. Try again.\n", *file_name);
		else
			input_correct = true;
	}
	return;
}

void move_offset(FILE **file_pointer, long offset)
{
	long i = 0;
	char ch;

	fseek(*file_pointer, 0L, SEEK_SET); 		//Moves to the beggining of the file
	while(i < offset)
	{
		ch = getc(*file_pointer);
		if(ch == EOF)
		{
			printf("|+|ERROR: File consumed.\n");
			exit(EXIT_FAILURE);
		}
		if(ch != '\n' && ch != '\r')
			i++;
	}
	return;	
}

int * read_sample(FILE **file_pointer, long sample_len)
{
	long i = 0;
	char ch;
	int *text = malloc(FILENAME_MAX * sizeof(int));

	while(i < sample_len)
	{
		ch = getc(*file_pointer);
		if(ch == EOF)
		{
			printf("|+|ERROR: File consumed at read.\n");
			exit(EXIT_FAILURE);
		}
		if(ch != '\n' && ch != '\r')
			text[i++] = ch - 32;
	}
	text[i] = '\0';
	return text;
}