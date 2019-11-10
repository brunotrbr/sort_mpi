#!/bin/bash

rm -rf run;

mpicc -o run bubblesort.c utils.c;

