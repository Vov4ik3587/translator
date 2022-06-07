#include "translator.h"

translator::translator(){
	admis_chars.generate_from_file("table_chars.txt");
	numbs.generate_from_file("table_numbs.txt");
	opers_chars.generate_from_file("table_opers_chars.txt");
	opers.generate_from_file("table_opers.txt");
	key_words.generate_from_file("table_key_words.txt");
	separaters.generate_from_file("table_seps.txt");

	//Ввод таблицы разбора

	ifstream inp_tab("parsing table.txt");

	while(!inp_tab.eof()){
		synt_table_el add_el; //добавляемый эллемент
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

	string rez_str; //строка, которую мы ищим

	switch(get_t.table_n){ //ищим по таблицам
		case 2:{
			opers.get_elemen_value(get_t.numb, rez_str);
	    } break;
		case 3:{
			key_words.get_elemen_value(get_t.numb, rez_str);
		}break;
		case 4:{
			char sym;
			separaters.get_elemen_value(get_t.numb, sym);
			char s2[1]; //немного магии
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

	lex_sorce_s.open(sorce_code_file.c_str(), ios::in); //поток для работы с файлом исходного кода
	token_s.open(tokens_file.c_str(), ios::out); //поток для работы с файлом токенов
	errors_s.open(errors_file.c_str(), ios::out); //поток для работы с файлом ошибок

	string inp_str; //введёная строка
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
		bool local_error = false; //ошибка не в первом символе
		string continue_str;
		string basic_inp = inp_str;
		if(inp_str.size() > 1 && inp_str[0] == '-' && numbs.is_element_in_table(inp_str[1]))
			first_sym_type = 1;
		switch(first_sym_type){

			case -1:{ //В случае недопустимого символа
				errors_s << "В " << inp_str << " обнаруженны недопустимые символы. Первый символ недопустим." << endl;
				has_error = true;

				cout << "We have a error!" << endl;
					}break;

			case 0:{ //Если с первого символа может начинатся индентификатор
				int i;
				//Проверяем можт ли быть такой индетификатор
				for(i = 1; i < inp_str.size() && !local_error; i++){
					local_error = !(admis_chars.is_element_in_table(inp_str[i]) || numbs.is_element_in_table(inp_str[i]));
				}
				int start = i;
				if(local_error)
					start--;
				continue_str = inp_str.substr(start);
				inp_str = inp_str.substr(0, start);

				//Если всё хорошо, то проверим ключивое это имя или нет
				//Если да создадим токен и пойдём дальше, если нет - добавим в индетификаторы и пойдём дальше)
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

			case 1:{ //Если первый символ - константа
				int i;
				//Проверяем константа ли это
				for(i = 1; i < inp_str.size() && !local_error; i++){
					local_error = !(numbs.is_element_in_table(inp_str[i]) || inp_str[i] == '.'); //это либо число, либо точка
				}
				int start = i;
				if(local_error)
					start--;
				local_error = false;
				continue_str = inp_str.substr(start);
				if(continue_str.size() > 0){
					if(admis_chars.is_element_in_table(continue_str[0])){
						local_error = true;
						errors_s << "После цифры - буква, ая-яй!" << endl;
					}
				}
				inp_str = inp_str.substr(0, start);

				int points_n = inp_str.find_last_of('.') - inp_str.find_first_of('.'); //количество вхождения точек

				//У нас может быть только одна точка
				if(points_n != 0)
					local_error = true;

				//Если нет ошибок, то обновляем таблицу констант и создаём токен
				if(!local_error){
					consts.add_ind(inp_str);
					int ch_n, n;
					consts.ind_adrress(inp_str, ch_n, n);
					token_s << token(6, ch_n, n);
					local_error = !lex_string_process(continue_str);
				}			

				   }break;

			case 7:{ //Если первый символ это символ операции
				if(inp_str[0] != '/'){
					int i;
					string oper_s = inp_str.substr(0, 2);
					int oper_l;
					//Пытаемся найти нужную двух символьнуюоперацию
					local_error = !opers.is_element_in_table(oper_s);
					//Теперь - односимвольную
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
					//Если всё хорошо - создаём токен
					if(!local_error){
						int n;
						opers.get_elemen_number(oper_s, n);
						token_s << token(2, -1, n);
						continue_str = inp_str.substr(oper_l, inp_str.size()-oper_l);
						local_error = !lex_string_process(continue_str);
					}
				}
				else{ //Если есть подозрение на комментарий
					if(inp_str.size() > 1){
						switch(inp_str[1]){
						case '*':{
							string garb; //сюда будем лишние строки
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
								errors_s << "Незакрытый коментарий" << endl;
							}
								 }break;
						case '/':{
							string garb;
							getline(lex_sorce_s, garb);
								 }break;
						default:{
							errors_s << "Ошибка, символ / не предворяет комментарий" << endl;
							local_error = true;
						  }break;

					};
					}
					else{
						errors_s << "Ошибка, символ / не предворяет комментарий" << endl;
						local_error = true;
					}
				}

				   }break;
			case 4 :{ // Если разделитель

				continue_str = inp_str.substr(1);
				int n;
				separaters.get_elemen_number(inp_str[0],n);
				token_s << token(4, -1, n);
				local_error = !lex_string_process(continue_str);

					}break;
			default: {
				errors_s << "Неполучилось определить принадлежность первого символа" << endl;
				has_error = true;

				cout << "Обноруженна ошибка" << endl;
					 }break;

		};
		if(local_error){
			cout << "We have a error!" << endl;
			errors_s << "Ошибка в " << basic_inp << " , недопустимая цепочка" << endl;
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

	ifstream parse_token_f(tokens_file.c_str()); //поток для ввода токенов
	ofstream parse_error_f(errors_file.c_str()); //поток для вывода ошибок

	token cur_t, next_t; //текущий токен и следующий за ним

	bool local_error = false; //наличие ошибки

	parse_token_f >> cur_t;
	parse_token_f >> next_t;

	int cur_row = 0; //текущая строка в таблице разбора
	int prev_row; //предыдущая строка в таблице разбора

	bool have_type = false; //определяем ли мы сейчас тип
	int type_type; //опеределяемый тип

	//переменные для построения дерева разбора

	bool little_tree_bg = false; //начато ли построение малого дерева
	string token_str_prev; //терминал предыдущего токена
	int id_numb; //номер встречаемого индетификатора

	vector<token> little_tree_code; // токены для создания маленького дерева

	while(!parse_token_f.eof() && !local_error){

		string token_str = get_token_text(cur_t); // какой текст содержится в токене
		if(cur_t.table_n == 5 || cur_t.table_n == 6)
			token_str = "ID";

		if(token_str == "ID")
			little_tree_bg = true;

		bool find_terminal = false; //допустим ли данный терминал

		for(int i = 0; i < parsing_table[cur_row].termenal.size() && !find_terminal; i++){
			if(parsing_table[cur_row].termenal[i] == token_str)
				find_terminal = true;
		}

		if(find_terminal){ //если получаем то, что ожидали то обрабатываем это
	
			bool change_row = false; //сменили ли мы строку

			if(parsing_table[cur_row].put_in_stack)
				parsing_stack.push(cur_row+1); //если надо получить в стек - ложим

			if(parsing_table[cur_row].accept){ //принимаем терминал и если надо - расширяем дерево
				
				if(little_tree_bg){
					little_tree_code.push_back(cur_t);
				}

				if(token_str == ";" || token_str == ","){ //если закончили разбор цельного оператора
					grow_tree(little_tree_code); //добавили всё что нужно в дерево

					//и перешли в исходное состояние
					little_tree_code.clear();
					little_tree_bg = false;

				}
				
				//все, обнуляем типа больше нет
				if(token_str == ";"){
					have_type = false;
				}

				//Если мы нашли тип, то мы его запоминаем
				if(token_str == "int" || token_str == "char" || token_str == "float"){
					have_type = true;
					if(token_str == "int")
						type_type = 1;
					if(token_str == "char")
						type_type = 2;
					if(token_str == "float")
						type_type = 3;
				}


				//Заносим тип в таблицу идентицикаторов
				if(token_str == "ID" && have_type && cur_row == 47){
					lexeme get_lex;
					identifier.get_lexeme(get_token_text(cur_t), get_lex);
					if(get_lex.type == 0)				
						identifier.set_ind_type(get_token_text(cur_t),type_type);
					else{
						parse_error_f << "Ошибка в обработке " << get_token_text(cur_t) << " повторное объявление переменной" <<endl;
						cout << "Lex error" << endl;
						local_error = true;
					}

				}

				//Если вдруг попытались присвоить что-то константе
				if(cur_row == 30 && cur_t.table_n != 5){
					parse_error_f << "Ошибка в обработке " << get_token_text(cur_t) << " константе не может быть присовоенно значение" <<endl;
					cout << "Sem error" << endl;
					local_error = true;
				}
				
				if(!local_error){

					//и пошли дальше
					cur_t = next_t;
					if(!parse_token_f.eof())
						parse_token_f >> next_t; //если принимает, то считываем новый
				}
			}

			if(parsing_table[cur_row].should_return){
				prev_row = cur_row; //запоминаем предыдущий
				cur_row = parsing_stack.top(); //если надо взять из стека - берём
				parsing_stack.pop();
				change_row = true;
			}

			
			if(!change_row && parsing_table[cur_row].jump != - 1){
				cur_row = parsing_table[cur_row].jump; //если надо прыгнуть - прыгаем
			}

		}
		else{ //если произошщло несоответсвие
			if(parsing_table[cur_row].error){ //если можем судить что уже ошибка, то возвращаем её
				local_error = true;
				parse_error_f << "Ошибка в обработке " << get_token_text(cur_t) << endl;
				cout << "Synt error" << endl;

				//Для РГЗ 1, начало - вывод альтернатив
				parse_error_f << "Возможно на этом месте должно быть: ";
				do{
					for(int i = 0 ; i < parsing_table[cur_row].termenal.size(); i++){
						parse_error_f << parsing_table[cur_row].termenal[i] << " ";
					}
					cur_row--;
				}while(!parsing_table[cur_row].error);
				parse_error_f << endl;
				//Для РГЗ 1 - конец
			}
			else{ //Если нет - переходим на следующий
				cur_row++;
			}
		}
		token_str_prev = token_str;
	};

	//Для РГЗ 2, начало - определения проверка определения типов
	for(int i = 10; i < 32; i++){
		bool is_els = true; //еслить ли элементы
		int j = 0; //номер элемента в цепочке
		lexeme check_lex; //получаемая лексема
		while(is_els){
			is_els = identifier.get_lexeme(i, j, check_lex);
			if(is_els){
				if(check_lex.type == 0){
					local_error = true;
					parse_error_f << "Ошибка в обработке идентификатора " << check_lex.name << " не объявлен тип" << endl;
					cout << "Type error" << endl;
				}
				j++;
			}
		};
	}

	//Для РГЗ 2 - конец

	parse_token_f.close();
	parse_error_f.close();

	return !local_error;
}

void translator::grow_tree(vector<token> code){
	
	tree_el* little_tree_beg = new tree_el; //корень маленького дерева

	if(code.size() > 2){
		grow_little_tree(code, little_tree_beg); //веращиваем маленькое деревце
		//Если это самое начало дерева
		if(tree_begin == 0){
			tree_begin = little_tree_beg;
			tree_cur = tree_begin;
		}
		else{
			tree_cur->right = little_tree_beg; //и присоединяем его к большому
		}

		while(tree_cur->right != 0){ //перемещаем курсор дерева
			tree_cur = tree_cur->right;
		};

	}
	
}

void translator::grow_little_tree(vector<token> code, tree_el *&beg){

	vector<token> L, R; //левое и правое поддеревья

	int bracket_num = 0; //количество скобок
	int L_num = 0;

	bool flag = false;


	//Если последний токен
	if(code.size() == 1 && (get_token_text(code[0]) == ";" || get_token_text(code[0]) == ",")){
		beg->id = ";";
		beg->type = 2;
	}
	else{

		vector<int> opers_n; //номера "верхних операций"

		//Находим все "верхние операции"
		for(int i = 0; i < code.size(); i++){
			if(get_token_text(code[i]) == "(") bracket_num++;
			if(get_token_text(code[i]) == ")") bracket_num--;
			if(code[i].table_n == 2 && bracket_num == 0) opers_n.push_back(i);
		}

		if(opers_n.size() == 0){ //ели операций нет пропуска всё связанное с ними
			flag = false;
		}
		else{ //если есть - ищим операцию с наименьшим приоритетом
			int low_num = -1; //номер нужной нам операции
			//Сначало проверка на наличие равенства
			for(int j = 0; j < opers_n.size() && low_num == -1; j++)
				if(get_token_text(code[opers_n[j]]) == "=" || get_token_text(code[opers_n[j]]) == "+=" || get_token_text(code[opers_n[j]]) == "-=" || get_token_text(code[opers_n[j]]) == "*=")
					low_num = opers_n[j];
			//Потом на + и -
			for(int j = opers_n.size()-1; j >=0 && low_num == -1; j--){
				if(get_token_text(code[opers_n[j]]) == "+" || get_token_text(code[opers_n[j]]) == "-"){
					low_num = opers_n[j]; //нашли нашу операцию
				}
			}

			//Если не нашли выбираем последнюю операцию
			if(low_num == -1)
				low_num = opers_n[opers_n.size() - 1];

			//Привет интегрулятору

			if(get_token_text(code[0]) == "(") L_num++;

			//Формируем левое поддерево
			for(int j = 0; L_num < low_num; L_num++, j++)
				L.push_back(code[L_num]);

			//Формируем правое поддерево
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
	ofstream code_gen_code(code_file.c_str()); //поток для вывода кода в файл

	//Формируем верх
	code_gen_code << ".386\n.MODEL FLAT, STDCALL\n\n";
	code_gen_code << "EXTRN\tExitProcess@4:NEAR\n";

	//Формируем блок данных

	string type_str; // строка, описывающая тип переменной
	
	code_gen_code << "\n\n;сегмент данных\n.data\n";

	code_gen_code << "\ttmp_var\tdd\t?" << " ;переменная, для преобразования типов" << endl;
	code_gen_code << "\ttmp_var_eq\tdd\t?" << " ;переменная, для сравнения" << endl;

	for(int i = 10; i < 32; i++){
		bool is_els = true; //есть ли элементы
		int j = 0; //номер элемента в цепочке
		lexeme check_lex; //получаемая лексема
		while(is_els){
			is_els = identifier.get_lexeme(i, j, check_lex);
			if(is_els){
				//Формируем тип
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

				//Выводим объявление перменной
				code_gen_code << "\t" + check_lex.name + "\t" + type_str + " ?" << endl;
				j++;
			}
		};
	}

	//А тут мы обрабатываем наши константы

	code_gen_code << "\n\t;Вещественные константы\n";

	for(int i = 0; i < 10; i++){
		bool is_els = true; //есть ли элементы
		int j = 0; //номер элемента в цепочке
		lexeme check_lex; //получаемая лексема
		while(is_els){
			is_els = consts.get_lexeme(i, j, check_lex);
			if(is_els){
			
				if(check_lex.name.find_first_of('.') != string::npos){ // если вещственная константа
					consts.set_type_address(i, j, 3);
					//Формируем имя переменной
					string tmp_str, tmp_str2, var_name;
					if(check_lex.name[0] == '-'){ //минус в имени переменной - плохо
						tmp_str.assign(check_lex.name, 1, check_lex.name.find_first_of('.')-1);
						tmp_str = "_minus_" + tmp_str;
					}
					else
						tmp_str.assign(check_lex.name, 0, check_lex.name.find_first_of('.'));
					tmp_str2.assign(check_lex.name.begin()+check_lex.name.find_first_of('.')+1, check_lex.name.end());

					var_name = "constname" + tmp_str + tmp_str2; //имя переменной

					code_gen_code <<"\t" + var_name + "\treal8\t" + check_lex.name << endl; //выводим этот ужас
					//И запомниаем в map, чтобы не считать ещё раз
					float_constants[check_lex.name] =  var_name;
				}
				else
					consts.set_type_address(i, j, 2);

				
				j++;
			}
		};
	}
		


	//И вот мы начинам формировать код...

	code_gen_code << "\n\n.code\n";
	code_gen_code << "\tSTART:" << endl;
	code_gen_code << ";Инициализпция математического со процессора\n\tfinit\n\n" << endl;

	tree_cur = tree_begin;

	while(tree_cur != NULL){
		code_gen_code << ";Начало выражения\n";
		
		string tmp_str;
		int check_type = check_tree_type(tree_cur);

		tmp_str = generate_part_float(tree_cur);


		code_gen_code << tmp_str << endl;
		code_gen_code << ";Конец выражения\n\n\n";

		//Передвигаемся на следующее поддерево
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
	//== Присваивание ==
	//==================
	if(beg->id == "="){
		rez_str += generate_part_float(beg->right);

		//В зависимости от типа выполняем различные извлечения из стека
		lexeme tmp_lex;
		identifier.get_lexeme(beg->left->id, tmp_lex);
		if(tmp_lex.type == 3)
			rez_str += "\tfstp " + beg->left->id + "\n\n";
		else
			rez_str += "\tfistp " + beg->left->id + "\n\n";

	}
	
	if(beg->id == "+="){
		rez_str += generate_part_float(beg->right);
		
		//В зависимости от типа выполняем различные извлечения из стека
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

		//В зависимости от типа выполняем различные извлечения из стека
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

		//В зависимости от типа выполняем различные извлечения из стека
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
	//==== Операции ===
	//==================

	//Поскольку идём мы сначало влево, потом в право, то первым в стеке будет лежать второй аргумент, а за ним - первый
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
	//===== Логика =====
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
	//=== Переменные и константы ====
	//===============================

	if(beg->type == 1){

		lexeme tmp_lex;
		bool is_id = identifier.get_lexeme(beg->id, tmp_lex);

		if(is_id){ //Переменные
			if(tmp_lex.type == 3){
				rez_str += "\tfld " + beg->id + "\n";
			}
			else{
				rez_str += "\tfild "+ beg->id + "\n";
			}
		}
		else{ //Константы
			consts.get_lexeme(beg->id, tmp_lex);
			if(tmp_lex.type == 3){
				string const_name = float_constants.find(beg->id)->second; //получаем имя константы
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