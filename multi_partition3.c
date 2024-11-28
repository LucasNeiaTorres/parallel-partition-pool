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
    long long maiorP;
    long long menorP;
    long long *Output;
    int tamOutput;
    int capacidadeOutput;
    int id;
} ThreadData;

long long *Input;
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

void multi_partition2(long long *Input, int n, long long *P, int np, long long *Output, int *Pos)
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

}

void adiciona_array(ThreadData *data, long long valor) {
    if (data->tamOutput >= data->capacidadeOutput) {
        data->capacidadeOutput *= 2;
        data->Output = (long long *)realloc(data->Output, data->capacidadeOutput * sizeof(long long));
    }
    data->Output[data->tamOutput++] = valor;
}

void single_partition(ThreadData *data) {
    for(int i = 0; i < n; i++) {
        // printf("Input[%d]: %lld maiorP: %lld menorP: %lld\n", i, Input[i], data->maiorP, data->menorP);
        if ((Input[i] < data->maiorP) && (Input[i] >= data->menorP)) {
            // printf("Thread %d adicionando %lld\n", data->id, Input[i]);
            adiciona_array(data, Input[i]);
        }
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
    ThreadData threadData[np];
    completedTasks = 0;

    for (int i = 0; i < np; i++) {
        if (i - 1 < 0) {
            threadData[i].menorP = 0;
        } else {
            threadData[i].menorP = P[i - 1];
        }
        threadData[i].maiorP = P[i];
        threadData[i].Output = (long long *)malloc(n * sizeof(long long));
        threadData[i].tamOutput = 0;
        threadData[i].capacidadeOutput = n;
        threadData[i].id = i;

        add_task(&threadData[i]);  // Passa o ponteiro para o objeto
    }

    // Aguarda até que todas as tarefas estejam concluídas
    pthread_mutex_lock(&completeMutex);
    while (completedTasks < np) {
        pthread_cond_wait(&completeCond, &completeMutex);
    }
    pthread_mutex_unlock(&completeMutex);
    int iOutput = 0;
    for (int i = 0; i < np; i++) {
        // Concatena os resultados
        for (int j = 0; j < threadData[i].tamOutput; j++) {
            Output[iOutput] = threadData[i].Output[j];
            iOutput++;
        }
        free(threadData[i].Output);
        if (i + 1 < np)
            Pos[i + 1] = Pos[i] + threadData[i].tamOutput;
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

        single_partition(task);  // Modifica diretamente o objeto original

        // Marca a tarefa como concluída
        pthread_mutex_lock(&completeMutex);
        completedTasks++;
        if (completedTasks == nP) {
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

    long long *P = malloc(nP * sizeof(long long));
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