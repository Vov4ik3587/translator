#include "translator.h"

translator::translator(){
	admis_chars.generate_from_file("table_chars.txt");
	numbs.generate_from_file("table_numbs.txt");
	opers_chars.generate_from_file("table_opers_chars.txt");
	opers.generate_from_file("table_opers.txt");
	key_words.generate_from_file("table_key_words.txt");
	separaters.generate_from_file("table_seps.txt");

	//���� ������� �������

	ifstream inp_tab("parsing table.txt");

	while(!inp_tab.eof()){
		synt_table_el add_el; //����������� ��������
		string read_str;

		inp_tab >> read_str;

		while(read_str != "|"){
			add_el.termenal.push_back(read_str);
			inp_tab >> read_str;
		};

		inp_tab >> add_el.jump >> add_el.accept >> add_el.put_in_stack >> add_el.should_return >> add_el.error;
		parsing_table.push_back(add_el);
	};

	tree_begin = 0;
}

translator::token::token(){
}

translator::token::token(int t_n, int ch_n, int n){
	table_n = t_n;
	chain_n = ch_n;
	numb = n;
}

ostream& operator << (ostream& os, translator::token& out_t){
	os << out_t.table_n << " " << out_t.chain_n << " " << out_t.numb << endl;
	return os;
}

istream& operator >> (istream& is, translator::token& inp_t){
	is >> inp_t.table_n >> inp_t.chain_n >> inp_t.numb;
	return is;
}

string translator::get_token_text(token get_t){

	string rez_str; //������, ������� �� ����

	switch(get_t.table_n){ //���� �� ��������
		case 2:{
			opers.get_elemen_value(get_t.numb, rez_str);
	    } break;
		case 3:{
			key_words.get_elemen_value(get_t.numb, rez_str);
		}break;
		case 4:{
			char sym;
			separaters.get_elemen_value(get_t.numb, sym);
			char s2[1]; //������� �����
			s2[0] = sym;
			rez_str.assign(s2, 1);
		}break;
		case 5:{
			lexeme tmp_l;
			identifier.get_lexeme(get_t.chain_n, get_t.numb, tmp_l);
			rez_str = tmp_l.name;
		}break;
		case 6:{
			lexeme tmp_l;
			consts.get_lexeme(get_t.chain_n, get_t.numb, tmp_l);
			rez_str = tmp_l.name;
		}break;
	}

	return rez_str;

}

bool translator::lex_analysis(string sorce_code_file, string tokens_file, string errors_file){

	lex_sorce_s.open(sorce_code_file.c_str(), ios::in); //����� ��� ������ � ������ ��������� ����
	token_s.open(tokens_file.c_str(), ios::out); //����� ��� ������ � ������ �������
	errors_s.open(errors_file.c_str(), ios::out); //����� ��� ������ � ������ ������

	string inp_str; //������� ������
	bool has_error = false;

	while(!lex_sorce_s.eof() && !has_error){
		lex_sorce_s >> inp_str;
		has_error = !lex_string_process(inp_str);

	};


	lex_sorce_s.close();
	token_s.close();
	errors_s.close();

	return !has_error;
}

bool translator::lex_string_process(string inp_str){
	if(inp_str.size() != 0){
		bool has_error = false;
		int first_sym_type = check_symbol(inp_str[0]);
		bool local_error = false; //������ �� � ������ �������
		string continue_str;
		string basic_inp = inp_str;
		if(inp_str.size() > 1 && inp_str[0] == '-' && numbs.is_element_in_table(inp_str[1]))
			first_sym_type = 1;
		switch(first_sym_type){

			case -1:{ //� ������ ������������� �������
				errors_s << "� " << inp_str << " ����������� ������������ �������. ������ ������ ����������." << endl;
				has_error = true;

				cout << "We have a error!" << endl;
					}break;

			case 0:{ //���� � ������� ������� ����� ��������� ��������������
				int i;
				//��������� ���� �� ���� ����� �������������
				for(i = 1; i < inp_str.size() && !local_error; i++){
					local_error = !(admis_chars.is_element_in_table(inp_str[i]) || numbs.is_element_in_table(inp_str[i]));
				}
				int start = i;
				if(local_error)
					start--;
				continue_str = inp_str.substr(start);
				inp_str = inp_str.substr(0, start);

				//���� �� ������, �� �������� �������� ��� ��� ��� ���
				//���� �� �������� ����� � ����� ������, ���� ��� - ������� � �������������� � ����� ������)
				if(key_words.is_element_in_table(inp_str)){
					int n;
					key_words.get_elemen_number(inp_str, n);
					token_s << token(3, -1, n);
				}
				else{
					identifier.add_ind(inp_str);
					int ch_n, n;
					identifier.ind_adrress(inp_str, ch_n, n);
					token_s << token(5, ch_n, n);
				}
				local_error = !lex_string_process(continue_str);

				   }break;

			case 1:{ //���� ������ ������ - ���������
				int i;
				//��������� ��������� �� ���
				for(i = 1; i < inp_str.size() && !local_error; i++){
					local_error = !(numbs.is_element_in_table(inp_str[i]) || inp_str[i] == '.'); //��� ���� �����, ���� �����
				}
				int start = i;
				if(local_error)
					start--;
				local_error = false;
				continue_str = inp_str.substr(start);
				if(continue_str.size() > 0){
					if(admis_chars.is_element_in_table(continue_str[0])){
						local_error = true;
						errors_s << "����� ����� - �����, ��-��!" << endl;
					}
				}
				inp_str = inp_str.substr(0, start);

				int points_n = inp_str.find_last_of('.') - inp_str.find_first_of('.'); //���������� ��������� �����

				//� ��� ����� ���� ������ ���� �����
				if(points_n != 0)
					local_error = true;

				//���� ��� ������, �� ��������� ������� �������� � ������ �����
				if(!local_error){
					consts.add_ind(inp_str);
					int ch_n, n;
					consts.ind_adrress(inp_str, ch_n, n);
					token_s << token(6, ch_n, n);
					local_error = !lex_string_process(continue_str);
				}			

				   }break;

			case 7:{ //���� ������ ������ ��� ������ ��������
				if(inp_str[0] != '/'){
					int i;
					string oper_s = inp_str.substr(0, 2);
					int oper_l;
					//�������� ����� ������ ���� ������������������
					local_error = !opers.is_element_in_table(oper_s);
					//������ - ��������������
					if(local_error){
						oper_s = inp_str.substr(0,1);
						local_error = !opers.is_element_in_table(oper_s);
						oper_l = 1;
					}
					else{
						if(inp_str.size() == 1)
							oper_l = 1;
						else
							oper_l = 2;
					}
					//���� �� ������ - ������ �����
					if(!local_error){
						int n;
						opers.get_elemen_number(oper_s, n);
						token_s << token(2, -1, n);
						continue_str = inp_str.substr(oper_l, inp_str.size()-oper_l);
						local_error = !lex_string_process(continue_str);
					}
				}
				else{ //���� ���� ���������� �� �����������
					if(inp_str.size() > 1){
						switch(inp_str[1]){
						case '*':{
							string garb; //���� ����� ������ ������
							bool comment_end = false;
							while(!comment_end && !lex_sorce_s.eof()){
								lex_sorce_s >> garb;
								int st = garb.find("*/");
								if(st != -1)
									comment_end = true;
							}
							if(!lex_sorce_s.eof()){
								int st1 = garb.find_first_of("*/");
								continue_str = garb.substr(st1+2);
								local_error = !lex_string_process(continue_str);
							}
							else{
								local_error = true;
								errors_s << "���������� ����������" << endl;
							}
								 }break;
						case '/':{
							string garb;
							getline(lex_sorce_s, garb);
								 }break;
						default:{
							errors_s << "������, ������ / �� ���������� �����������" << endl;
							local_error = true;
						  }break;

					};
					}
					else{
						errors_s << "������, ������ / �� ���������� �����������" << endl;
						local_error = true;
					}
				}

				   }break;
			case 4 :{ // ���� �����������

				continue_str = inp_str.substr(1);
				int n;
				separaters.get_elemen_number(inp_str[0],n);
				token_s << token(4, -1, n);
				local_error = !lex_string_process(continue_str);

					}break;
			default: {
				errors_s << "������������ ���������� �������������� ������� �������" << endl;
				has_error = true;

				cout << "����������� ������" << endl;
					 }break;

		};
		if(local_error){
			cout << "We have a error!" << endl;
			errors_s << "������ � " << basic_inp << " , ������������ �������" << endl;
		}
		return !(has_error || local_error);
	}
	else
		return true;
}

int translator::check_symbol(char sym){
	if(admis_chars.is_element_in_table(sym))
		return 0;
	if(numbs.is_element_in_table(sym))
		return 1;
	if(opers_chars.is_element_in_table(sym))
		return 7;
	if(separaters.is_element_in_table(sym))
		return 4;
	return -1;
}

bool translator::parse(string tokens_file, string errors_file){

	ifstream parse_token_f(tokens_file.c_str()); //����� ��� ����� �������
	ofstream parse_error_f(errors_file.c_str()); //����� ��� ������ ������

	token cur_t, next_t; //������� ����� � ��������� �� ���

	bool local_error = false; //������� ������

	parse_token_f >> cur_t;
	parse_token_f >> next_t;

	int cur_row = 0; //������� ������ � ������� �������
	int prev_row; //���������� ������ � ������� �������

	bool have_type = false; //���������� �� �� ������ ���
	int type_type; //������������� ���

	//���������� ��� ���������� ������ �������

	bool little_tree_bg = false; //������ �� ���������� ������ ������
	string token_str_prev; //�������� ����������� ������
	int id_numb; //����� ������������ ��������������

	vector<token> little_tree_code; // ������ ��� �������� ���������� ������

	while(!parse_token_f.eof() && !local_error){

		string token_str = get_token_text(cur_t); // ����� ����� ���������� � ������
		if(cur_t.table_n == 5 || cur_t.table_n == 6)
			token_str = "ID";

		if(token_str == "ID")
			little_tree_bg = true;

		bool find_terminal = false; //�������� �� ������ ��������

		for(int i = 0; i < parsing_table[cur_row].termenal.size() && !find_terminal; i++){
			if(parsing_table[cur_row].termenal[i] == token_str)
				find_terminal = true;
		}

		if(find_terminal){ //���� �������� ��, ��� ������� �� ������������ ���
	
			bool change_row = false; //������� �� �� ������

			if(parsing_table[cur_row].put_in_stack)
				parsing_stack.push(cur_row+1); //���� ���� �������� � ���� - �����

			if(parsing_table[cur_row].accept){ //��������� �������� � ���� ���� - ��������� ������
				
				if(little_tree_bg){
					little_tree_code.push_back(cur_t);
				}

				if(token_str == ";" || token_str == ","){ //���� ��������� ������ �������� ���������
					grow_tree(little_tree_code); //�������� �� ��� ����� � ������

					//� ������� � �������� ���������
					little_tree_code.clear();
					little_tree_bg = false;

				}
				
				//���, �������� ���� ������ ���
				if(token_str == ";"){
					have_type = false;
				}

				//���� �� ����� ���, �� �� ��� ����������
				if(token_str == "int" || token_str == "char" || token_str == "float"){
					have_type = true;
					if(token_str == "int")
						type_type = 1;
					if(token_str == "char")
						type_type = 2;
					if(token_str == "float")
						type_type = 3;
				}


				//������� ��� � ������� ���������������
				if(token_str == "ID" && have_type && cur_row == 47){
					lexeme get_lex;
					identifier.get_lexeme(get_token_text(cur_t), get_lex);
					if(get_lex.type == 0)				
						identifier.set_ind_type(get_token_text(cur_t),type_type);
					else{
						parse_error_f << "������ � ��������� " << get_token_text(cur_t) << " ��������� ���������� ����������" <<endl;
						cout << "Lex error" << endl;
						local_error = true;
					}

				}

				//���� ����� ���������� ��������� ���-�� ���������
				if(cur_row == 30 && cur_t.table_n != 5){
					parse_error_f << "������ � ��������� " << get_token_text(cur_t) << " ��������� �� ����� ���� ����������� ��������" <<endl;
					cout << "Sem error" << endl;
					local_error = true;
				}
				
				if(!local_error){

					//� ����� ������
					cur_t = next_t;
					if(!parse_token_f.eof())
						parse_token_f >> next_t; //���� ���������, �� ��������� �����
				}
			}

			if(parsing_table[cur_row].should_return){
				prev_row = cur_row; //���������� ����������
				cur_row = parsing_stack.top(); //���� ���� ����� �� ����� - ����
				parsing_stack.pop();
				change_row = true;
			}

			
			if(!change_row && parsing_table[cur_row].jump != - 1){
				cur_row = parsing_table[cur_row].jump; //���� ���� �������� - �������
			}

		}
		else{ //���� ���������� �������������
			if(parsing_table[cur_row].error){ //���� ����� ������ ��� ��� ������, �� ���������� �
				local_error = true;
				parse_error_f << "������ � ��������� " << get_token_text(cur_t) << endl;
				cout << "Synt error" << endl;

				//��� ��� 1, ������ - ����� �����������
				parse_error_f << "�������� �� ���� ����� ������ ����: ";
				do{
					for(int i = 0 ; i < parsing_table[cur_row].termenal.size(); i++){
						parse_error_f << parsing_table[cur_row].termenal[i] << " ";
					}
					cur_row--;
				}while(!parsing_table[cur_row].error);
				parse_error_f << endl;
				//��� ��� 1 - �����
			}
			else{ //���� ��� - ��������� �� ���������
				cur_row++;
			}
		}
		token_str_prev = token_str;
	};

	//��� ��� 2, ������ - ����������� �������� ����������� �����
	for(int i = 10; i < 32; i++){
		bool is_els = true; //������ �� ��������
		int j = 0; //����� �������� � �������
		lexeme check_lex; //���������� �������
		while(is_els){
			is_els = identifier.get_lexeme(i, j, check_lex);
			if(is_els){
				if(check_lex.type == 0){
					local_error = true;
					parse_error_f << "������ � ��������� �������������� " << check_lex.name << " �� �������� ���" << endl;
					cout << "Type error" << endl;
				}
				j++;
			}
		};
	}

	//��� ��� 2 - �����

	parse_token_f.close();
	parse_error_f.close();

	return !local_error;
}

void translator::grow_tree(vector<token> code){
	
	tree_el* little_tree_beg = new tree_el; //������ ���������� ������

	if(code.size() > 2){
		grow_little_tree(code, little_tree_beg); //���������� ��������� �������
		//���� ��� ����� ������ ������
		if(tree_begin == 0){
			tree_begin = little_tree_beg;
			tree_cur = tree_begin;
		}
		else{
			tree_cur->right = little_tree_beg; //� ������������ ��� � ��������
		}

		while(tree_cur->right != 0){ //���������� ������ ������
			tree_cur = tree_cur->right;
		};

	}
	
}

void translator::grow_little_tree(vector<token> code, tree_el *&beg){

	vector<token> L, R; //����� � ������ ����������

	int bracket_num = 0; //���������� ������
	int L_num = 0;

	bool flag = false;


	//���� ��������� �����
	if(code.size() == 1 && (get_token_text(code[0]) == ";" || get_token_text(code[0]) == ",")){
		beg->id = ";";
		beg->type = 2;
	}
	else{

		vector<int> opers_n; //������ "������� ��������"

		//������� ��� "������� ��������"
		for(int i = 0; i < code.size(); i++){
			if(get_token_text(code[i]) == "(") bracket_num++;
			if(get_token_text(code[i]) == ")") bracket_num--;
			if(code[i].table_n == 2 && bracket_num == 0) opers_n.push_back(i);
		}

		if(opers_n.size() == 0){ //��� �������� ��� �������� �� ��������� � ����
			flag = false;
		}
		else{ //���� ���� - ���� �������� � ���������� �����������
			int low_num = -1; //����� ������ ��� ��������
			//������� �������� �� ������� ���������
			for(int j = 0; j < opers_n.size() && low_num == -1; j++)
				if(get_token_text(code[opers_n[j]]) == "=" || get_token_text(code[opers_n[j]]) == "+=" || get_token_text(code[opers_n[j]]) == "-=" || get_token_text(code[opers_n[j]]) == "*=")
					low_num = opers_n[j];
			//����� �� + � -
			for(int j = opers_n.size()-1; j >=0 && low_num == -1; j--){
				if(get_token_text(code[opers_n[j]]) == "+" || get_token_text(code[opers_n[j]]) == "-"){
					low_num = opers_n[j]; //����� ���� ��������
				}
			}

			//���� �� ����� �������� ��������� ��������
			if(low_num == -1)
				low_num = opers_n[opers_n.size() - 1];

			//������ �������������

			if(get_token_text(code[0]) == "(") L_num++;

			//��������� ����� ���������
			for(int j = 0; L_num < low_num; L_num++, j++)
				L.push_back(code[L_num]);

			//��������� ������ ���������
			int R_num = 0;
			int l;
			if(get_token_text(code[low_num +1])=="(") R_num++;
			for(l=0, R_num = low_num +1+R_num; R_num<code.size(); R_num++, l++)
				R.push_back(code[R_num]);

			beg->left = new tree_el;
			beg->right = new tree_el; 

			beg->id = get_token_text(code[low_num]);
			beg->type = 0;

			grow_little_tree(L, beg->left);
			grow_little_tree(R, beg->right);
			flag = true;
		}

		if(!flag){
				int k = 0;
				while(get_token_text(code[k]) == "(") k++;
				beg->id = get_token_text(code[k]);
				beg->type = 1;

				if(code.size() > k+1){
					beg->right = new tree_el;
					beg->right->id = ";";
					beg->right->type = 2;
				}

			}
	}

}

void translator::out_tree(string f_name){
	ofstream out_f(f_name.c_str());

	out_tree_rec(out_f, tree_begin);

	out_f.close();
}

void translator::out_tree_rec(ofstream& out_f, tree_el *beg){
	if(beg != 0){
		if(beg->left != 0) out_tree_rec(out_f, beg->left);
		if(beg->right != 0) out_tree_rec(out_f, beg->right);

		out_f << beg->id << " ";
	}
}


void translator::generate_code(string code_file){
	ofstream code_gen_code(code_file.c_str()); //����� ��� ������ ���� � ����

	//��������� ����
	code_gen_code << ".386\n.MODEL FLAT, STDCALL\n\n";
	code_gen_code << "EXTRN\tExitProcess@4:NEAR\n";

	//��������� ���� ������

	string type_str; // ������, ����������� ��� ����������
	
	code_gen_code << "\n\n;������� ������\n.data\n";

	code_gen_code << "\ttmp_var\tdd\t?" << " ;����������, ��� �������������� �����" << endl;
	code_gen_code << "\ttmp_var_eq\tdd\t?" << " ;����������, ��� ���������" << endl;

	for(int i = 10; i < 32; i++){
		bool is_els = true; //���� �� ��������
		int j = 0; //����� �������� � �������
		lexeme check_lex; //���������� �������
		while(is_els){
			is_els = identifier.get_lexeme(i, j, check_lex);
			if(is_els){
				//��������� ���
				switch(check_lex.type){
					case 1:{
						type_str = "dd";
					} break;
					case 2:{
						type_str = "dw";
					} break;
					case 3:{
						type_str = "real8";
					}break;
				};

				//������� ���������� ���������
				code_gen_code << "\t" + check_lex.name + "\t" + type_str + " ?" << endl;
				j++;
			}
		};
	}

	//� ��� �� ������������ ���� ���������

	code_gen_code << "\n\t;������������ ���������\n";

	for(int i = 0; i < 10; i++){
		bool is_els = true; //���� �� ��������
		int j = 0; //����� �������� � �������
		lexeme check_lex; //���������� �������
		while(is_els){
			is_els = consts.get_lexeme(i, j, check_lex);
			if(is_els){
			
				if(check_lex.name.find_first_of('.') != string::npos){ // ���� ����������� ���������
					consts.set_type_address(i, j, 3);
					//��������� ��� ����������
					string tmp_str, tmp_str2, var_name;
					if(check_lex.name[0] == '-'){ //����� � ����� ���������� - �����
						tmp_str.assign(check_lex.name, 1, check_lex.name.find_first_of('.')-1);
						tmp_str = "_minus_" + tmp_str;
					}
					else
						tmp_str.assign(check_lex.name, 0, check_lex.name.find_first_of('.'));
					tmp_str2.assign(check_lex.name.begin()+check_lex.name.find_first_of('.')+1, check_lex.name.end());

					var_name = "constname" + tmp_str + tmp_str2; //��� ����������

					code_gen_code <<"\t" + var_name + "\treal8\t" + check_lex.name << endl; //������� ���� ����
					//� ���������� � map, ����� �� ������� ��� ���
					float_constants[check_lex.name] =  var_name;
				}
				else
					consts.set_type_address(i, j, 2);

				
				j++;
			}
		};
	}
		


	//� ��� �� ������� ����������� ���...

	code_gen_code << "\n\n.code\n";
	code_gen_code << "\tSTART:" << endl;
	code_gen_code << ";������������� ��������������� �� ����������\n\tfinit\n\n" << endl;

	tree_cur = tree_begin;

	while(tree_cur != NULL){
		code_gen_code << ";������ ���������\n";
		
		string tmp_str;
		int check_type = check_tree_type(tree_cur);

		tmp_str = generate_part_float(tree_cur);


		code_gen_code << tmp_str << endl;
		code_gen_code << ";����� ���������\n\n\n";

		//������������� �� ��������� ���������
		while(tree_cur->id != ";")
			tree_cur = tree_cur->right;
		tree_cur = tree_cur->right;
	};

	code_gen_code << "\tCALL ExitProcess@4\n";
	code_gen_code << "END START\n";

	code_gen_code.close();		

}



string translator::generate_part_float(tree_el *beg){

	string rez_str = "";

	if(beg->id == ";"){
		tree_cur = beg;
		return rez_str;
	}

	//==================
	//== ������������ ==
	//==================
	if(beg->id == "="){
		rez_str += generate_part_float(beg->right);

		//� ����������� �� ���� ��������� ��������� ���������� �� �����
		lexeme tmp_lex;
		identifier.get_lexeme(beg->left->id, tmp_lex);
		if(tmp_lex.type == 3)
			rez_str += "\tfstp " + beg->left->id + "\n\n";
		else
			rez_str += "\tfistp " + beg->left->id + "\n\n";

	}
	
	if(beg->id == "+="){
		rez_str += generate_part_float(beg->right);
		
		//� ����������� �� ���� ��������� ��������� ���������� �� �����
		lexeme tmp_lex;
		identifier.get_lexeme(beg->left->id, tmp_lex);
		if(tmp_lex.type == 3){
			rez_str += "\tfld " + beg->left->id + "\n";
			rez_str += "\tfadd\n";
			rez_str += "\tfstp " + beg->left->id + "\n\n";
		}
		else{
			rez_str += "\tfild " + beg->left->id + "\n";
			rez_str += "\tfadd\n";
			rez_str += "\tfistp " + beg->left->id + "\n\n";
		}
	}

	if(beg->id == "-="){
		rez_str += generate_part_float(beg->right);

		//� ����������� �� ���� ��������� ��������� ���������� �� �����
		lexeme tmp_lex;
		identifier.get_lexeme(beg->left->id, tmp_lex);
		if(tmp_lex.type == 3){
			rez_str += "\tfld " + beg->left->id + "\n";
			rez_str += "\tfsubr\n";
			rez_str += "\tfstp " + beg->left->id + "\n\n";
		}
		else{
			rez_str += "\tfild " + beg->left->id + "\n";
			rez_str += "\tfsubr\n";
			rez_str += "\tfistp " + beg->left->id + "\n\n";
		}
	}

	if(beg->id == "*="){
		rez_str += generate_part_float(beg->right);

		//� ����������� �� ���� ��������� ��������� ���������� �� �����
		lexeme tmp_lex;
		identifier.get_lexeme(beg->left->id, tmp_lex);
		if(tmp_lex.type == 3){
			rez_str += "\tfld " + beg->left->id + "\n";
			rez_str += "\tfmul\n";
			rez_str += "\tfstp " + beg->left->id + "\n\n";
		}
		else{
			rez_str += "\tfild " + beg->left->id + "\n";
			rez_str += "\tfmul\n";
			rez_str += "\tfistp " + beg->left->id + "\n\n";
		}
	}

	//==================
	//==== �������� ===
	//==================

	//��������� ��� �� ������� �����, ����� � �����, �� ������ � ����� ����� ������ ������ ��������, � �� ��� - ������
	if(beg->id == "+"){
		rez_str += generate_part_float(beg->left);
		rez_str += generate_part_float(beg->right);

		rez_str += "\tfadd\n";

	}

	if(beg->id == "-"){
		rez_str += generate_part_float(beg->left);
		rez_str += generate_part_float(beg->right);

		rez_str += "\tfsub\n";
	}

	if(beg->id == "*"){
		rez_str += generate_part_float(beg->left);
		rez_str += generate_part_float(beg->right);

		rez_str += "\tfmul\n";
	}

	//==================
	//===== ������ =====
	//==================

	if(beg->id == "=="){
		rez_str += generate_part_float(beg->left);
		rez_str += generate_part_float(beg->right);

		string mark = "jmp_mark_eq_" + beg->left->id + beg->right->id;

		rez_str += "\tfstp tmp_var_eq\n";
		rez_str += "\tfcom tmp_var_eq\n";
		rez_str += "\tfstp tmp_var_eq\n";
		rez_str += "\tfstsw ax\n\tsahf\n";
		rez_str += "\tje " + mark + "\n";

		//false
		rez_str += "\t;else\n";
		rez_str += "\t\tfldz\n";
		rez_str += "\t\tjmp " + mark + "_end\n";

		//true
		rez_str += "\t;then\n";
		rez_str += "\t" + mark + ":\n";
		rez_str += "\t\tfld1\n\n";
		rez_str += "\t" + mark + "_end:\n\n";
	}

	if(beg->id == "!="){
		rez_str += generate_part_float(beg->left);
		rez_str += generate_part_float(beg->right);

		string mark = "jmp_mark_neq_" + beg->left->id + beg->right->id;


		rez_str += "\tfstp tmp_var_eq\n";
		rez_str += "\tfcom tmp_var_eq\n";
		rez_str += "\tfstp tmp_var_eq\n";
		rez_str += "\tfstsw ax\n\tsahf\n";
		rez_str += "\tjne " + mark + "\n";

		//false
		rez_str += "\t;else\n";
		rez_str += "\t\tfldz\n";
		rez_str += "\t\tjmp " + mark + "_end\n";

		//true
		rez_str += "\t;then\n";
		rez_str += "\t" + mark + ":\n";
		rez_str += "\t\tfld1\n\n";
		rez_str += "\t" + mark + "_end:\n\n";
	}

	if(beg->id == "<"){
		rez_str += generate_part_float(beg->left);
		rez_str += generate_part_float(beg->right);

		string mark = "jmp_mark_l_" + beg->left->id + beg->right->id;

		rez_str += "\tfstp tmp_var_eq\n";
		rez_str += "\tfcom tmp_var_eq\n";
		rez_str += "\tfstp tmp_var_eq\n";
		rez_str += "\tfstsw ax\n\tsahf\n";
		rez_str += "\tjl " + mark + "\n";

		//false
		rez_str += "\t;else\n";
		rez_str += "\t\tfldz\n";
		rez_str += "\t\tjmp " + mark + "_end\n";

		//true
		rez_str += "\t;then\n";
		rez_str += "\t" + mark + ":\n";
		rez_str += "\t\tfld1\n\n";
		rez_str += "\t" + mark + "_end:\n\n";
	}

	if(beg->id == ">"){
		rez_str += generate_part_float(beg->left);
		rez_str += generate_part_float(beg->right);

		string mark = "jmp_mark_l_" + beg->left->id + beg->right->id;

		rez_str += "\tfstp tmp_var_eq\n";
		rez_str += "\tfcom tmp_var_eq\n";
		rez_str += "\tfstp tmp_var_eq\n";
		rez_str += "\tfstsw ax\n\tsahf\n";
		rez_str += "\tjb " + mark + "\n";

		//false
		rez_str += "\t;else\n";
		rez_str += "\t\tfldz\n";
		rez_str += "\t\tjmp " + mark + "_end\n";

		//true
		rez_str += "\t;then\n";
		rez_str += "\t" + mark + ":\n";
		rez_str += "\t\tfld1\n\n";
		rez_str += "\t" + mark + "_end:\n\n";
	}

	//===============================
	//=== ���������� � ��������� ====
	//===============================

	if(beg->type == 1){

		lexeme tmp_lex;
		bool is_id = identifier.get_lexeme(beg->id, tmp_lex);

		if(is_id){ //����������
			if(tmp_lex.type == 3){
				rez_str += "\tfld " + beg->id + "\n";
			}
			else{
				rez_str += "\tfild "+ beg->id + "\n";
			}
		}
		else{ //���������
			consts.get_lexeme(beg->id, tmp_lex);
			if(tmp_lex.type == 3){
				string const_name = float_constants.find(beg->id)->second; //�������� ��� ���������
				rez_str += "\tfld " + const_name + "\n";
			}
			else{
				rez_str += "\tmov tmp_var, " + beg->id + "\n";
				rez_str += "\tfild tmp_var\n";
			}
		}

	}

	return rez_str;
}

int translator::check_tree_type(tree_el *beg){

	if(beg->type == 0){
		int type_l = check_tree_type(beg->left);
		int type_r = check_tree_type(beg->right);

		if(type_l == type_r)
			return type_l;

		if(type_r == 3 || type_l == 3)
			return 3;

		if(type_r == 1 || type_l == 1)
			return 1;

		if(type_r == 2 || type_l == 2)
			return 2;
	}

	if(beg->type == 1){
		lexeme tmp_lex;
		if(identifier.get_lexeme(beg->id, tmp_lex))
			return tmp_lex.type;
		if(consts.get_lexeme(beg->id, tmp_lex))
			return tmp_lex.type;		
	}
}