#ifndef TRANSLATOR_VARIABLE_TABLE_H
#define TRANSLATOR_VARIABLE_TABLE_H

#pragma once

#include "const_table.h"
#include <vector>

//Класс, для хранения переменных таблиц, в виде хеш-таблиц
//Реализация в виде хеш-таблицы с цепочками. Хеширование производится по первому символу идентификатора
const int hash_N = 26 * 2 + 10; //Максимальное значение хэша
class variable_table {
public:
    //Конструктор, для выделения нужной памяти
    variable_table() {
        our_table = new vector<lexeme>[hash_N];
    }

    //Деструктор
    ~variable_table() {
        for (int i = 0; i < hash_N; i++)
            our_table[i].clear();
        delete[] our_table;
    }

    bool add_ind(string ind_name); //Функция добавления идентификатора, true - если добавили, false - не добавили.
    bool set_ind_type(string ind_name, int type); //Функция добавления значения типа идентификатора.
    bool set_ind_val(string ind_name, bool val); //Функция  изменения типо инициализации идентификатора.
    bool check_ind_in_table(string ind_name); //Проверка, входит ли идентификатор в таблицу, true - входит, false - нет.
    bool ind_adrress(string ind_name, int &hash,
                     int &numb); //Возврашает полный адрес идентификатора, либо false если его нет в таблице.
    bool
    get_lexeme(string ind_name, lexeme &gets_lexeme); //Получаем лексему по идентификатору, если такого нет - false.
    bool get_lexeme(int hash, int numb,
                    lexeme &gets_lexeme); //Получаем лексему по его полному адресу, если такого нет - false.
    bool set_type_address(int hash, int numb, int type); //Установка типа по полному адресу
    bool set_value_address(int hash, int numb, bool val); //Установка значения по полному адресу



private:

    vector<lexeme> *our_table; //наша табличка

    int calc_hash(string ind_name);    //Функция вычисления хэша. Сначала - числа, потом строчные буквы, потом заглавные
    int find_in_vector(int hash, string ind_name);
};

#endif //TRANSLATOR_VARIABLE_TABLE_H
