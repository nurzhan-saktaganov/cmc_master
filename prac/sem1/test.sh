#!/bin/sh

# $1 - program name
# $2 - tmp file name

if [ $# -lt 1 ]
	then echo "program name is missed"
	exit 1
fi

echo "Compiling: gcc -Wall -o $1.exe $1.c -lm"
gcc -Wall -o $1.exe $1.c -lm

tmp_file1="tmp1.data"
tmp_file2="tmp2.data"

if [ $# -eq 3 ]
	then
		$tmp_file1=$2
		$tmp_file2=$3
fi

echo "Running: ./$1.exe $tmp_file1 $tmp_file2"
./$1.exe $tmp_file1 $tmp_file2

echo "./plotter.py $tmp_file1 $tmp_file2"
./plotter.py $tmp_file1 $tmp_file2

echo "Cleaning: rm $1.exe $tmp_file1 $tmp_file2"
rm $1.exe $tmp_file1 $tmp_file2