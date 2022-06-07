//
// Created by vava on 6/7/22.
//

#ifndef TRANSLATOR_CONST_TABLE_H
#define TRANSLATOR_CONST_TABLE_H

#pragma once

#include "lexeme.h"

#include <iostream>
#include <fstream>
#include <set>

//Класс, релазиующий хранение постоянных таблиц
//Поскольку у нас могут быть как целые строки, так и символы чтобы не тратить память - делаем шаблон
template<typename Table_type>
class const_table {
public:

    //Два конструктора. Скучный - по умлочанию, и для генерации из файла
    const_table() = default;

    const_table(string f_name) {
        generate_from_file(f_name);
    }

    //И деструктор, для приличия
    ~const_table() {
        our_table.clear();
    }

    //Генерирует таблицу из файла, возаращает true если файл открыли, false если нет
    bool generate_from_file(const string& f_name) {
        ifstream file_stream(f_name.c_str()); //поток чтения из файла

        if (file_stream.is_open()) {
            Table_type read_el; //считываемый элемент

            while (!file_stream.eof()) {
                file_stream >> read_el;
                our_table.insert(read_el);
            };

            file_stream.close();
            return true;
        } else
            return false;
    }

    //Добавление одного элемента в табилцу. Малоли, вдруг пригодится.
    void add_element(Table_type add_elem) {
        our_table.insert(add_elem);
    }

    //Проверка - есть ли элемент в таблице, true если есть, false если нет.
    bool is_element_in_table(Table_type check_el) {
        typename set<Table_type>::iterator el_numb = our_table.find(check_el); // находим номер элемента

        return el_numb != our_table.end() ? true
                                          : false; //элемента нет в таблице, если его "номер" равен концу структуры...STL же
    }

    //Поиск номера элемента. Если такого нет - false.
    bool get_elemen_number(Table_type check_el, int &num) {
        if (is_element_in_table(check_el)) {
            num = distance(our_table.begin(), our_table.find(check_el));
            return true;
        } else
            return false;
    }

    //Поиск элемента по номеру. Если такого нет - false.
    bool get_elemen_value(int num, Table_type &gets_el) {
        if (num >= 0 && num < our_table.size()) {
            typename set<Table_type>::iterator el_iter = our_table.begin();
            for (int i = 0; i < num; i++)
                el_iter++;
            gets_el = *el_iter;
            return true;
        } else
            return false;
    }

private:
    set<Table_type> our_table; //собственно, наша табличка
};

#endif //TRANSLATOR_CONST_TABLE_H
