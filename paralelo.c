#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"
#include "utils.h"

int main(int argc, char **argv)
{
    int ret;
    char *hostname;
    int my_Rank;
    int proc_size;
    unsigned int soma;
    unsigned int total;
    MPI_Status status;
    int count;

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

    soma = 1;
    total = 0;
    if(my_Rank == 0){
        int aux = 1;
        for(aux ; aux < proc_size ; aux++){
            ret = MPI_Send(&soma, 1, MPI_UNSIGNED, aux, 0, MPI_COMM_WORLD);
            if(ret != MPI_SUCCESS){
                mpi_err(1,"MPI_Send");
            }
        }  
        aux = 1;
        for(aux ; aux < proc_size ; aux++){
            ret = MPI_Recv(&soma, 1, MPI_UNSIGNED,aux,0,MPI_COMM_WORLD, &status);
            if(ret != MPI_SUCCESS){
                mpi_err(1,"MPI_Recv");
            }
            total += soma;
        }  

        printf("Soma total vale: %d\n", total);
        printf("\n");
    } else {
        ret = MPI_Recv(&soma, 1, MPI_UNSIGNED, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        if(ret != MPI_SUCCESS){
            mpi_err(1,"MPI_Recv");
        }

        ret = MPI_Get_count(&status, MPI_UNSIGNED, &count);
        if(ret != MPI_SUCCESS){
            mpi_err(1,"MPI_Get_count");
        }

        // poderia verificar quem não enviou, procurar função
        if(count != 1){
            mpi_err(1,"Count errado %d", count);
        }

        soma = soma + 1;
        printf("[%s] soma tem o valor de %d. my rank: %d\n", hostname, soma, my_Rank);

        ret = MPI_Send(&soma, 1, MPI_UNSIGNED, 0, 0, MPI_COMM_WORLD);
        if(ret != MPI_SUCCESS){
            mpi_err(1,"MPI_Send");
        }
    }



    ret = MPI_Finalize();
    if(ret != MPI_SUCCESS){
        mpi_err(1,"MPI_Finalize");
    }

    return 0;
}
