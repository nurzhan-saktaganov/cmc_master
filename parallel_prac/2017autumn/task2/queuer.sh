#!/usr/bin/env bash
mpisubmit.bg --nproc 128 --mode smp  ./task2bg.exe -- 15000 10000 4
