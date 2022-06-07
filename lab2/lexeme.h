#pragma once

#include <string>

using namespace std;

//���������, ���������� ���������� �������������� ������.
struct lexeme{
	string name; //��� ���������������
	int type;
	//��� ���������������
	//0 - �� ���������
	//1 - int
	//2 - char
	//3 - float
	bool value; //�������������� �� �������������. true - ���������������, false - ���.

	//�����������, ��� ���������� ����
	lexeme(){
	}
	lexeme(string ind_name){
		name = ind_name;
		type = 0;
		value = false;
	}

	//��� ����
	bool operator < (const lexeme& b) const{
			return name < b.name;
	}

	bool operator == (lexeme b){
			return name == b.name;
	}
};

