#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

int main(void)
{
    int i, j, k, l;
    int index_results = 0, index_array = 0, p = 0;
    long array[1000], results[1000];
    bool isGenerator = false;

    printf("Insert p: ");
    scanf("%d", &p);
    printf("P: %d\n", p);

    for(i = 2; i < p; i++)
    {
        index_array = 0;
        for(j = 0; j < p; j++)
        {
            printf("%ld\n", ((long) pow((double) i, (double) j)));
            printf("%ld\n", ((long) pow((double) i, (double) j)) % p);
            printf("P: %d\n", p);
            array[index_array] = ((long) pow((double) i, (double) j)) % p;
            // array[index_array] = array[index_array] % p;
            index_array++;
        }

        for(l = 0; l < index_array; l++)
            printf("%ld ", array[index_array]);
        printf("\n");    

        for(j = 1; j < p; j++)
        {
            isGenerator = false;
            for(l = 0; l < index_array; l++)
            {
                if(j == array[l++])
                {
                    isGenerator = true;
                    break;
                }
            }
            if(!isGenerator)
                break;
        }                
        
        if(isGenerator)
            results[index_results++] = i;
    }

    printf("Generators: ");
    for(i = 0; i < index_results; i++)
        printf("%d ", results[i]);

    return 0;
}