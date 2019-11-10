#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <time.h>
#include "utils.h"

// Algoritmo BubbleSort obtido em:
// https://www.geeksforgeeks.org/bubble-sort/
void swap(int *xp, int *yp) 
{ 
	int temp = *xp; 
	*xp = *yp; 
	*yp = temp; 
}

void bubbleSort(int arr[], int n) 
{ 
int i, j; 
for (i = 0; i < n-1; i++)	 
	for (j = 0; j < n-i-1; j++) 
		if (arr[j] > arr[j+1]) 
			swap(&arr[j], &arr[j+1]); 
} 

// Algoritmo HeapSort obtido em:
// https://ide.geeksforgeeks.org/rFO7Lm
// A heap has current size and array of elements
struct MaxHeap
{
    int size;
    int* array;
};

// The main function to heapify a Max Heap. The function
// assumes that everything under given root (element at
// index idx) is already heapified
void maxHeapify(struct MaxHeap* maxHeap, int idx)
{
    int largest = idx;  // Initialize largest as root
    int left = (idx << 1) + 1;  // left = 2*idx + 1
    int right = (idx + 1) << 1; // right = 2*idx + 2

    // See if left child of root exists and is greater than
    // root
    if (left < maxHeap->size &&
        maxHeap->array[left] > maxHeap->array[largest])
        largest = left;

    // See if right child of root exists and is greater than
    // the largest so far
    if (right < maxHeap->size &&
        maxHeap->array[right] > maxHeap->array[largest])
        largest = right;

    // Change root, if needed
    if (largest != idx)
    {
        swap(&maxHeap->array[largest], &maxHeap->array[idx]);
        maxHeapify(maxHeap, largest);
    }
}

// A utility function to create a max heap of given capacity
struct MaxHeap* createAndBuildHeap(int *array, int size)
{
    int i;
    struct MaxHeap* maxHeap = (struct MaxHeap*) malloc(sizeof(struct MaxHeap));
    maxHeap->size = size;   // initialize size of heap
    maxHeap->array = array; // Assign address of first element of array

    // Start from bottommost and rightmost internal mode and heapify all
    // internal modes in bottom up way
    for (i = (maxHeap->size - 2) / 2; i >= 0; --i)
        maxHeapify(maxHeap, i);
    return maxHeap;
}

// The main function to sort an array of given size
void heapSort(int* array, int size)
{
    // Build a heap from the input data.
    struct MaxHeap* maxHeap = createAndBuildHeap(array, size);

    // Repeat following steps while heap size is greater than 1.
    // The last element in max heap will be the minimum element
    while (maxHeap->size > 1)
    {
        // The largest item in Heap is stored at the root. Replace
        // it with the last item of the heap followed by reducing the
        // size of heap by 1.
        swap(&maxHeap->array[0], &maxHeap->array[maxHeap->size - 1]);
        --maxHeap->size;  // Reduce heap size

        // Finally, heapify the root of tree.
        maxHeapify(maxHeap, 0);
    }
}

void ordenaAposMpi(int dst[], int tamanhoDst){
    heapSort(dst,tamanhoDst);
}

void inicializaVetor(int vetor[], int tamanhoVetor){
    srand(time(NULL));
    int i = 0;
    for(i = 0 ; i < tamanhoVetor ; i++){
        vetor[i] = rand() % (tamanhoVetor * 100);
    }
}

void imprimeVetor(int vetor[], int tamanhoVetor){
    int i = 0;
    for(i = 0 ; i < tamanhoVetor ; i++){
        printf("%d ",vetor[i]);
    }
    printf("\n\n");
}

void imprimeArquivo(int vetor[], int tamanhoVetor, char tipo, int imprimeSaida)
{
    if(imprimeSaida == 1){
        if(tipo == 'o'){
            FILE * arquivo;
            arquivo = fopen("saida.txt", "w");
            int i;

            fprintf(arquivo, "%s\n\t","Vetor original:");
            for(i = 0; i < tamanhoVetor ; i++){
                fprintf(arquivo,"%d ",vetor[i]);
            }
            fprintf(arquivo,"\n\n");

            fclose(arquivo);
        } else if(tipo == 'n'){
            FILE * arquivo;
            arquivo = fopen("saida.txt", "a+");
            int i;

            fprintf(arquivo, "%s\n\t","Vetor final nao ordenado:");
            for(i = 0; i < tamanhoVetor ; i++){
                fprintf(arquivo,"%d ",vetor[i]);
            }
            fprintf(arquivo,"\n\n");

            fclose(arquivo);
        } else {
            FILE * arquivo;
            arquivo = fopen("saida.txt", "a+");
            int i;
            fprintf(arquivo, "%s\n\t","Vetor final ordenado:");
            for(i = 0; i < tamanhoVetor ; i++){
                fprintf(arquivo,"%d ",vetor[i]);
            }
            fprintf(arquivo,"\n\n");
            printf("\n\n");
            fclose(arquivo);
        }
    }
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
    int imprimeSaida = atoi(argv[2]);
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
        if(imprimeSaida == 1){
            printf("Tamanho do vetor: %d\n", tamanhoVetor);
            printf("Tamanho n vetores: %d | tamanho do ultimo vetor: %d\n\n", tamVetSplit, tamUltPos);
        }
        imprimeArquivo(vetor,tamanhoVetor, 'o', imprimeSaida);
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
                juntaVetores(ultPosVetSpl, tamUltPos, vetorFinal, offset);
            } else {
                // demais processadores
                ret = MPI_Recv(nVetSpl, tamVetSplit * sizeof(int), MPI_INT,rank,0,MPI_COMM_WORLD, &status);
                if(ret != MPI_SUCCESS){
                    mpi_err(1,"MPI_Recv");
                }
                juntaVetores(nVetSpl, tamVetSplit, vetorFinal, offset);
                offset += tamVetSplit;
            }
            
        } 
        imprimeArquivo(vetorFinal,tamanhoVetor, 'n', imprimeSaida);
        ordenaAposMpi(vetorFinal, tamanhoVetor);
        //sort_after_mpi(vetorFinal,tamanhoVetor,tamVetSplit,proc_size);
        if(imprimeSaida == 1){
            printf("Vetor ordenado impresso em saida.txt\t");
        }
        imprimeArquivo(vetorFinal,tamanhoVetor, 'r', imprimeSaida);
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
            bubbleSort(ultPosVetSpl, tamUltPos);
            
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
            bubbleSort(nVetSpl, tamVetSplit);
            
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
