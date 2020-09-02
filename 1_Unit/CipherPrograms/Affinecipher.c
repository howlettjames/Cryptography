#include <stdio.h>
#include <windows.h>
#include <string.h>

#define MSG_LENGHT 200
	
int main(void)
{
	int option = 0, a = 0, b = 0, index = 0, value = 0, remainder = 0, n = 0, i = 0, factor1 = 0, factor2 = 0, product1 = 0, product2 = 0;
	int array[100][4], array1[100][2], aInverse = 0, j = 0;
	char ch;
	char message[MSG_LENGHT], ciphertext[MSG_LENGHT];

	while(1)
	{
		printf("|+|+|+| AFFINE CIPHER |+|+|+|\n");
		printf("1. Cipher\n");
		printf("2. Decipher\n");
		printf("3. Exit\n");
		printf("Choose an option: ");
		scanf(" %d", &option);

		switch(option)
		{
			case 1:
				printf("This program works using the English alphabet. It uses two values that form a key: \"a\" and \"b\".\nThe \"a\" value needs to be in the set Zn*. \nThe \"b\" value needs to be a value between 0-25.\n");
				printf("Enter the key you want to use i.e. <a> <b>: ");
				scanf("%d %d", &a, &b);
				printf("Enter the message (plain text) using capital letters: ");
				getchar();
				index = 0;
				while((ch = getchar()) != '\n')
					message[index++] = ch;
				message[index] = '\0';
		
				//CIPHERING	
				index = 0;
				while(message[index] != '\0')
				{
					if(message[index] != ' ')
						message[index] = (((a * (message[index] - 'A')) + b) % 26) + 'A';
					index++;
				}
				printf("The cipher text is: %s\n", message);
			break;
			case 2:
				printf("Enter the key you want to use i.e. <a> <b>: ");
				scanf("%d %d", &a, &b);
				printf("Enter the message (cipher text) using capital letters: ");
				getchar();
				index = 0;
				while((ch = getchar()) != '\n')
					message[index++] = ch;
				message[index] = '\0';

				n = 26;
				index = 0;
				//Calculating inverse of a
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
				/*
				for(i = 0; i < index; i++)
					printf("%d = %d + %d * %d\n", array[i][0], array[i][1], array[i][2], array[i][3]);
				printf("\n");
				*/
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
				/*
				for(i = 0; i < index; i++)
					printf("%d %d\n", array1[i][0], array1[i][1]);
				*/
				aInverse = array1[index - 1][1];
				if(aInverse < 0)
					aInverse = 26 - (-aInverse % 26);
				printf("El inverso es: %d\n", aInverse);
				//DECIPHERING	
				index = 0;
				while(message[index] != '\0')
				{
					if(!(message[index] == ' '))
					{
						if((message[index] - 'A') < b)
							message[index] = (26 - (-(aInverse * (message[index] - 'A' - b)) % 26)) + 'A';	
						else
							message[index] = ((aInverse * (message[index] - 'A' - b)) % 26) + 'A';	
					}
					index++;
				}
				printf("The plain text is: %s\n\n", message);
			break;
			case 3:
				system("cls");
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