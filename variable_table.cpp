//
// Created by vava on 6/7/22.
//

#include "variable_table.h"

bool variable_table::add_ind(string ind_name) {
    int ind_hash = calc_hash(ind_name);
    bool check = check_ind_in_table(ind_name);
    if (!check) {
        our_table[ind_hash].push_back(lexeme(ind_name));
        return true;
    } else
        return false;
}

bool variable_table::set_ind_type(std::string ind_name, int type) {
    int ind_hash = calc_hash(ind_name);
    if (check_ind_in_table(ind_name)) {
        int ind_num = find_in_vector(ind_hash, ind_name); //ищим наш элемент
        our_table[ind_hash][ind_num].type = type;
        return true;
    } else
        return false;
}

bool variable_table::set_ind_val(std::string ind_name, bool val) {
    int ind_hash = calc_hash(ind_name);
    if (check_ind_in_table(ind_name)) {
        int ind_num = find_in_vector(ind_hash, ind_name); //ищим наш элемент
        our_table[ind_hash][ind_num].value = val;
        return true;
    } else
        return false;
}

bool variable_table::set_type_address(int hash, int numb, int type) {
    if (hash >= 0 && hash < hash_N && numb >= 0 && numb < our_table[hash].size()) {
        our_table[hash][numb].type = type;
        return true;
    }
    return false;
}

bool variable_table::set_value_address(int hash, int numb, bool val) {
    if (hash >= 0 && hash < hash_N && numb >= 0 && numb < our_table[hash].size()) {
        our_table[hash][numb].value = val;
        return true;
    }
    return false;
}

bool variable_table::check_ind_in_table(string ind_name) {
    int ind_hash = calc_hash(ind_name);

    int num = find_in_vector(ind_hash, ind_name); //номер элемента

    return num != -1 ? true : false;
}

int variable_table::find_in_vector(int hash, string ind_name) {
    lexeme find_t(ind_name);
    for (int i = 0; i < our_table[hash].size(); i++)
        if (find_t == our_table[hash][i])
            return i;
    return -1;
}

bool variable_table::ind_adrress(string ind_name, int &hash, int &numb) {
    hash = calc_hash(ind_name);
    numb = find_in_vector(hash, ind_name);
    return numb != -1 ? true : false;
}

bool variable_table::get_lexeme(string ind_name, lexeme &gets_lexeme) {
    int hash, numb;
    bool check = ind_adrress(ind_name, hash, numb);
    if (check) {
        gets_lexeme = our_table[hash][numb];
        return true;
    }
    return false;
}

bool variable_table::get_lexeme(int hash, int numb, lexeme &gets_lexeme) {
    if (hash >= 0 && hash < hash_N && numb >= 0 && numb < our_table[hash].size()) {
        gets_lexeme = our_table[hash][numb];
        return true;
    } else
        return false;
}


int variable_table::calc_hash(string ind_name) {
    if ('0' <= ind_name[0] && ind_name[0] <= '9')
        return ind_name[0] - '0';
    if ('a' <= ind_name[0] && ind_name[0] <= 'z')
        return ind_name[0] - 'a' + 10;
    if ('A' <= ind_name[0] && ind_name[0] <= 'Z')
        return ind_name[0] - 'A' + 36;
    return 0;
}
