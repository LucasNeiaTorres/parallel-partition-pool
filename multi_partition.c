// lógica "melhor"
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
ThreadData *taskQueue[MAX_TASKS];
int taskIndex = 0;
int tasksInQueue = 0;
int completedTasks = 0;

pthread_mutex_t queueMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t completeMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queueCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t completeCond = PTHREAD_COND_INITIALIZER;

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
        if (input[m] < x)
            left = m + 1;
        else
            right = m;
    }
    return left;
}

void single_partition(ThreadData *data) {
    for(int i = data->inicio; i < data->fim; i++) {
        int posL = bsearch_lower_bound(P, 0, nP, Input[i]);
        adiciona_array(data, posL, Input[i]);
    }
} 

// Adiciona uma tarefa ao pool de threads
void add_task(ThreadData *input) {
    pthread_mutex_lock(&queueMutex);

    taskQueue[(taskIndex + tasksInQueue) % MAX_TASKS] = input;
    tasksInQueue++;

    pthread_cond_signal(&queueCond);
    pthread_mutex_unlock(&queueMutex);
}

void multi_partition(long long *Input, int n, long long *P, int np, long long *Output, int *Pos) {
    ThreadData threadData[numThreads];
    int chunkSize = (n + numThreads - 1) / numThreads;
    completedTasks = 0;

    for (int i = 0; i < numThreads; i++) {
        threadData[i].inicio = i * chunkSize;
        int fim = (i + 1) * chunkSize;
        threadData[i].fim = (fim > n) ? n : fim;
        threadData[i].tamOutput = (int *)calloc(np, sizeof(int));
        threadData[i].capacidadeOutput = (int *)calloc(np, sizeof(int));
        threadData[i].tempOutput = (long long **)malloc(np * sizeof(long long *));
        if (threadData[i].tamOutput == NULL || threadData[i].capacidadeOutput == NULL || threadData[i].tempOutput == NULL) {
            fprintf(stderr, "Memory allocation failed for threadData[%d]\n", i);
            exit(EXIT_FAILURE);
        }
        for (int j = 0; j < np; j++) {
            threadData[i].capacidadeOutput[j] = n / numThreads;
            threadData[i].tempOutput[j] = (long long *) malloc(threadData[i].capacidadeOutput[j] * sizeof(long long));
            if (threadData[i].tempOutput[j] == NULL) {
                fprintf(stderr, "Memory allocation failed for threadData[%d].tempOutput[%d]\n", i, j);
                exit(EXIT_FAILURE);
            }
        }
        threadData[i].id = i;

        add_task(&threadData[i]); 
    }

    // Aguarda até que todas as tarefas estejam concluídas
    pthread_mutex_lock(&completeMutex);
    while (completedTasks < numThreads) {
        pthread_cond_wait(&completeCond, &completeMutex);
    }
    pthread_mutex_unlock(&completeMutex);
    int iOutput = 0;

    // concatena resultados
    for(int i = 0; i < np; i++) {
        for(int j = 0; j < numThreads; j++) {
            for(int k = 0; k < threadData[j].tamOutput[i]; k++) {
                Output[iOutput] = threadData[j].tempOutput[i][k];
                iOutput++;
            }
            free(threadData[j].tempOutput[i]);
            if(i + 1 < np)
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

// Função de trabalho da thread
void *thread_worker(void *arg) {
    while (1) {
        pthread_mutex_lock(&queueMutex);

        while (tasksInQueue == 0)
            pthread_cond_wait(&queueCond, &queueMutex);

        // Pega uma tarefa da fila
        ThreadData *task = taskQueue[taskIndex];
        taskIndex = (taskIndex + 1) % MAX_TASKS;
        tasksInQueue--;

        pthread_mutex_unlock(&queueMutex);

        single_partition(task); 

        // Marca a tarefa como concluída
        pthread_mutex_lock(&completeMutex);
        completedTasks++;
        if (completedTasks == numThreads) {
            pthread_cond_signal(&completeCond);
        }
        pthread_mutex_unlock(&completeMutex);
    }
}

// Inicializa o pool de threads
void init_thread_pool()
{
    for (int i = 0; i < numThreads; i++)
    {
        if (pthread_create(&threads[i], NULL, thread_worker, NULL) != 0) {
            fprintf(stderr, "Erro ao criar thread %d\n", i);
        // } else {
            // printf("Thread %d criada\n", i);
        }
    }
}

// TODO: corrige veriicação particionamento: tudo 0 está dando correto

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

    // printf("Input: ");
    // print_array_long_long(Input, n);
    // printf("P: ");
    // print_array_long_long(P, nP);

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

    init_thread_pool(); 

    double timeSeconds = 0.0;
    chronometer_t time;
    chrono_reset(&time);
    chrono_start(&time);

    for (int i = 0; i < NTIMES; i++) {
        multi_partition(&inputG[i * n], n, &PG[i * nP], nP, output, pos);
    }

    chrono_stop(&time);
    timeSeconds += (double)chrono_gettotal(&time) / ((double)1000 * 1000 * 1000); // NanoSeconds para Seconds
    double tempoMedio = timeSeconds / NTIMES;
    printf("\ntotal_time_in_seconds: %lf s\n", tempoMedio);

    // printf("Output: ");
    // print_array_long_long(output, n);
    // printf("Pos: ");
    // print_array(pos, nP);

    verifica_particoes(Input, n, P, nP, output, pos);

    free(output);
    free(pos);
    free(Input);
    free(P);
    free(inputG);
    free(PG);
    return 0;
}