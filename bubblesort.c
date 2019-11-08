#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"
#include "utils.h"

void bubble_sort (int vetor[], int n) {
    int k, j, aux;

    for (k = 1; k < n; k++) {
        for (j = 0; j < n - 1; j++) {
            if (vetor[j] > vetor[j + 1]) {
                aux          = vetor[j];
                vetor[j]     = vetor[j + 1];
                vetor[j + 1] = aux;
            }
        }
    }
}


int main(int argc, char **argv)
{
    int tamanhoVetor = 100;
    int ret;
    char *hostname;
    int my_Rank;
    int proc_size;

    int vetor[tamanhoVetor];
    int i = 0;
    int count = 0;

    for(i = tamanhoVetor ; i > 0 ; i--){
        vetor[count] = i;
        count++;
    }

    for(i = 0 ; i < tamanhoVetor ; i++){
        printf("%d ",vetor[i]);
    }

    printf("\n");
    bubble_sort(vetor, tamanhoVetor);

    for(i = 0 ; i < tamanhoVetor ; i++){
        printf("%d ",vetor[i]);
    }
    printf("\n");
    printf("\n");
}
