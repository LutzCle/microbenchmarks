#!/bin/bash

gcc -O2 -o test -lgmp -lgsl -lgslcblas cle_math.c test.c
