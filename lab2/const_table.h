#pragma once

#include "lexeme.h"

#include <iostream>
#include <fstream>
#include <set>

//�����, ����������� �������� ���������� ������
//��������� � ��� ����� ���� ��� ����� ������, ��� � ������� ����� �� ������� ������ - ������ ������
template <typename Table_type> class const_table{
 public:

	 //��� ������������. ������� - �� ���������, � ��� ��������� �� �����
	 const_table(){
	 }
	 const_table(string f_name){
		 generate_from_file(f_name);
	 }
	 //� ����������, ��� ��������
	 ~const_table(){
		 our_table.clear();
	 }
	//���������� ������� �� �����, ���������� true ���� ���� �������, false ���� ���
	 bool generate_from_file(string f_name){
		 ifstream file_stream(f_name.c_str()); //����� ������ �� �����

		 if(file_stream.is_open()){
			 Table_type read_el; //����������� �������

			 while(!file_stream.eof()){
				 file_stream >> read_el;
				 our_table.insert(read_el);
			 };

			 file_stream.close();
			 return true;
		 }

		 else
			 return false;
	 }

	//���������� ������ �������� � �������. ������, ����� ����������.
	void add_element(Table_type add_elem){
		our_table.insert(add_elem);
	}
	//�������� - ���� �� ������� � �������, true ���� ����, false ���� ���.
	bool is_element_in_table(Table_type check_el){
		typename set<Table_type>::iterator el_numb = our_table.find(check_el); // ������� ����� ��������

		return el_numb != our_table.end() ? true : false; //�������� ��� � �������, ���� ��� "�����" ����� ����� ���������...STL ��
	}
	//����� ������ ��������. ���� ������ ��� - false.
	bool get_elemen_number(Table_type check_el, int& num){
		if(is_element_in_table(check_el)){
			num = distance(our_table.begin(), our_table.find(check_el));
			return true;
		}
		else
			return false;
	}
	//����� �������� �� ������. ���� ������ ��� - false.
	bool get_elemen_value(int num, Table_type& gets_el){
		if(num >= 0 && num < our_table.size()){
			typename set<Table_type>::iterator el_iter = our_table.begin();
			for(int i = 0; i < num; i++)
				el_iter++;
			gets_el = *el_iter;
			return true;
		}
		else
			return false;
	}

 private:
	set<Table_type> our_table; //����������, ���� ��������
};
