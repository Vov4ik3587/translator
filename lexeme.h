//
// Created by vava on 6/7/22.
//

#ifndef TRANSLATOR_LEXEME_H
#define TRANSLATOR_LEXEME_H

#pragma once

#include <string>

using namespace std;

//Стуркутра, являющаяся програмным предстовлением токена.
struct lexeme {
    string name; //имя индендификатора
    int type;
    //Тип индендификатора
    //0 - не опрерделён
    //1 - int
    //2 - char
    //3 - float
    bool value; //Инциализирован ли индедификатор. true - инициализирован, false - нет.

    //Констукторы, для уменьшения кода
    lexeme() {
    }

    lexeme(string ind_name) {
        name = ind_name;
        type = 0;
        value = false;
    }


    //Для сета
    bool operator<(const lexeme &b) const {
        return name < b.name;
    }

    bool operator==(lexeme b) {
        return name == b.name;
    }
};

#endif //TRANSLATOR_LEXEME_H
