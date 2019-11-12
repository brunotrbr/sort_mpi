#include "utils.h"
#include <limits.h>
#include <stdio.h>
#include <string.h>

#include <sys/stat.h>

static
unsigned int
pick_one_p(unsigned int *v, size_t vlen, unsigned int **p, 
		unsigned int **plimit, size_t plen, int *end)
{
	unsigned int ret;
	size_t i, c;

	ret = UINT_MAX;	
	c = 0;
	/* find min value */
	for (i = 0; i < plen; i++) {
		if (p[i] == NULL)
			continue;
		if (*(p[i]) > ret)
			continue;

		c = i;
		ret = *(p[i]);
	}
	/* update pointer; kill it, if it has reached its limit */
	p[c]++;
	if (p[c] == plimit[c])
		p[c] = NULL;
	/* is this the end? */
	for (i = 0; i < plen; i++) {
		if (p[i] != NULL)
			goto not_end;
	}
	*end = 1;
not_end:
	return (ret);
}

void
hostname_init(char **hostname)
{
	int ret;
	FILE *fp;
	struct stat st;
	size_t sret;

	*hostname = NULL;
	fp = fopen("/etc/hostname", "r");
	if (fp == NULL)
		goto fail;

	ret = stat("/etc/hostname", &st);
	if (ret != 0)
		goto fail;

	*hostname = calloc(1, (size_t)st.st_size);
	if (*hostname == NULL)
		err(1, "calloc");

	sret = fread(*hostname, (size_t)st.st_size, 1, fp);
	if (sret != 1)
		goto fail;

	/* remove new line */
	(*hostname)[st.st_size - 1] = '\0';
	return;
fail:
	if (*hostname != NULL)
		free(hostname);

	*hostname = calloc(1, strlen(FAILED_HOSTNAME));
	if (*hostname == NULL)
		err(1, "calloc");
	memcpy(*hostname, FAILED_HOSTNAME, strlen(FAILED_HOSTNAME));
}

void
sort_after_mpi(unsigned int *v, size_t vlen, int slice, int proc_size)
{
	unsigned int **p;
	unsigned int **plimit;
	unsigned int *vcopy;
	size_t i;
#ifdef DISTSORT_DEBUG
	size_t j;
#endif
	int end;
	const size_t plen = (size_t)proc_size - 1;
	const size_t szslice = (size_t)slice;

	p = calloc(plen, sizeof(*p));
	if (p == NULL)
		err(1, "calloc");

	plimit = calloc(plen, sizeof(*p));
	if (plimit == NULL)
		err(1, "calloc");

	vcopy = calloc(vlen, sizeof(*vcopy));
	if (vcopy == NULL)
		err(1, "calloc");

	memcpy(vcopy, v, vlen * sizeof(*v));

	for (i = 0; i < plen; i++) {
		p[i] = &vcopy[i * szslice];
		/* fix last case */
		if (i == (plen - 1))
			plimit[i] = &vcopy[vlen];
		else
			plimit[i] = &vcopy[(i + 1) * szslice];
#ifdef DISTSORT_DEBUG
		printf("p %zu plimit %zu\n", i * szslice,
		    i == (plen - 1) ? vlen : (i + 1) * szslice);
#endif
	}

	end = 0;
	i = 0;
	while (!end) {
#ifdef DISTSORT_DEBUG
		for (j = 0; j < plen; j++)
			printf("%u ", p[j] == NULL ? UINT_MAX : *(p[j]));
		printf("\n");
#endif
		v[i++] = pick_one_p(vcopy, vlen, p, plimit, plen, &end);	
		if (v[i - 2] > v[i - 1])
			err(1, "error: pos %zu %u %zu %u",
			    i - 2, v[i - 2], i - 1, v[i - 1]);
	}
	
	free(vcopy);
	free(plimit);
	free(p);
}
