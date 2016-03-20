#!/usr/bin/env python
#-*- coding: utf-8 -*-

import sys
import json
from math import exp

__author__ = 'Nurzhan Saktaganov'

def main():
    signal.signal(signal.SIGINT, good_bye)
    config = load_json('config.json')
    params = load_json(config['parameters'])

    table = load_table(params['table'])
    info, t = split_table(table)

    x_new = step_3(info, t, config, params)
    X_recr = step_4(info)
    step_5(X_recr, config)
    X_recr_tr = step_6(X_recr, config)
    X_sum = step_7(x_new, X_recr, X_recr_tr)
    step_8(X_sum, config)
    intensities = step_9(X_sum)
    step_10(intensities, config)
    step_11(config, params)

    return

# Загружает файл в формате JSON.
# Возврашает JSON в виде словаря.
def load_json(file_name):
    with open(file_name, 'r') as config_file:
        config_string = "".join([line for line in config_file])
    return json.loads(config_string)

def get_row(line):
    row = line.strip().replace(',', '.').split('\t')
    try:
        id = int(row[0])
        row = map(float, row)
        row[0] = id
    except Exception, e:
        pass
    return row

# Загружает таблицу, удаляет заголовок
#   и возвращает содержимое в виде
#   двумерного списка / матрицы
def load_table(file_name):
    with open(file_name, 'r') as data_file:
        table = [get_row(line) for line in data_file]
    # Удаление заголовка
    del table[0]

    # Проверка таблицы на корректность
    for row in table:
        try:
            if len(table) != len(row) - 6:
                raise Exception
            map(float, row)
        except Exception, e:
            raise Exception('Неправильный формат таблицы.')

    return table

# info это словарь
#   ключ - уникальный идентификатор района
#   значение - это список [b, c, d, e, f]
#       b - численность населения в районе, тыс.чел
#       c - численность занятого населения в районе, тыс. чел
#       d - число мест приложения труда в районе, штук
#       e - рекреционный потенциал района, тыс. мест
#       f - вес района, дробное число
#
# t - это двумерный список / матрица
#   на позиции (i,j) находится средние затраты времени
#   на передвижение между парами районов, минуты, дробное число
def split_table(table):
    info = {row[0]: row[1:6] for row in table}
    t = map(lambda row: row[6:], table)
    return info, t

# Для всех i,j проверяет условие 
#   |x(i,j) - x_new(i,j)| <= accuracy
#   возращает False, если хоть в одном месте нарушается
#   иначе возвращает True
def is_satisfy(x, x_new, accuracy):
    size = len(x)
    for i in range(size):
        for j in range(size):
            if abs(x[i][j] - x_new[i][j]) > accuracy:
                return False
    return True

# Сохраняет матрицу в файл в формате tsv
def save_matrix(matrix, to_file):
    with open(to_file, 'w') as output:
        for row in matrix:
            output.write('\t'.join(map(str, row)) + '\r\n')
    return

# Получаем население всего города
def get_city_population(info):
    return sum(map(lambda value: value[0], info.values()))

# Получаем количество всего занятого населения
def get_city_busy_population(info):
    return sum(map(lambda value: value[1], info.values()))

# Сумма весов всех районов
def get_weights_sum(info):
    return sum(map(lambda value: value[4], info.values()))

# Число мест проиложения труда во всех исследуемых районах
def get_total_work_places(info):
    return sum(map(lambda value: value[2], info.values()))


def get_A(info, params):
    size = len(info.keys())
    # cp - city population
    cp = get_city_population(info)
    # tb - total busy
    tb = get_city_busy_population(info)
    # sw - summary weights
    sw = get_weights_sum(info)

    multiplier_1 = cp * params['c'] * params['d'] / tb
    multiplier_2 = 0.5 * 1.5 * tb * params['c'] * params['e'] / sw
    A = [multiplier_1 * info[i][0] + multiplier_2 * info[i][4]
            for i in range(size)]
    return A

def get_A_dividers(b, t, l):
    size = len(b)
    dividers =\
        [sum([b[j] * exp(-l * t[i][j])
                for j in range(size)])
                    for i in range(size)]
    return dividers

def update_a(info, t, params, b):
    size = len(b)
    A = get_A(info, params)
    l = params['g']
    dividers = get_A_dividers(b, t, l)
    a_new = [A[i] / dividers[i] for i in range(size)]
    return a_new

def get_B(info, params):
    size = len(info.keys())
    # cp - city population
    cp = get_city_population(info)
    # wp - work places
    wp = get_total_work_places(info)
    multiplier = cp * params['c'] * params['d'] / wp
    B = [ multiplier * info[i][2] for i in range(size)]
    return B

def get_B_dividers(a, t, l):
    size = len(a)
    dividers =\
        [sum([a[i] * exp(-l * t[i][j])
                for i in range(size)])
                    for j in range(size)]
    return dividers

def update_b(info, t, params, a):
    size = len(a)
    B = get_B(info, params)
    l = params['g']
    dividers = get_B_dividers(a, t, l)
    b_new = [B[i] / dividers[i] for i in range(size)]
    return b_new

def step_3(info, t, config, params):
    # Сколько районов у нас имеется
    size = len(t)
    l = params['g']
    accuracy = params['a']

    # Подпункт a
    a, b = [1] * size, [1] * size

    # Подпункт b
    x = [[a[i] * b[j] * exp(-l * t[i][j])
            for j in range(size)]
                for i in range(size)]

    count = 0

    while True:
        count += 1
        sys.stdout.write(str(count) + '\r')
        sys.stdout.flush()
        # Подпункт c
        a_new = update_a(info, t, params, b)

        # Подпункт d
        b_new = update_b(info, t, params, a)

        # Подпункт e
        x_new = [[a_new[i] * b_new[j] * exp(-l * t[i][j])
                for j in range(size)]
                    for i in range(size)]

        # Подпункт f
        is_accuracy_satisfy = is_satisfy(x, x_new, accuracy)
        
        if not is_accuracy_satisfy:
            # Подпункт g
            a = a_new
            b = b_new
            x = x_new
        else:
            # Подпункт h
            save_matrix(x_new, config['step 3 output'])
            break
    return x_new

def step_4(info, params):
    size = len(info.keys())
    city_population = get_city_population(info)
    X_recr = [[info[i][0] * params['f'] * info[j][3] /  city_population
                for j in range(size)]
                    for i in range(size)]
    return X_recr

def step_5(X_recr, config):
    save_matrix(X_recr, config['step 5 output'])
    return

# Для шага 6
# Транспонирует квадратную матрицу
def transpose(X):
    size = len(X)
    X_transposed = [[X[i][j]
                        for i in range(size)]
                            for j in range(size)]
    return X_transposed

def step_6(X_recr, config):
    X_transposed = transpose(X_recr)
    save_matrix(X_transposed, config['step 6 output'])
    return X_transposed

def step_7(x_new, X_recr, X_recr_tr):
    size = len(x_new)
    X_sum = [[ x_new[i][j] + X_recr[i][j] + X_recr_tr[i][j]
                for j in range(size)]
                    for i in range(size)]
    return X_sum

def step_8(X_sum, config):
    save_matrix(X_sum, config['step 8 output'])
    return

def step_9(X_sum):
    size = len(X_sum)
    intensities = [0.0] * size
    for id in range(size):
        # Строка под номером id
        intensities[id] += sum(X_sum[id])
        # Столбец под номером id
        intensities[id] += sum([X_sum[i][id] for i in range(size)])
    return intensities

def step_10(intensities, config):
    with open(config['step 10 output'], 'w') as output:
        for id, intensity in enumerate(intensities):
            output.write(str(id) + '\t' + str(intensity) + '\n')

def step_11(config, params):
    with open(config['step 11 output'], 'w') as output:
        param_names = config['input_params']
        for key in sorted(param_names.keys()):
            to_print = param_names[key] + '=' + str(params[key]) + '\n'
            output.write(to_print.encode('utf-8'))

def good_bye(signal, frame):
    print '\nДо свидания!'
    exit()

if __name__ == '__main__':
    main()
    
