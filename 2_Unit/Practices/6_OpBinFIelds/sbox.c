//@Authors: Jaime Bastida and Daniel Solorio
//@Date: 2019-04-01
//@Abstract: This program does the implementation of operations in the binary field to construct an S-Box from an m(x) and entries given. 
//@Base Example: 
//	Take m = 101 i.e. m(x) = x^8 + 1
// 	Take a = 25 -> a^(-1) = 4D i.e. a^-1(x) = x^6 + x^3 + x^2 + 1. Important take a^(-1) not a
//  Take b = 1F i.e. b(x) = x^4 + x^3 + x^2 + x + 1
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#define MAX_DEGREE_7 128
#define MAX_DEGREE_15 32768
#define MAX_DEGREE_23 8388608
#define MAX_DEGREE_31 2147483648

typedef unsigned char BYTE;

void construct_sbox(unsigned int b, unsigned int m, unsigned int c, const char *sbox_table_fn);
char * parse_to_polynomial(unsigned int number);
unsigned int mult_gf2_n(unsigned int a, unsigned int b, unsigned int m, unsigned short m_degree);
unsigned int mult_gf2_n_monomial(unsigned int a, unsigned int b, unsigned int m, unsigned short m_degree);
unsigned int find_inverse_in_table(unsigned int a, const char *mult_inv_table);
unsigned short find_m_degree(unsigned int m);

int main()
{
	short opcode = 0;
	unsigned int m = 0, a = 0, b = 0;
	unsigned int c = 0;
	const char *sbox_table_fn = "SBox.txt";

	while(true)
	{
		printf("|+|+|+| OPERATIONS IN BINARY FIELDS |+|+|+|\n");
		printf("1. Generate S-Box\n");
		printf("2. Exit\n");
		printf("Choose an option: ");
		scanf("%d", &opcode);
		getchar();
		switch(opcode)
		{
			case 1:
				printf("Write m(x): ");
				scanf("%x", &m);
				printf("Write b: ");
				scanf("%x", &b);
				printf("Write c: ");
				scanf("%x", &c);

				construct_sbox(b, m, c, sbox_table_fn);		
			break;
			case 2:
				exit(EXIT_SUCCESS);
			break;
			default:
			printf("Choose a correct option.\n\n");
		}
	}
	return 0;
}

void construct_sbox(unsigned int b, unsigned int m, unsigned int c, const char *sbox_table_fn)
{
	unsigned int a, a_inverse, result;
	unsigned short m_degree;
	const char *mult_inv_table_fn = "MultInverseTable.txt";
	const char *sbox_polynomial_table_fn = "SBoxPolynomial.txt";
	char *polynomial;
	FILE *write_fp1, *write_fp2;

	write_fp1 = fopen(sbox_table_fn, "w");
	write_fp2 = fopen(sbox_polynomial_table_fn, "w");
	m_degree = find_m_degree(m);
	short jump = 0;
	for(a = 0; a < 256; a++)
	{
		if(jump == 16)
		{
			fprintf(write_fp1, "\n");
			fprintf(write_fp2, "\n");
			jump = 0;
		}
		a_inverse = find_inverse_in_table(a, mult_inv_table_fn);
		result = mult_gf2_n(a_inverse, b, m, m_degree);			//Result of multiply a(x) * b(x) mod m(x) this is not the monomial function
		result = result ^ c;
		fprintf(write_fp1 ,"%.2X ", result);
		polynomial = parse_to_polynomial(result);
		fprintf(write_fp2, "%s\n", polynomial);
		jump++;
	}
	fclose(write_fp1);
	fclose(write_fp2);

	return ;
}

char * parse_to_polynomial(unsigned int number)
{
	char *polynomial;
	polynomial = calloc(FILENAME_MAX, sizeof(char));
	unsigned int i;
	
	for(i = MAX_DEGREE_15; i > 0; i = i >> 1)
	{
		if((i & number) == 128)
			sprintf(polynomial + strlen(polynomial), "x^7 + ");
		else if((i & number) == 64)
			sprintf(polynomial + strlen(polynomial), "x^6 + ");
		else if((i & number) == 32)
			sprintf(polynomial + strlen(polynomial), "x^5 + ");
		else if((i & number) == 16)
			sprintf(polynomial + strlen(polynomial), "x^4 + ");
		else if((i & number) == 8)
			sprintf(polynomial + strlen(polynomial), "x^3 + ");
		else if((i & number) == 4)
			sprintf(polynomial + strlen(polynomial), "x^2 + ");
		else if((i & number) == 2)
			sprintf(polynomial + strlen(polynomial), "x^1 + ");
		else if((i & number) == 1)
			sprintf(polynomial + strlen(polynomial), "1");
	}
	//TRIMMING 
	i = 0;
	while(polynomial[i++] != '\0')
		;

	i = i - 2;
	if(polynomial[i] == '1')
		return polynomial;
	else
	{
		i = i - 2;
		polynomial[i] = '\0';
		return polynomial;
	}
}

unsigned int mult_gf2_n(unsigned int a, unsigned int b, unsigned int m, unsigned short m_degree)
{
	unsigned int aux_b = 0, result = 0, i;
	//DOING a * b mod m(x)
	//Through this loop we go over every monomial in "a" e.g. a(x) = x^6 + x^3 + x^2 + 1 and b(x) = x^4 + x^3 + x^2 + x^1 + 1
	//In the first iteration we are going to pass to the function a = x^6 multiplied by b(x)
	//The result of that is stored into aux_b and xored with the resul, and so on.
	for(i = MAX_DEGREE_15; i > 0; i = i >> 1)
	{
		if((i & a) != 0) 
		{
			aux_b = mult_gf2_n_monomial(i, b, m, m_degree);
			result = result ^ aux_b;
		}
	}
	
	return result;	
}

unsigned int find_inverse_in_table(unsigned int a, const char *mult_inv_table_fn)
{
	FILE *read_fp;
	unsigned int i, a_inverse;
	char ch;

	//Reading Multiplicative Inverse Table in GF(2^8)
	if((read_fp = fopen(mult_inv_table_fn, "rb")) == NULL)
	{
		printf("|+|ERROR: Can't open: %s. Try again.\n", mult_inv_table_fn);
		exit(EXIT_FAILURE);
	}
	//Skipping lines i.e. moving over X
	for(i = 0; i < (a >> 4); i++)
	{
		while((ch = getc(read_fp)) != '\n')
			;
	}
	//Moving over Y
	for(i = 0; i < (a & 0x0F); i++)
		fscanf(read_fp, "%x", &a_inverse);

	fscanf(read_fp, "%x", &a_inverse);
	fclose(read_fp);

	return a_inverse;
}

unsigned short find_m_degree(unsigned int m)
{
	for(unsigned int i = MAX_DEGREE_15; i > 0; i = i >> 1)
		if((i & m) != 0)
			return (unsigned short) (log((double) i) / log(2)); //log_2(x) = log_y(x) / log_y(2) where y can be any value, e.g. Euler or 10
}

unsigned int mult_gf2_n_monomial(unsigned int a, unsigned int b, unsigned int m, unsigned short m_degree)
{
	unsigned int i, aux;

	for(i = MAX_DEGREE_15; i > 0; i = i >> 1)
	{
		if((i & a) == 1) // 1 * b(x) = b(x)
			return b;
		else if((i & a) == 2) // x * b(x) = b(x) << 1
		{ 
			aux = 1;
			if(((aux << (m_degree - 1)) & b) != 0)
			{
				b = b << 1;
				b = b ^ m;
			}
			else
				b = b << 1;
			return b;
		}
		else if((i & a) != 0)//x^2 or greater, call recursive
		{
			b = mult_gf2_n_monomial(a >> 1, b, m, m_degree);
			aux = 1;
			if(((aux << (m_degree - 1)) & b) != 0)
			{
				b = b << 1;
				b = b ^ m;
			}
			else
				b = b << 1;
			return b;
		}	
	}
}