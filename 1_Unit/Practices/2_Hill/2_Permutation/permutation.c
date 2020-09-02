#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

typedef unsigned char BYTE;

int main(void)
{
	BYTE option = 0;
	int i = 0, j = 0, k_len = 0, *key;
	char file_name[FILENAME_MAX], ch, *msg, *aux;
	FILE *read_fp, *write_fp;

	srand((unsigned) time(NULL));
	while(1)
	{
		printf("\n|+|+|+| PERMUTATION CIPHER |+|+|+|\n");
		printf("1. Encipher\n");
		printf("2. Decipher\n");
		printf("3. Exit\n");
		printf("Choose an option: ");
		scanf(" %d", &option);
		getchar();
		switch(option)
		{
			case 1:
				printf("Enter the length of the key: ");
				scanf("%d", &k_len);
				//Allocate memory for key array, msg and key array
				key = malloc(k_len * sizeof(int));
				msg = malloc(k_len);
				aux = malloc(k_len);
				printf("Enter the pairs of bijective positions: ");
				for(i = 0; i < k_len; i++)
					scanf(" %d", &key[i]);
				
				tryagainptfile:
				printf("|+|Enter the file name with the plaintext (it may be an absolute path): ");
				scanf("%s", file_name);
				if((read_fp = fopen(file_name, "rb")) == NULL)
				{
					printf("|+|+|+|ERROR: Can't open: %s. Try again.\n", file_name);
					goto tryagainptfile;
				}
				//Appending "perm" to what will be the new file with the ciphertext
				i = 0;
				while(file_name[i++] != '.')
					;
				i--;
				file_name[i++] = 'p';
				file_name[i++] = 'e';
				file_name[i++] = 'r';
				file_name[i++] = 'm';
				file_name[i++] = '.';
				file_name[i++] = 't';
				file_name[i++] = 'x';
				file_name[i++] = 't';
				file_name[i++] = '\0';
				//Creating new file to write ciphertext
				write_fp = fopen(file_name, "w");

				i = 0;
				while((ch = getc(read_fp)) != EOF)
				{
					if(ch > 31 && ch < 127)
					{
						msg[i++] = ch;
						if(i >= k_len)
						{
							for(i = 0; i < k_len; i++)
								aux[i] = msg[i];

							for(i = 0; i < k_len; i++)
								msg[i] = aux[key[i] - 1];

							for(i = 0; i < k_len; i++)
								putc(msg[i], write_fp);

							i = 0;
						}	
					}
					else
						putc(ch, write_fp);						
				}
				if(i < k_len)
				{
					for(j = i; j < k_len; j++)
						msg[j] = (rand() % 95) + 32;
					
					for(i = 0; i < k_len; i++)
						aux[i] = msg[i];

					for(i = 0; i < k_len; i++)
						msg[i] = aux[key[i] - 1];

					for(i = 0; i < k_len; i++)
						putc(msg[i], write_fp);					
				}
				fclose(write_fp);
				fclose(read_fp);
			break;
			case 2:
				printf("Enter the length of the key: ");
				scanf("%d", &k_len);
				//Allocate memory for key array, msg and key array
				key = malloc(k_len * sizeof(int));
				msg = malloc(k_len);
				aux = malloc(k_len);
				printf("Enter the pairs of bijective positions: ");
				for(i = 0; i < k_len; i++)
					scanf(" %d", &key[i]);
				
				//Obtaining inverse of the key
				for(i = 0; i < k_len; i++)
					aux[i] = key[i];

				for(i = 0; i < k_len; i++)
				{
					for(j = 0; j < k_len; j++)
					{
						if(i == aux[j] - 1)
							break;
					}
					key[i] = j + 1;
				}

				tryagainpermfile:
				printf("|+|Enter the file name with the ciphertext (it may be an absolute path): ");
				scanf("%s", file_name);
				if((read_fp = fopen(file_name, "rb")) == NULL)
				{
					printf("|+|+|+|ERROR: Can't open: %s. Try again.\n", file_name);
					goto tryagainpermfile;
				}
				//Creating new file to write plaintext
				write_fp = fopen("deciphered.txt", "w");

				i = 0;
				while((ch = getc(read_fp)) != EOF)
				{
					if(ch > 31 && ch < 127)
					{
						msg[i++] = ch;
						if(i >= k_len)
						{
							for(i = 0; i < k_len; i++)
								aux[i] = msg[i];

							for(i = 0; i < k_len; i++)
								msg[i] = aux[key[i] - 1];

							for(i = 0; i < k_len; i++)
								putc(msg[i], write_fp);

							i = 0;
						}	
					}
					else
						putc(ch, write_fp);						
				}
				if(i < k_len)
				{
					for(j = i; j < k_len; j++)
						msg[j] = (rand() % 95) + 32;
					
					for(i = 0; i < k_len; i++)
						aux[i] = msg[i];

					for(i = 0; i < k_len; i++)
						msg[i] = aux[key[i] - 1];

					for(i = 0; i < k_len; i++)
						putc(msg[i], write_fp);					
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