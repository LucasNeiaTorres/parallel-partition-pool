// Autores: Lucas Néia Torres, Inaiê Moscal Zambonin

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

void verifica_particoes(long long *Input, int n, long long *P, int np, long long *Output, int *Pos) {
    int erro = 0;
    long long faixa_min = 0;
    long long faixa_max;

    // Verificar as partições uma a uma
    for (int i = 0; i < np; i++) {
        faixa_max = P[i];
        
        for (int j = Pos[i]; j < ((i == np - 1) ? n : Pos[i + 1]); j++) {
            if (Output[j] < faixa_min || Output[j] >= faixa_max) {
                erro = 1;
                // printf("Erro na faixa %d: elemento %lld fora do intervalo [%lld, %lld)\n", 
                    //    i, Output[j], faixa_min, faixa_max);
            }
        }

        faixa_min = faixa_max;
    }

    // Exibir resultado final
    if (erro) {
        printf("===> particionamento COM ERROS\n");
    } else {
        printf("===> particionamento CORRETO\n");
    }
}