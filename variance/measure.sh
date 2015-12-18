#!/bin/bash

clang -O2 -msse4.1 -o measure -lm timer.c cle_math.c measure.c
