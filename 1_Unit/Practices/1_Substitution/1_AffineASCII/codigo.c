#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
	
int main(void)
{
	int option = 0, a = 0, b = 0, index = 0, value = 0, remainder = 0, n = 0, i = 0, factor1 = 0, factor2 = 0, product1 = 0, product2 = 0;
	int array[100][4], array1[100][2], aInverse = 0, j = 0, previousRemainder = 0, aux = 0;
	char *message, ch, file_name[FILENAME_MAX];
	FILE *write_fp, *read_fp;

	while(1)
	{
		printf("\n|+|+|+| AFFINE CIPHER |+|+|+|\n");
		printf("1. Cipher\n");
		printf("2. Decipher\n");
		printf("3. Exit\n");
		printf("Choose an option: ");
		scanf(" %d", &option);

		switch(option)
		{
			case 1:
				tryagaina:
				printf("This program works using the ASCII alphabet. It uses two values that form a key: \"a\" and \"b\".\nThe \"a\" value needs to be in the set Zn* where n = 96.\nThe \"b\" value needs to be a value between 0-95.\n");
				printf("Enter the key you want to use i.e. <a> <b>: ");
				scanf("%d %d", &a, &b);
				//Validating "a" using the Euclid algorithm
				n = 96;
				remainder = 0;
				aux = a;
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
					printf("|+|+|+|ERROR: Value for \"a\" not in the set Zn* where n = 96. Try again.\n\n\n");
					goto tryagaina;
				}

				tryagainfile:
				printf("|+|Enter the file name (it may be an absolute path): ");
				scanf("%s", file_name);
				if((read_fp = fopen(file_name, "rb")) == NULL)
				{
					printf("|+|+|+|ERROR: Can't open: %s. Try again.\n", file_name);
					goto tryagainfile;
				}

				//Appending afn to the file_name
				index = 0;
				while(file_name[index++] != '.')
					;
				index--;
				file_name[index++] = 'a';
				file_name[index++] = 'f';
				file_name[index++] = 'n';
				file_name[index++] = '.';
				file_name[index++] = 't';
				file_name[index++] = 'x';
				file_name[index++] = 't';
				file_name[index++] = '\0';
				//Creating new file
				write_fp = fopen(file_name, "w");
				//Ciphering and writing down to the new file
				while((ch = getc(read_fp)) != EOF)
				{
					if(ch > 31 && ch < 128)
						putc((((a * (ch - 32)) + b) % 96) + 32, write_fp);
					else
						putc(ch, write_fp);
				}	
				fclose(write_fp);
				fclose(read_fp);
			break;
			case 2:
				tryagaina1:
				printf("Enter the key you want to use i.e. <a> <b>: ");
				scanf("%d %d", &a, &b);
				//Validating "a" using the Euclid algorithm
				n = 96;
				remainder = 0;
				aux = a;
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
					printf("|+|+|+|ERROR: Value for \"a\" not valid. Try again.\n\n\n");
					goto tryagaina1;
				}
				//Calculating inverse of a using the Extended Euclid Algorithm
				n = 96;
				index = 0;
				while(1) 
				{
					remainder = n % a;
					if(remainder == 0)
						break;

					array[index][0] = remainder;
					array[index][1] = n;
					array[index][2] = a;
					array[index][3] = - (n - remainder) / a; 

					n = a;
					a = remainder;
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
				aInverse = array1[index - 1][1];
				if(aInverse < 0)
					aInverse = 96 - (-aInverse % 96);
				//End of Extended Euclid algorithm
				tryagainfile1:
				printf("|+|Enter the file name (it may be an absolute path): ");
				scanf("%s", file_name);
				if((read_fp = fopen(file_name, "rb")) == NULL)
				{
					printf("|+|+|+|ERROR: Can't open: %s. Try again.\n", file_name);
					goto tryagainfile1;
				}
				//Creating new file
				write_fp = fopen("dechipered.txt", "w");

				//Ciphering and writing down to the new file
				while((ch = getc(read_fp)) != EOF)
				{
					if(ch > 31 && ch < 128)
					{
						if((ch - 32) < b)
							putc((96 - (-(aInverse * (ch - 32 - b)) % 96)) + 32, write_fp);	
						else
							putc(((aInverse * (ch - 32 - b)) % 96) + 32, write_fp);	
					}
					else
						putc(ch, write_fp);
				}
				fclose(write_fp);
				fclose(read_fp);
			break;
			case 3:
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