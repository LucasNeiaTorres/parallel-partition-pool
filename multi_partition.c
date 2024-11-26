#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include "verifica_particoes.h"

long long geraAleatorioLL()
{
    int a = rand(); // Returns a pseudo-random integer
                    //    between 0 and RAND_MAX.
    int b = rand(); // same as above
    long long v = (long long)a * 100 + b;
    return v;
}

void geraNaleatorios(long long v[], int n)
{
    for (int i = 0; i < n; i++)
    {
        v[i] = geraAleatorioLL();
    }
}

void print_array_long_long(long long *arr, int n)
{
    int i;
    for (i = 0; i < n; i++)
    {
        printf("%lld ", arr[i]);
    }
    printf("\n");
}

void print_array(int *arr, int n)
{
    int i;
    for (i = 0; i < n; i++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void multi_partition(long long *Input, int n, long long *P, int np, long long *Output, int *Pos)
{
    int i, j;
    int iOutput = 0;
    for (i = 0; i < np; i++)
    {
        for (j = 0; j < n; j++)
        {
            // printf("i: %d, j: %d, P[i]: %lld, Input[j]: %lld\n", i, j, P[i], Input[j]);
            if (Input[j] < P[i])
            {
                if ((i - 1 < 0) || (Input[j] >= P[i - 1]))
                {
                    Output[iOutput] = Input[j];
                    iOutput++;
                    if (i + 1 < np)
                        Pos[i + 1]++;
                }
            }
        }
        Pos[i + 1] += Pos[i];
    }

    for (i = 0; i < np; i++)
    {
        for (j = 0; j < n; j++)
        {

        }
    }
}

int compare(const void *a, const void *b)
{
    long long arg1 = *(const long long *)a;
    long long arg2 = *(const long long *)b;

    if (arg1 < arg2)
        return -1;
    if (arg1 > arg2)
        return 1;
    return 0;
}

int main()
{
    srand(time(NULL));
    int n = 14;
    int nP = 10;

    long long *input = malloc(n * sizeof(long long));
    if (input == NULL)
    {
        fprintf(stderr, "Erro ao alocar memória para input\n");
        return 1;
    }

    geraNaleatorios(input, n);

    long long *P = malloc(nP * sizeof(long long));
    if (P == NULL)
    {
        fprintf(stderr, "Erro ao alocar memória para P\n");
        return 1;
    }

    geraNaleatorios(P, nP);
    P[nP - 1] = LLONG_MAX;

    qsort(P, nP, sizeof(long long), compare);

    printf("Input: ");
    print_array_long_long(input, n);
    printf("P: ");
    print_array_long_long(P, nP);

    long long *output = malloc(n * sizeof(long long));
    if (output == NULL)
    {
        fprintf(stderr, "Erro ao alocar memória para output\n");
        return 1;
    }

    int *pos = (int *)calloc(nP, sizeof(int));
    if (output == NULL)
    {
        fprintf(stderr, "Erro ao alocar memória para pos\n");
        return 1;
    }

    multi_partition(input, n, P, nP, output, pos);

    printf("Output: ");
    print_array_long_long(output, n);
    printf("Pos: ");
    print_array(pos, nP);

    verifica_particoes(input, n, P, nP, output, pos);

    free(output);
    free(pos);
    free(input);
    free(P);
    return 0;
}