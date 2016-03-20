#!/usr/bin/env python
#-*- coding: utf-8 -*-

import json
import signal

__author__ = 'Nurzhan Saktaganov'

options = \
    '1 - Перейти к выбору исходных данных;\n' +\
    '2 - Изменить параметры;\n' +\
    '3 - Закончить работу.\n'

def main():
    signal.signal(signal.SIGINT, good_bye)
    try:
        config = load_config("config.json")
    except Exception, e:
        print 'Ошибка загрузки файла с конфигурацией.\n' +\
                'Возможно, поврежден файл "config.json".'
        return

    choose_table = True

    while True:
        if choose_table:
            table_file_path = get_table_path()
        params = request_params(config)
        params['table'] = table_file_path
        save_params(config['parameters'], params)
        try:
            execfile('./run.py')
        except Exception, e:
            print e
            print 'В процессе вычисления возникла ошибка.\n' +\
                    'Выполнение программы завершается.'
            return
        
        choose = None

        print options
        while True:
            try:
                choose = raw_input("Выберите вариант: ")
                choose = int(choose)
                if choose == 1:
                    choose_table = True
                if choose == 2:
                    choose_table = False
                if choose == 3:
                    return
                if choose in [1,2]:
                    break
            except Exception, e:
                print 'Неправильный вариант, попытайтесь еще: ' + str(choose)

def load_config(file_name):
    with open(file_name, 'r') as config_file:
        config_string = ''.join([line for line in config_file])
    return json.loads(config_string)

def save_params(file_name, params):
    with open(file_name, 'w') as param_file:
        param_file.write(json.dumps(params))

def request_params(config):
    params = {key : None for key in config['input_params'].keys()}
    for key in sorted(config['input_params'].keys()):
        value_name = config['input_params'][key].encode('utf-8')
        to_user = 'Введите ' + value_name + ': '
        while True:  
            try:
                params[key] = raw_input(to_user)
                params[key] = float(params[key])
                break
            except Exception, e:
                pass
            print 'Неправильный ввод, попытайтесь снова: ' + str(params[key])
    return params

def get_table_path():
    table_path = None
    while True:
        try:
            table_path = raw_input('Введите имя файла: ')
            with open(table_path, 'r') as dummy:
                pass
            return table_path
        except Exception, e:
            pass
        print 'Нет такого файла, попытайтесь снова: ' + str(table_path)

def good_bye(signal, frame):
    print '\nДо свидания!'
    exit()

if __name__ == '__main__':
    main()