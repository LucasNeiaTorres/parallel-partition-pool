#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <limits.h>

void print_array_long_long(long long *arr, int n)
{
    int i;
    for(i = 0; i < n; i++) {
        printf("%lld ", arr[i]);
    }
    printf("\n");
}  

void print_array(int *arr, int n)
{
    int i;
    for(i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void multi_partition(long long *Input, int n, long long *P, int np, long long *Output, int *Pos)
{
    int i, j;
    int iOutput = 0;
    for(i = 0; i < np; i++) {
        for(j = 0; j < n; j++) {
            // printf("i: %d, j: %d, P[i]: %lld, Input[j]: %lld\n", i, j, P[i], Input[j]);
            if(Input[j] < P[i]) {
                if((i-1 < 0) || (Input[j] >= P[i-1])) {
                    Output[iOutput] = Input[j];
                    iOutput++;
                    if(i+1 < np)  
                        Pos[i+1]++;
                }
            }
        }
        Pos[i+1] += Pos[i];
    }
}

int main()
{
    srand(time(NULL));
    int n = 14;
    // long long *input = malloc(n * sizeof(long long));
    // if (input == NULL) {
    //     fprintf(stderr, "Erro ao alocar memória para input\n");
    //     return 1;
    // }
    long long input[100] = {8, 4, 13, 7, 11, 100, 44, 3, 7, 7, 100, 110, 46, 44};

    int nP = 4;
    long long P[20] = {12, 70, 90, LLONG_MAX};

    long long *output = malloc(n * sizeof(long long));
    if (output == NULL) {
        fprintf(stderr, "Erro ao alocar memória para output\n");
        return 1;
    }

    int *pos = (int*) calloc(nP, sizeof(int));
    if (output == NULL) {
        fprintf(stderr, "Erro ao alocar memória para pos\n");
        return 1;
    }

    multi_partition(input, n, P, nP, output, pos);

    printf("Output: ");
    print_array_long_long(output, n);
    printf("Pos: ");
    print_array(pos, nP);

    free(output);
    free(pos);
    return 0;
}