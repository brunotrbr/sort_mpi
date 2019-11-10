#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <time.h>
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

void inicializaVetor(int vetor[], int tamanhoVetor){
    srand(time(NULL));
    int i = 0;
    for(i = 0 ; i < tamanhoVetor ; i++){
        vetor[i] = rand() % (tamanhoVetor * 10);
    }
}

void imprimeVetor(int vetor[], int tamanhoVetor){
    int i = 0;
    for(i = 0 ; i < tamanhoVetor ; i++){
        printf("%d ",vetor[i]);
    }
    printf("\n\n");
}

/**
 * offset is the deslocation of pos[0] in array.
 */
void divideVetor(int src[], int tamanhoSrc, int dst[], int numcpy, int offset){
    int i;
    for(i = numcpy ; i > 0 ; i--){
        dst[numcpy - i] = src[numcpy - i + offset];
    }
}

/**
 * offset is the deslocation of pos[0] in array.
 */
void juntaVetores(int src[], int tamanhoSrc, int dst[], int offset){
    int i;
    for(i = 0 ; i < tamanhoSrc ; i++){
        dst[i + offset] = src[i];
    }
}

int main(int argc, char *argv[])
{
    int tamanhoVetor = atoi(argv[1]);
    int ret;
    char *hostname;
    int my_Rank;
    int proc_size;
    MPI_Status status;
    int tamVetSplit;
    int resTamVetSplit;
    int *vetor = (int *)malloc(tamanhoVetor * sizeof(int));
    int *vetorFinal = (int *)calloc(tamanhoVetor, sizeof(int));
    
    inicializaVetor(vetor, tamanhoVetor);

    // Inicialização do MPI
    hostname_init(&hostname);

    ret = MPI_Init(&argc, &argv);
    if(ret != MPI_SUCCESS){
        mpi_err(1,"MPI_Init");
    }

    ret = MPI_Comm_rank(MPI_COMM_WORLD, &my_Rank);
    if(ret != MPI_SUCCESS){
        mpi_err(1,"MPI_Comm_rank");
    }

    ret = MPI_Comm_size(MPI_COMM_WORLD, &proc_size);
    if(ret != MPI_SUCCESS){
        mpi_err(1,"MPI_Comm_size");
    }
    // Fim inicialização do MPI

    tamVetSplit = tamanhoVetor / (proc_size-1); // tamanho de elementos do vetor
    resTamVetSplit = tamanhoVetor % (proc_size-1); // ultimo pedaco vai ter tamVetSplit + resTamvetSplit
    // Trecho paraleliza vetor
    int rank;
    int offset = 0;
    // n pedaços do array
    int *nVetSpl = (int *)malloc(tamVetSplit * sizeof(int));
    // Ultimo pedaço do array
    int tamUltPos = tamVetSplit + resTamVetSplit;
    int *ultPosVetSpl = (int *)malloc(tamUltPos * sizeof(int));
    if(my_Rank == 0){
        printf("Tamanho do vetor: %d\n", tamanhoVetor);
        printf("Tamanho n vetores: %d | tamanho do ultimo vetor: %d\n\n", tamVetSplit, tamUltPos);
        printf("Vetor original:\t");
        imprimeVetor(vetor, tamanhoVetor);
        // Realiza o envio para os processadores
        for(rank = 1 ; rank < proc_size; rank++){
            if(rank == proc_size-1){
                // último processador
                divideVetor(vetor, tamanhoVetor, ultPosVetSpl, tamUltPos, offset);
                ret = MPI_Send(ultPosVetSpl, tamUltPos * sizeof(int), MPI_INT, rank, 0, MPI_COMM_WORLD);
                if(ret != MPI_SUCCESS){
                    mpi_err(1,"MPI_Send");
                }
            } else {
                // demais processadores
                divideVetor(vetor, tamanhoVetor, nVetSpl, tamVetSplit, offset);
                offset += tamVetSplit;
                ret = MPI_Send(nVetSpl, tamVetSplit * sizeof(int), MPI_INT, rank, 0, MPI_COMM_WORLD);
                if(ret != MPI_SUCCESS){
                    mpi_err(1,"MPI_Send");
                }
            }
        }
        offset = 0;
        // faz o recebimento dos dados enviados
        for(rank = 1 ; rank < proc_size ; rank++){
            if(rank == proc_size-1){
                // último processador
                ret = MPI_Recv(ultPosVetSpl, tamUltPos * sizeof(int), MPI_INT,rank,0,MPI_COMM_WORLD, &status);
                if(ret != MPI_SUCCESS){
                    mpi_err(1,"MPI_Recv");
                }
                imprimeVetor(ultPosVetSpl, tamUltPos);
                juntaVetores(ultPosVetSpl, tamUltPos, vetorFinal, offset);
            } else {
                // demais processadores
                ret = MPI_Recv(nVetSpl, tamVetSplit * sizeof(int), MPI_INT,rank,0,MPI_COMM_WORLD, &status);
                if(ret != MPI_SUCCESS){
                    mpi_err(1,"MPI_Recv");
                }
                imprimeVetor(nVetSpl, tamVetSplit);
                juntaVetores(nVetSpl, tamVetSplit, vetorFinal, offset);
                offset += tamVetSplit;
            }
            
        } 
        printf("Vetor final nao ordenado:\t");
        imprimeVetor(vetorFinal,tamanhoVetor);
        //sort_after_mpi(vetorFinal,tamanhoVetor,tamVetSplit,proc_size);
        printf("Vetor ordenado:\t");
        imprimeVetor(vetorFinal,tamanhoVetor);
        printf("acabou\n");
    } else {
        ret = MPI_Comm_rank(MPI_COMM_WORLD, &my_Rank);
        if(ret != MPI_SUCCESS){
            mpi_err(1,"MPI_Comm_rank");
        }
        if(my_Rank == proc_size-1){
            // último processador
            printf("Processado no [%s] rank %d\n", hostname, my_Rank);
            ret = MPI_Recv(ultPosVetSpl, tamUltPos * sizeof(int), MPI_INT,MPI_ANY_SOURCE,0,MPI_COMM_WORLD, &status);
            if(ret != MPI_SUCCESS){
                mpi_err(1,"MPI_Recv");
            }
            bubble_sort(ultPosVetSpl, tamUltPos);
            
            ret = MPI_Send(ultPosVetSpl, tamUltPos * sizeof(int), MPI_INT, 0, 0, MPI_COMM_WORLD);
            if(ret != MPI_SUCCESS){
                mpi_err(1,"MPI_Send");
            }
        } else {
            // demais processadores
            printf("Processado no [%s] rank %d\n", hostname, my_Rank);
            ret = MPI_Recv(nVetSpl, tamVetSplit * sizeof(int), MPI_INT,MPI_ANY_SOURCE,0,MPI_COMM_WORLD, &status);
            if(ret != MPI_SUCCESS){
                mpi_err(1,"MPI_Recv");
            }
            bubble_sort(nVetSpl, tamVetSplit);
            
            ret = MPI_Send(nVetSpl, tamVetSplit * sizeof(int), MPI_INT, 0, 0, MPI_COMM_WORLD);
            if(ret != MPI_SUCCESS){
                mpi_err(1,"MPI_Send");
            }
        }
        
    }
    
    ret = MPI_Finalize();
    if(ret != MPI_SUCCESS){
        mpi_err(1,"MPI_Finalize");
    }
    return 0;
}
