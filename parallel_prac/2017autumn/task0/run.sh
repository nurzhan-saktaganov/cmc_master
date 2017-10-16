#!/usr/bin/env bash
mpicc -Wall -o barrier.exe barrier.c\
  &&  mpirun -n 4 ./barrier.exe\
  && rm ./barrier.exe

