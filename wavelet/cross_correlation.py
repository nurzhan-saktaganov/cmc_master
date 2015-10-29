#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys

__author__ = 'Nurzhan Saktaganov'

# r_xy, on page 10, formula 1.47
def cross_correlation_sequence(x, y, m):
    if m >= 0:
        return sum([y[i + m] * x[i].conjugate() for i in range(len(y) - m)])
    else:
        return cross_correlation_sequence(y, x, -m).conjugate()

# R_xy, on page 18, formula 1.94
def cross_correlation_matrix(x, y):
    return [[ cross_correlation_sequence(x, y, j - i) \
                for i, _ in enumerate(x)] for j, _ in enumerate(y)]

def main():
    x = [1+1j, 1+2j, 1+3j, 1+4j]
    y = x[:]#[1,1,1,1]
    for row in cross_correlation_matrix(x,y):
        print row

if __name__ == '__main__':
    main()