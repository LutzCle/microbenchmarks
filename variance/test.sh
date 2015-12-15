#!/bin/bash

gcc -o test -lgmp -lgsl -lgslcblas cle_math.c test.c
