#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define ORDER 32

typedef unsigned char BYTE;

int main(void)
{
	BYTE option = 0, s[ORDER], k[ORDER], key[2], i, j, aux;

	while(1)
	{
		printf("\n|+|+|+| RC4 CIPHER |+|+|+|\n");
		printf("1. Cipher\n");
		printf("2. Decipher\n");
		printf("3. Exit\n");
		printf("Choose an option: ");
		scanf(" %d", &option);

		switch(option)
		{
			case 1:
				key[0] = 5;
				key[1] = 6;
				printf("Key scheduling...\n");		
				for(i = 0; i < ORDER; i++)
					s[i] = i;

				for(i = 0, j = 0; i < ORDER; i++)
				{
					k[i] = key[j++];
					if(j == 2)
						j = 0;
				}

				j = 0;
				for(i = 0; i < ORDER; i++)
				{
					j = (j + s[i] + k[i]) % ORDER;
					//Swap
					aux = s[i];
					s[i] = s[j];
					s[j] = aux;
				}
				printf("Key scheduled\n");
				printf("RC4 algorithm\n");
				i = (i + 1) % ORDER;
				j = (j + s[i]) % ORDER;
				//Swap
				aux = s[i];
				s[i] = s[j];
				s[j] = aux;
				t = (s[i] + s[j]) % ORDER;
				k[i] = s[t];
			break;
			case 2:
				
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