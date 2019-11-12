#!/bin/bash

rm -rf run;

mpicc -o run sort_mpi.c utils.c;

