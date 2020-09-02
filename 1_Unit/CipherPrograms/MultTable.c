#include <stdio.h>
#include <windows.h>
#include <stdbool.h>

#define ARRAY_LENGTH 100

int main(void)
{
	int option = 0, n = 0, numberValues = 0;
	int x = 0, y = 0, z = 0, valuesSkip[ARRAY_LENGTH], xAxis[ARRAY_LENGTH], yAxis[ARRAY_LENGTH], xAxisLength = 0;
	bool skip = true;

	while(1)
	{
		printf("|+|+|+| MULTIPLICATION TABLE |+|+|+|\n");
		printf("1. Calculate a table.\n");
		printf("2. Exit\n");
		printf("Choose an option: ");
		scanf(" %d", &option);

		switch(option)
		{
			case 1:
				printf("This program displays the table Zn for any \"n\" given.\n");
				printf("Enter the n value: ");
				scanf(" %d", &n);
				printf("Enter how many values will be skipped: ");
				scanf(" %d", &numberValues);
				printf("Enter the values to be skipped: ");
				for(x = 0; x < numberValues; x++)
					scanf(" %d", &valuesSkip[x]);

				//Printing and removing values to be skipped, saving them into an array to avoid doing more calculations
				printf("\n *  | ");
				z = 0;
				xAxisLength = 0;
				for(x = 1; x < n; x++)
				{
					skip = false;
					for(z = 0; z < numberValues; z++)
					{
						if(valuesSkip[z] == x)
						{
							skip = true;
							break;
						}
						else if(x % valuesSkip[z] == 0)
						{
							skip = true;
							break;	
						}
					}		
					if(!skip)
					{
						printf("%3d  ", x);
						xAxis[xAxisLength++] = x;	
					}
				}
				printf("\n");
				for(x = 0; x < 2 * n; x++)
					printf("- ");

				//Doing the calculation part
				z = 0;
				for(x = 0; x < xAxisLength; x++)
				{
					printf("\n%3d | ", xAxis[x]);
					for(y = 0; y < xAxisLength; y++)
						printf("%3d  ", (xAxis[x] * xAxis[y]) % n);
				}
				printf("\n\n");
			break;
			case 2:
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