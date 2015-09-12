#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import matplotlib.pyplot as pyplot
import numpy as np

__author__ = 'Nurzhan Saktaganov' 

def read_file(file_name):
    with open(file_name) as f:
        return [ map(float, line.split()) for line in f]

heat_map = lambda file_name: np.array(read_file(file_name))
    
def main():

    if len(sys.argv) == 2:
        pyplot.subplot(1, 1, 1)
        pyplot.pcolor(heat_map(sys.argv[1]))
        pyplot.title('result')
    elif len(sys.argv) == 3:
        pyplot.subplot(1, 2, 1)
        pyplot.pcolor(heat_map(sys.argv[1]))
        pyplot.title('result')

        pyplot.subplot(1, 2, 2)
        pyplot.pcolor(heat_map(sys.argv[2]))
        pyplot.title('sample')

    pyplot.show()

if __name__ == '__main__':
    main()
