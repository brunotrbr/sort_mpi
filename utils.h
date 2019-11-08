#ifndef __UTILS_H__
#define __UTILS_H__

#include <err.h>
#include <stdarg.h>
#include <stdlib.h>
#include <mpi.h>"

#ifdef __cplusplus
export C {
#endif

/**
 * Funcao para impressao de erros do MPI
 * utilize da seguinte forma:
 *
 * <C>
 * int ret;	
 * int my_rank;
 *
 * ret = MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
 * if (ret != MPI_SUCCESS)
 * 	mpi_err(ret, "MPI_Comm_Rank")
 *
 * </C>
 *
 * sendo MPI_Comm_rank o nome de uma funcao do mpi qualquer
 * e "MPI_Comm_rank" a conversao da mesma em formato string
 *
 * A funcao tambem permite o uso de parametros como printf
 * por exemplo:
 *
 * <C>
 * int ret;
 * int my_rank;
 * unsigned int soma;
 *
 * ret = MPI_RECV(&soma, 1, MPI_UNSIGNED, my_rank - 1, 0, MPI_COMM_WORLD);
 * if (ret != MPI_SUCCESS)
 * 	mpi_err(ret, "MPI_Recv do %d", my_rank - 1);
 *
 * </C>
 *
 * @param eval erro recebido do MPI em formato int (i.e., ret)
 * @param format string para impressao de erro. Normalmente eh desejado o nome
 * 	da funcao que ocasionou o erro (como utilizado nos exemplos).
 **/
static inline
void
__attribute__((format(printf, 2, 3)))
mpi_err(int eval, const char *format, ...)
{
	int len;
	va_list vap;
	char str[MPI_MAX_ERROR_STRING];

	(void)MPI_Error_string(eval, str, &len);
	va_start(vap, format);
	warnx(format, vap);
	va_end(vap);
	err(1, "%s", str);
	/* NO RETURN */
}

/**
 * Funcao que captura o hostname da maquina hospedeira.
 * Senao for possivel capturar o hostname, o hostname #FAILED_HOSTNAME sera
 * utilizado.
 * A funcao realiza o alocamento dinamico da variavel hostname de acordo com o 
 * tamanho da string encontrada no arquivo /etc/hostname.
 *
 * Ao final da aplicacao, _deve_ ser chamada a funcao hostname_end() para
 * encerramento do hostname.
 *
 * @param hostname string a ser alocada com o hostname da maquina
 **/
#define FAILED_HOSTNAME	"failed_hostname"
void
hostname_init(char **hostname);


/**
 * Encerramento da operacoes relacionadas a hostname.
 * Essa funcao _deve_ ser chamada ao termino da aplicacao.
 *
 * @param hostname area de memoria previamente alocada por hostname_init()
 **/
static inline
void
hostname_end(char *hostname)
{
	free(hostname);
}

/**
 * Funcao que ordena um vetor de unsigned int a partir de _proc_size - 2_
 * vetores ordenados. 
 * Suponhe que um dos nodos eh um mestre que nao realiza ordenamento.
 *
 * _SE_ um dos nodos ordenou um vetor maior que _slice_, esse vetor deve ser
 * o ultimo presente no vetor _v_.
 *
 * 
 * @param v grande vetor desordenado, contendo _proc_size - 2_ vetores ordenados
 * @param vlen tamanho do vetor v
 * @param slice tamanho dos vetores ordenados (veja comentario sobre o ultimo
 * 	vetor)
 * @param proc_size numero de nodos presentes na simulacao
 **/
void
sort_after_mpi(unsigned int *v, size_t vlen, int slice, int proc_size);

#ifdef __cplusplus
}
#endif
#endif
