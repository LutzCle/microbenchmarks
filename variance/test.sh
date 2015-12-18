#!/bin/bash

gcc -O2 -msse4.1 -o test -lm -lgmp -lgsl -lgslcblas cle_math.c test.c
