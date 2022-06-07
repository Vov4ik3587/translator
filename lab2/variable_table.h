#pragma once

#include "const_table.h"
#include <vector>

//�����, ��� �������� ���������� ������, � ���� ���-������
//���������� � ���� ���-������� � ���������. ����������� ������������ �� ������� ������� ���������������
const int hash_N = 26*2 + 10; //������������ �������� ����
class variable_table{
 public:
	 //�����������, ��� ��������� ������ ������
	 variable_table(){
		 our_table = new vector<lexeme> [hash_N];
	 }
	 //����������
	 ~variable_table(){
		 for(int i = 0; i < hash_N; i++)
			 our_table[i].clear();
	 delete[] our_table;
	}
	
	 bool add_ind(string ind_name); //������� ���������� ���������������, true - ���� ��������, false - �� ��������.
	 bool set_ind_type(string ind_name, int type); //������� ���������� �������� ���� ���������������.
	 bool set_ind_val(string ind_name, bool val); //�������  ������� ���� ������������� ���������������.
	 bool check_ind_in_table(string ind_name); //��������, ������ �� �������������� � �������, true - ������, false - ���.
	 bool ind_adrress(string ind_name, int& hash, int& numb); //���������� ������ ����� ��������������, ���� false ���� ��� ��� � �������.
	 bool get_lexeme(string ind_name, lexeme& gets_lexeme); //�������� ������� �� ��������������, ���� ������ ��� - false.
	 bool get_lexeme(int hash, int numb, lexeme& gets_lexeme); //�������� ������� �� ��� ������� ������, ���� ������ ��� - false.
	 bool set_type_address(int hash, int numb, int type); //��������� ���� �� ������� ������
	 bool set_value_address(int hash, int numb, bool val); //��������� �������� �� ������� ������



 private:

	vector<lexeme> *our_table; //���� ��������	

	int calc_hash(string ind_name);	//������� ���������� ����. ������� - �����, ����� �������� �����, ����� ���������
	int find_in_vector(int hash, string ind_name);
};