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
    for(i = 0; i < np; i++) {
        for(j = 0; j < n; j++) {
            if(Input[j] < P[i]) {
                if((i-1 < 0) || (Input[j] >= P[i-1])) {
                    Output[Pos[i]] = Input[j];
                    Pos[i]++;
                }
            }
        }
    }
    printf("Output: ");
    print_array_long_long(Output, n);
    printf("Pos: ");
    print_array(Pos, np);
}

int main()
{
    srand(time(NULL));
    int n = 10;
    long long input[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    long long P[20] = {3, 5, 6, LLONG_MAX};
    int nP = 4;

    long long output[10];
    int pos[20] = {0};

    multi_partition(input, n, P, nP, output, pos);

    return 0;
}