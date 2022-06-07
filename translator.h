#ifndef TRANSLATOR_TRANSLATOR_H
#define TRANSLATOR_TRANSLATOR_H

#pragma once

#include <stack>
#include <map>

#include "variable_table.h"


class translator {

public:
    translator();
    //Констуркутор по умолчанию. Ввод постоянных таблиц.

    bool lex_analysis(string sorce_code_file, string tokens_file, string errors_file);
    //Фаза лексического анализа
    //sorce_code_file - имя файла с исходным кодом
    //tokens_file - имя файла токенов(будет генерироватся)
    //errors_file - имя файла для ошибок
    //Возврашает true, если фаза успешно завершенна

    bool parse(string tokens_file, string errors_file);
    //Фаза синтаксического анализа
    //tokens_file - имя файла токенов
    //errors_file - имя файла для ошибок

    void generate_code(string code_file);
    //Фаза генерации кода
    //code_file - файл, куда будет выведен код

    void out_tree(string f_name);
    //Вывод дерева для генерации кода в файл (концевым обходом)
    //f_name - имя файла

    //Струкутра, описывающая токен
    class token {
    public:
        int table_n; //номер таблицы
        int chain_n; //номер цепочки, если таблица постоянная - -1
        int numb; //номер в таблице / в цепочке

        token();

        token(int t_n, int ch_n, int n); //конструктор по типу "сразу всё"

        friend ostream &operator<<(ostream &os, token &out_t); //вывод токена
        friend istream &operator>>(istream &is, token &inp_t); //ввод токена

    };

private:
    //Постоянны таблицы
    const_table<char> admis_chars; //таблица допустимых символов, используемых для имён (за исключением цифр), номер таблицы - 0
    const_table<char> numbs; //таблица цифр, номер таблицы - 1
    const_table<char> opers_chars; //таблица символов операций, номер таблицы - 7
    const_table<string> opers; //таблица операций, номер таблицы - 2
    const_table<string> key_words; //таблица ключевых слов, номер таблицы - 3
    const_table<char> separaters; //таблица разделителей, номер таблицы - 4

    //Переменные таблицы
    variable_table identifier; //таблица индетификаторов, номер таблицы - 5
    variable_table consts; //таблица констант, номер таблицы - 6


    string get_token_text(token get_t); // определяем какая строка содержится в токене

    bool
    lex_string_process(string inp_str); //Лексическая обработка строки. Возврашает true, если ошибок нет, иначе - false

    int check_symbol(char sym); //проверят допустим ли символ и возврашает:
    // -1 - не допустим
    // Если допустим, то вернёт номер таблицы

    ifstream lex_sorce_s; //Лексический анализ. Поток для работы с файлом исходного кода.
    ofstream token_s; //Лексический анализ. Поток для работы с файлом токенов.
    ofstream errors_s; //Лексический анализ. Поток для работы с файлом ошибок.

    //===============================================================================================================
    //===================================== Для синтаксического анализа =============================================

    //Элемент таблицы разбора
    struct synt_table_el {
        vector<string> termenal; //терминальный символ
        int jump; //переход
        bool accept; //принимает или нет
        bool put_in_stack; //ложим в стек или нет
        bool should_return; //возвращается или нет
        bool error; //может вернуть ошибку или нет

    };

    vector<synt_table_el> parsing_table; //таблица разбора
    stack<int> parsing_stack; //стек, используемый для разбора


    // ===================================== Описание бинарного дерева =====================================

    //Бинарное дерево, для хранения результата третей лабы (если сделать его классом будет куча мороки, поэтому лучше так)
    struct tree_el {

        tree_el() {    //конструктор по умолчанию
            left = 0;
            right = 0;
        }

        int type; //Тип элемент
        //0 - знак операции(или присваивания)
        //1 - переменная
        //2 - точка с запятой


        string id; //хранится идентификаторв, в виде строки

        tree_el *left, *right; //правое и левое поддерева
    };

    void grow_tree(vector<token> code); //Создание дерева из вектора токенов и присоеденение его к основному дереву
    void grow_little_tree(vector<token> code, tree_el *&beg); //Создание маленького дерева(рекурсивное)

    tree_el *tree_begin; //корень дерева
    tree_el *tree_cur; //Указатель на текущий эллемент дерева

    void out_tree_rec(ofstream &out_f, tree_el *beg); //вывод рекурсивный, ибо удобно это

    //=======================================================================================
    //================================= Для генерации кода ==================================
    //=======================================================================================

    string generate_part_float(tree_el *beg); //генерирует часть дерева

    int check_tree_type(tree_el *beg); //определяет тип выражения

    map<string, string> float_constants; //каждой константе (вещественной) присвоим имя

};

#endif //TRANSLATOR_TRANSLATOR_H
