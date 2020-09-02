#include <stdio.h>
#include <windows.h>
#include <string.h>

#define MSG_LENGHT 200
	
int main(void)
{
	short option = 0, key = 0, index = 0, value = 0;
	char ch;
	char message[MSG_LENGHT], ciphertext[MSG_LENGHT];

	while(1)
	{
		printf("|+|+|+| CAESAR CIPHER |+|+|+|\n");
		printf("1. Cipher\n");
		printf("2. Decipher\n");
		printf("3. Serial Decipher\n");
		printf("4. Exit\n");
		printf("Choose an option: ");
		scanf(" %d", &option);

		switch(option)
		{
			case 1:
				printf("This program works using the Enlgish alphabet. That is why the key needs to be in the range between 0 and 25.\n");
				printf("Enter the key you want to use: ");
				scanf("%d", &key);
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
						message[index] = (((message[index] - 'A') + key) % 26) + 'A';
					index++;
				}
				printf("The cipher text is: %s\n", message);
			break;
			case 2:
				printf("This program works using the English alphabet. That is why the key needs to be in the range between 0 and 25.\n");
				printf("Enter the key you want to use: ");
				scanf("%d", &key);
				printf("Enter the message (cipher text) using capital letters: ");
				getchar();
				index = 0;
				while((ch = getchar()) != '\n')
					message[index++] = ch;
				message[index] = '\0';

				//DECIPHERING	
				index = 0;
				while(message[index] != '\0')
				{
					if(!(message[index] == ' '))
					{
						if((message[index] - 'A') < key)
							message[index] = (26 + (message[index] - 'A' - key)) + 'A';	
						else
							message[index] = (((message[index] - 'A') - key) % 26) + 'A';	
					}
					index++;
				}
				printf("The plain text is: %s\n\n", message);
			break;
			case 3:
				printf("\nThis modality works trying every single key using the English alphabet, that is a value between 0 - 25.\n");
				printf("Enter the message you want to decipher (in capital letters) and I will try a key starting from k = 0: ");
				getchar();
				index = 0;
				while((ch = getchar()) != '\n')
					message[index++] = ch;
				message[index] = '\0';

				key = 0;
				option = 1;
				while(option && key < 26)
				{
					printf("\n\n");
					strcpy(ciphertext, message);
					//DECIPHERING	
					index = 0;
					while(ciphertext[index] != '\0')
					{
						if(!(ciphertext[index] == ' '))
						{
							if((ciphertext[index] - 'A') < key)
								ciphertext[index] = (26 + (ciphertext[index] - 'A' - key)) + 'A';	
							else
								ciphertext[index] = (((ciphertext[index] - 'A') - key) % 26) + 'A';	
						}
						index++;
					}
					printf("The plain text using key = %d is: %s\n", key, ciphertext);
					printf("You want to continue? Yes == 1 / No == 0 : ");	
					scanf(" %d", &option);
					key++;
				}
			break;
			case 4:
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