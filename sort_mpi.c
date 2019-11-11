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

// A function to implement bubble sort 
void bubbleSort(int arr[], int n) 
{ 
int i, j; 
for (i = 0; i < n-1; i++)	 
    // Last i elements are already in place 
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

/**
 * Ordenamento do vetor semi-ordenado utilizando HeapSort
 * @param dst vetor desordenado, com partes ordenadas pelos nós escravos
 * @param tamanhoDst tamanho do vetor
 */
void ordenaAposMpi(int dst[], int tamanhoDst){
    heapSort(dst,tamanhoDst);
}

/**
 * Inicializa o vetor com número randômicos inteiros positivos
 * @param dst vetor que vai ser inicializado
 * @param tamanhoDst tamanho do vetor 
 */
void inicializaVetor(int dst[], int tamanhoDst){
    srand(time(NULL));
    int i = 0;
    for(i = 0 ; i < tamanhoDst ; i++){
        dst[i] = rand() % (tamanhoDst * 100);
    }
}

/**
 * Imprime o vetor na tela
 * @param dst vetor que vai ser impresso
 * @param tamanhoDst tamanho do vetor 
 */
void imprimeVetor(int dst[], int tamanhoDst){
    int i = 0;
    for(i = 0 ; i < tamanhoDst ; i++){
        printf("%d ",dst[i]);
    }
    printf("\n\n");
}

/**
 * Imprime os vetores no arquivo saida.txt
 * @param dst vetor que vai ser impresso
 * @param tamanhoDst tamanho do vetor 
 * @param tipo tipo que vai ser impresso: [o] vetor original, [n] vetor não ordenado, [r] vetor ordenado
 * @param imprimeSaida flag que indica se vai imprimir a saída. 1 para imprimir, 0 para não imprimir
 */
void imprimeArquivo(int dst[], int tamanhoDst, char tipo, int imprimeSaida)
{
    if(imprimeSaida == 1){
        if(tipo == 'o'){
            FILE * arquivo;
            arquivo = fopen("saida.txt", "w");
            int i;

            fprintf(arquivo, "%s\n\t","Vetor original:");
            for(i = 0; i < tamanhoDst ; i++){
                fprintf(arquivo,"%d ",dst[i]);
            }
            fprintf(arquivo,"\n\n");

            fclose(arquivo);
        } else if(tipo == 'n'){
            FILE * arquivo;
            arquivo = fopen("saida.txt", "a+");
            int i;

            fprintf(arquivo, "%s\n\t","Vetor final nao ordenado:");
            for(i = 0; i < tamanhoDst ; i++){
                fprintf(arquivo,"%d ",dst[i]);
            }
            fprintf(arquivo,"\n\n");

            fclose(arquivo);
        } else {
            FILE * arquivo;
            arquivo = fopen("saida.txt", "a+");
            int i;
            fprintf(arquivo, "%s\n\t","Vetor final ordenado:");
            for(i = 0; i < tamanhoDst ; i++){
                fprintf(arquivo,"%d ",dst[i]);
            }
            fprintf(arquivo,"\n\n");
            printf("\n\n");
            fclose(arquivo);
        }
    }
}

/**
 * Verifica se o elemento na posicao i do vetor é maior que i+1.
 * Se for, encerra o programa.
 * Dessa forma, não é necessário imprimir o vetor para confirmar a ordenação
 * @param vetor vetor a ser verificado
 * @param tamanhoVetor tamanho do vetor a ser verificado
 */
void verificaVetor(int vetor[], int tamanhoVetor){
    int i;
    for(i = 0 ; i < tamanhoVetor -2 ; i++){
        if(vetor[i] > vetor[i+1]){
            printf("vetor[%d] = %d > vetor[%d] = %d\n",i,vetor[i],i+1,vetor[i+1]);
            exit(-2);
        }
    }
}

/**
 * Imprime o tempo de execucao do programa
 * @param tempo tempo de execucao em segundos
 * @param numeroProcesso numero de processos executados
 */
void imprimeTempoExecucao(double tempo, int numeroProcessos){
    FILE * arquivo;
    arquivo = fopen("tempo.txt", "a+");
    fprintf(arquivo,"%s: ", "Numero de processos");
    fprintf(arquivo,"%d\n",numeroProcessos);
    fprintf(arquivo, "%s ","Tempo de execucao em segundos:");
    fprintf(arquivo,"%f",tempo);
    fprintf(arquivo,"\n\n");

    fclose(arquivo);
}

/**
 * Divide um vetor em pedacos menores
 * @param src vetor original
 * @param tamanhoSrc tamanho do vetor original
 * @param dst vetor menor, contendo parte do conteudo do vetor original
 * @param numcpy quantidade de inteiros que serão copiados
 * @param offset deslocamento no vetor original, para obter parte do conteudo do vetor original
 */
void divideVetor(int src[], int tamanhoSrc, int dst[], int numcpy, int offset){
    int i;
    for(i = numcpy ; i > 0 ; i--){
        dst[numcpy - i] = src[numcpy - i + offset];
    }
}

/**
 * Junta pedacos menores de vetor em um único vetor
 * @param src vetor menor original
 * @param tamanhoSrc tamanho do vetor menor
 * @param dst vetor completo, contendo todas as parte dos vetores menores
 * @param offset deslocamento no vetor original, para nao sobrepor o conteudo dos vetors menores
 */
void juntaVetores(int src[], int tamanhoSrc, int dst[], int offset){
    int i;
    for(i = 0 ; i < tamanhoSrc ; i++){
        dst[i + offset] = src[i];
    }
}

/**
 * Trecho principal do programa
 * Executar conforme o exemplo abaixo:
 * 
 * mpirun -np 4 run 10 1

* onde:
    np eh o numero de processos
    run é o nome do programa
    10 eh o tamanho do vetor
    1 informa se quer imprimir no arquivo de saida, 0 para não imprimir nada
 */
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
    clock_t inicio, fim;
    double tempoExecucao;
    
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

    // Obtencao dos tamanhos t e r
    tamVetSplit = tamanhoVetor / (proc_size-1); // tamanho t de elementos do vetor
    resTamVetSplit = tamanhoVetor % (proc_size-1); // resto r da divisao de t por n-1
    
    // Trecho paraleliza vetor
    int rank;
    int offset = 0;
    // inicializa array de tamanho t
    int *nVetSpl = (int *)malloc(tamVetSplit * sizeof(int));
    // calcula tamanho t + r
    int tamUltPos = tamVetSplit + resTamVetSplit;
    // inicializa array de tamanho t + r
    int *ultPosVetSpl = (int *)malloc(tamUltPos * sizeof(int));
    // Nodo mestre, so efetua a divisao do trabalho
    if(my_Rank == 0){
        printf("Tamanho do vetor: %d\n", tamanhoVetor);
        printf("Tamanho n vetores: %d | tamanho do ultimo vetor: %d\n\n", tamVetSplit, tamUltPos);
        // Imprime o vetor original
        imprimeArquivo(vetor,tamanhoVetor, 'o', imprimeSaida);
        inicio = clock();
        // Realiza o envio para os nos escravos
        for(rank = 1 ; rank < proc_size; rank++){
            if(rank == proc_size-1){
                // último escravo
                // divide o vetor em pedaco menor de tamanho t + r
                divideVetor(vetor, tamanhoVetor, ultPosVetSpl, tamUltPos, offset);
                // envia para o escravo
                ret = MPI_Send(ultPosVetSpl, tamUltPos * sizeof(int), MPI_INT, rank, 0, MPI_COMM_WORLD);
                if(ret != MPI_SUCCESS){
                    mpi_err(1,"MPI_Send");
                }
            } else {
                // demais escravos
                // divide o vetor em pedaco menor de tamanho t
                divideVetor(vetor, tamanhoVetor, nVetSpl, tamVetSplit, offset);
                // atualiza o deslocamento offset
                offset += tamVetSplit;
                // envia para o escravo
                ret = MPI_Send(nVetSpl, tamVetSplit * sizeof(int), MPI_INT, rank, 0, MPI_COMM_WORLD);
                if(ret != MPI_SUCCESS){
                    mpi_err(1,"MPI_Send");
                }
            }
        }
        offset = 0;
        // faz o recebimento dos dados enviados pelos escravos
        for(rank = 1 ; rank < proc_size ; rank++){
            if(rank == proc_size-1){
                // último escravo
                // recebe o vetor ordenado de tamanho t + r
                ret = MPI_Recv(ultPosVetSpl, tamUltPos * sizeof(int), MPI_INT,rank,0,MPI_COMM_WORLD, &status);
                if(ret != MPI_SUCCESS){
                    mpi_err(1,"MPI_Recv");
                }
                // junta no vetor de tamanho total
                juntaVetores(ultPosVetSpl, tamUltPos, vetorFinal, offset);
            } else {
                // demais escravos
                // recebe os vetores ordenados de tamanho t
                ret = MPI_Recv(nVetSpl, tamVetSplit * sizeof(int), MPI_INT,rank,0,MPI_COMM_WORLD, &status);
                if(ret != MPI_SUCCESS){
                    mpi_err(1,"MPI_Recv");
                }
                // junta no vetor de tamanho total
                juntaVetores(nVetSpl, tamVetSplit, vetorFinal, offset);
                // atualiza o deslocamento offset
                offset += tamVetSplit;
            }
            
        }
        // imprime vetor nao ordenado com n pedacos ordenados
        imprimeArquivo(vetorFinal,tamanhoVetor, 'n', imprimeSaida);
        // ordena o vetor desordenado com n pedacos ordenados
        sort_after_mpi(vetorFinal,tamanhoVetor,tamVetSplit,proc_size);
        if(imprimeSaida == 1){
            printf("Vetor ordenado impresso em saida.txt\t");
        }
        fim = clock();
        tempoExecucao = (fim - inicio) / CLOCKS_PER_SEC;
        // imprime no arquivo tempo.txt o tempo de execucao do programa
        imprimeTempoExecucao(tempoExecucao, proc_size);
        printf("Tempo impresso em tempo.txt\n");
        // imprime no arquivo de saida o vetor final ordenado
        imprimeArquivo(vetorFinal,tamanhoVetor, 'r', imprimeSaida);
        // verifica se o vetor possui algum elemento nao ordenado, para
        // confirmar a ordenacao sem ser necessario imprimir o vetor
        verificaVetor(vetorFinal,tamanhoVetor);
    } else {
        // Nodos escravos, efetuam o processamento dos dados
        // Identifica o rank do escravo
        ret = MPI_Comm_rank(MPI_COMM_WORLD, &my_Rank);
        if(ret != MPI_SUCCESS){
            mpi_err(1,"MPI_Comm_rank");
        }
        // se for o ultimo escravo, recebeu o vetor de tamanho t + r
        if(my_Rank == proc_size-1){
            // ultimo escravo
            printf("Processado no [%s] rank %d\n", hostname, my_Rank);
            // recebe o vetor ordenado de tamanho t + r
            ret = MPI_Recv(ultPosVetSpl, tamUltPos * sizeof(int), MPI_INT,MPI_ANY_SOURCE,0,MPI_COMM_WORLD, &status);
            if(ret != MPI_SUCCESS){
                mpi_err(1,"MPI_Recv");
            }
            // realiza ordenamento com bubble sort
            bubbleSort(ultPosVetSpl, tamUltPos);
            // envia o vetor de tamanho t + r ordenado para o no mestre
            ret = MPI_Send(ultPosVetSpl, tamUltPos * sizeof(int), MPI_INT, 0, 0, MPI_COMM_WORLD);
            if(ret != MPI_SUCCESS){
                mpi_err(1,"MPI_Send");
            }
        } else { // senao, recebeu o vetor de tamanho t
            // demais escravos
            printf("Processado no [%s] rank %d\n", hostname, my_Rank);
            // recebe o vetor ordenado de tamanho t
            ret = MPI_Recv(nVetSpl, tamVetSplit * sizeof(int), MPI_INT,MPI_ANY_SOURCE,0,MPI_COMM_WORLD, &status);
            if(ret != MPI_SUCCESS){
                mpi_err(1,"MPI_Recv");
            }
            // realiza ordenamento com bubble sort
            bubbleSort(nVetSpl, tamVetSplit);
            // envia o vetor de tamanho t ordenado para o no mestre
            ret = MPI_Send(nVetSpl, tamVetSplit * sizeof(int), MPI_INT, 0, 0, MPI_COMM_WORLD);
            if(ret != MPI_SUCCESS){
                mpi_err(1,"MPI_Send");
            }
        }
        
    }
    
    // Finaliza o MPI
    ret = MPI_Finalize();
    if(ret != MPI_SUCCESS){
        mpi_err(1,"MPI_Finalize");
    }
    return 0;
}
