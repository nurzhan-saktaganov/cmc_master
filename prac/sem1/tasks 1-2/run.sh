#!/bin/sh

# $1 - program name
# $2 - tmp file name

if [ $# -lt 1 ]
	then echo "program name is missed"
	exit 1
fi

echo "Compiling: gcc -Wall -o $1.exe $1.c -lm"
gcc -Wall -o $1.exe $1.c -lm

tmp_file='tmp.data'

if [ $# -eq 2 ]
	then tmp_file=$2
fi

echo "Running: ./$1.exe $tmp_file"
./$1.exe $tmp_file

echo "./plotter.py $tmp_file"
./plotter.py $tmp_file

echo "Cleaning: rm $1.exe $tmp_file"
rm $1.exe $tmp_file