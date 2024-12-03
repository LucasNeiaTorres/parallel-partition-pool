// lógica "melhor"
// Autores: Lucas Néia Torres, Inaiê Moscal Zambonin

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include "verifica_particoes.h"
#include "chrono.h"

#define MAX_THREADS 8
#define MAX_TASKS 1000
#define NTIMES 10

typedef struct
{
    int inicio;
    int fim;
    int *tamOutput;
    long long **tempOutput;
    int *capacidadeOutput;
    int id;
} ThreadData;

// long long **tempOutput;
long long *Input;
long long *P;
int n;
int nP;
int numThreads;
pthread_t threads[MAX_THREADS];
ThreadData threadData[MAX_THREADS];

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

void adiciona_array(ThreadData *data, int linha, long long valor) {
    if (data->tamOutput[linha] >= data->capacidadeOutput[linha]) {
        data->capacidadeOutput[linha] *= 2;
        data->tempOutput[linha] = (long long *)realloc(data->tempOutput[linha], data->capacidadeOutput[linha] * sizeof(long long));
    }
    data->tempOutput[linha][data->tamOutput[linha]++] = valor;
    // printf("Thread %d adicionando %lld na linha %d e coluna %d\n", data->id, valor, linha, data->tamOutput[linha] - 1);
}

int bsearch_lower_bound(long long *input, int left, int right, long long x)
{
    while (left < right)
    {
        int m = left + (right - left) / 2;
        if (input[m]-1 < x)
            left = m + 1;
        else
            right = m;
    }
    return left;
}


// Função de trabalho da thread
void *thread_worker(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    // printf("2 - Thread %d iniciando do início %d e fim %d \n", data->id, data->inicio, data->fim);
    for (int i = data->inicio; i < data->fim; i++) {
        int posL = bsearch_lower_bound(P, 0, nP, Input[i]);
        adiciona_array(data, posL, Input[i]);
    }
}

void multi_partition(long long *Input, int n, long long *P, int nP, long long *Output, int *Pos) {
    int chunkSize = (n + numThreads - 1) / numThreads;
    
    // Configuração de threads
    for (int i = 0; i < numThreads; i++) {
        threadData[i].inicio = i * chunkSize;
        int fim = (i + 1) * chunkSize;
        threadData[i].fim = (fim > n) ? n : fim;
        threadData[i].tamOutput = (int *)calloc(nP, sizeof(int));
        threadData[i].capacidadeOutput = (int *)calloc(nP, sizeof(int));
        threadData[i].tempOutput = (long long **)malloc(nP * sizeof(long long *));
        if (threadData[i].tamOutput == NULL || threadData[i].capacidadeOutput == NULL || threadData[i].tempOutput == NULL) {
            fprintf(stderr, "Memory allocation failed for threadData[%d]\n", i);
            exit(EXIT_FAILURE);
        }
        for (int j = 0; j < nP; j++) {
            threadData[i].capacidadeOutput[j] = 5000;
            threadData[i].tempOutput[j] = (long long *) malloc(threadData[i].capacidadeOutput[j] * sizeof(long long));
            if (threadData[i].tempOutput[j] == NULL) {
                fprintf(stderr, "Memory allocation failed for threadData[%d].tempOutput[%d]\n", i, j);
                exit(EXIT_FAILURE);
            }
        }
        threadData[i].id = i;
        pthread_create(&threads[i], NULL, thread_worker, &threadData[i]);
    }

    // Aguardando threads
    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    int iOutput = 0;

    // concatena resultados
    for(int i = 0; i < nP; i++) {
        for(int j = 0; j < numThreads; j++) {
            for(int k = 0; k < threadData[j].tamOutput[i]; k++) {
                Output[iOutput] = threadData[j].tempOutput[i][k];
                iOutput++;
            }
            free(threadData[j].tempOutput[i]);
            if(i + 1 < nP)
                Pos[i + 1] = Pos[i] + threadData[j].tamOutput[i];
        }
    }

    for (int i = 0; i < numThreads; i++) {
        free(threadData[i].tamOutput);
        free(threadData[i].capacidadeOutput);
        free(threadData[i].tempOutput);
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

int main(int argc, char *argv[])
{
    if (argc != 4) {
        printf("usage: %s <numElemInput> <numElemP> <nThreads>\n", argv[0]);
        return 0;
    } else if(atoi(argv[3]) > MAX_THREADS) {
        printf("Número de threads maior que o máximo permitido\n");
        return 0;
    } else {
        numThreads = atoi(argv[3]);
        nP = atoi(argv[2]);
        n = atoi(argv[1]);
    }
    srand(time(NULL));

    Input = malloc(n * sizeof(long long));
    if (Input == NULL)
    {
        fprintf(stderr, "Erro ao alocar memória para Input\n");
        return 1;
    }

    geraNaleatorios(Input, n);

    P = malloc(nP * sizeof(long long));
    if (P == NULL)
    {
        fprintf(stderr, "Erro ao alocar memória para P\n");
        free(Input);    
        return 1;
    }

    geraNaleatorios(P, nP);
    P[nP - 1] = LLONG_MAX;

    qsort(P, nP, sizeof(long long), compare);

    long long *output = malloc(n * sizeof(long long));
    if (output == NULL)
    {
        fprintf(stderr, "Erro ao alocar memória para output\n");
        free(Input);
        free(P);
        return 1;
    }

    int *pos = (int *)calloc(nP, sizeof(int));
    if (output == NULL)
    {
        fprintf(stderr, "Erro ao alocar memória para pos\n");
        free(Input);
        free(P);    
        free(output);   
        return 1;
    }

    long long *inputG = malloc(NTIMES * n * sizeof(long long));
    if (inputG == NULL) {
        fprintf(stderr, "Erro ao alocar memória para inputG\n");
        free(Input);
        free(P);    
        free(output); 
        free(pos);
        return 1;
    }

    long long *PG = malloc(NTIMES * nP * sizeof(long long));
    if (PG == NULL) {
        fprintf(stderr, "Erro ao alocar memória para PG\n");
        free(Input);
        free(P);    
        free(output); 
        free(pos);
        free(inputG);
        return 1;
    }

    for (int j = 0; j < NTIMES; j++) {
        memcpy(inputG + (j * n), Input, n * sizeof(long long));
        memcpy(PG + (j * nP), P, nP * sizeof(long long));
    }

    double timeSeconds = 0.0;
    chronometer_t time;
    chrono_reset(&time);
    chrono_start(&time);

    for (int i = 0; i < NTIMES; i++) {
        multi_partition(&inputG[i * n], n, &PG[i * nP], nP, output, pos);
    }

    chrono_stop(&time);
    timeSeconds += (double)chrono_gettotal(&time) / ((double)1000 * 1000 * 1000); // NanoSeconds para Seconds
    // double tempoMedio = timeSeconds / NTIMES;
    printf("\ntotal_time_in_seconds: %lf s\n", timeSeconds);

    // printf("Output: ");
    // print_array_long_long(output, n);
    // printf("Pos: ");
    // print_array(pos, nP);

    // verifica_particoes(Input, n, P, nP, output, pos);

    free(output);
    free(pos);
    free(Input);
    free(P);
    free(inputG);
    free(PG);
    return 0;
}