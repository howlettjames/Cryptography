#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
	
int main(void)
{
	int option = 0, index = 0, k_len = 0;
	char key[96], file_name[FILENAME_MAX], ch;
	FILE *write_fp, *read_fp;

	while(1)
	{
		printf("\n|+|+|+| KEYWORD |+|+|+|\n");
		printf("1. Cipher\n");
		printf("2. Decipher\n");
		printf("3. Exit\n");
		printf("Choose an option: ");
		scanf(" %d", &option);
		getchar();
		switch(option)
		{
			case 1:
				printf("Enter the key word: ");	
				k_len = 0;
				while((ch = getc(stdin)) != '\n')
					key[k_len++] = ch - 32;

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
				
				//CIPHERING and writing down to the new file
				index = 0;
				while((ch = getc(read_fp)) != EOF)
				{
					if(ch > 31 && ch < 128)
					{
						if(index >= k_len)
							index = 0;
						putc((((ch - 32) + key[index++]) % 96) + 32, write_fp);
					}
					else if(ch != '\n' && ch != '\t')
						putc(ch, write_fp);
				}
				fclose(write_fp);
				fclose(read_fp);
			break;
			case 2:
				printf("Enter the key word: ");	
				k_len = 0; 
				while((ch = getchar()) != '\n')
					key[k_len++] = ch - 32;
				
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
				index = 0;
				while((ch = getc(read_fp)) != EOF)
				{
					if(ch > 31 && ch < 128)
					{
						if(index >= k_len)
							index = 0;
						if((ch - 32) < key[index])
							putc((96 - (-(ch - 32 - key[index]) % 96)) + 32, write_fp);
						else
							putc(((ch - 32 - key[index]) % 96) + 32, write_fp);	
						index++;
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