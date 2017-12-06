#include<iostream>
#include<fstream>
#include<string>
#include<vector>

using namespace std;

ifstream in;
ofstream out;
char ch[100];
char ch_tmp[10];
bool IsReadNext = false;
int current_state;//当前语句标号
int current_level;//当前level
int id_level;//get_add得到的level
int id_add;//get_add得到的adr
string instruction[1000];//指令数组
string ope[6] = { "=","#","<","<=",">",">=" };

struct bgroup
{
	bool id_num;
	int sym;
	string id;
	int num;
};
bgroup* bg;
struct name_table {
	string name;
	int kind; // 0常量，1变量，2程序
	int level = 0; // 常量的value存到level里
	int adr = 3;
};
name_table temp;
vector<name_table> table;

int main_procedure();
int sect_procedure();
int const_sect();
int var_sect();
int proce_sect();
int statement();
int const_def();//暂时未用
int assign_sta();
int cond_sta();
int while_sta();
int call_sta();
int read_sta();
int write_sta();
int complex_sta();
int express_sta();//自带读取
int item();//自带读取   考虑加一个读取位
int cond();
int factor();
void err();
int get_add(string s);
string intTostring(int a) {
	if (a == 0) return "0";
	char c[11];
	c[10] = '\0';
	int i,j = 9;
	while (a % 10) {
		i = a % 10;
		a = a / 10;
		c[j--] = i + '0';
	}
	string s = "";
	while (c[++j] != '\0')
		s += c[j];
	return s;
}

int get_add(string s) {//变量1常量2
	id_level = 0;
	id_add = 0;
	for (auto it = table.begin(); it != table.end(); it++) {
		if (it->name == s) {
			id_level = it->level;
			id_add = it->adr;
			if (it->kind == 0) {
				return 2;
			}
		}
	}
	if (id_level == 0 && id_add == 0) return 0;
	return 1;
}


int get_next(ifstream& in, bgroup* bg) {
	if (in.getline(ch, 100)) {
		if (ch[2] == ',' || ch[2] == '>') bg->sym = ch[1] - '0';
		else bg->sym = (ch[1] - '0') * 10 + ch[2] - '0';
		if (bg->sym <= 50) return 1;
		if (bg->sym == 51) {
			int i;
			for (i = 0; ch[i + 4] != '>'; i++) ch_tmp[i] = ch[i + 4];
			ch_tmp[i] = '\0';
			bg->id = ch_tmp;
			bg->id_num = 0;
			return 1;
		}
		else {
			int i, tmp = 0;
			for (i = 0; ch[i + 4] != '>'; i++) tmp = tmp * 10 + ch[i + 4] - '0';
			bg->num = tmp;
			bg->id_num = 1;
			return 1;
		}
		return 1;
	}
	bg->sym = 0;//表示无statement
	return 1;

}

int read_next()
{
	if (!IsReadNext) {
		get_next(in, bg);
		IsReadNext = true;
	}
	return 1;
}

void err() {
	throw("error");
	exit(-1);
}

int main_procedure() {
	current_state = 0;
	current_level = 1;
	sect_procedure();
	read_next();
	if (bg->sym == 0) {
		instruction[current_state] = "";
		return 0;
	}
	cout << "Something left" << endl;
}

int sect_procedure() {
	read_next();
	if (bg->sym == 0) return 0;
	if (bg->sym == 1) { const_sect(); read_next(); if (bg->sym == 0) return 0; }
	if (bg->sym == 2) {
		var_sect(); 
		read_next(); 
		if (bg->sym == 0) return 0;
	}
	while (bg->sym == 3) { 
		proce_sect(); 
		read_next(); 
		if (bg->sym == 0) return 0;
	}

	if (bg->sym == 0) return 0;
	statement();
	return 0;
}

int const_sect() {
	read_next();//读过const
	int a[] = { 51,25,52 };
	do {
		if (!get_next(in, bg)) err();
		if (bg->sym != a[0]) err();
		temp.name = bg->id;
		if (!get_next(in, bg)) err();
		if (bg->sym != a[1]) err();
		if (!get_next(in, bg)) err();
		if (bg->sym != a[2]) err();
		temp.adr = bg->num;
		temp.level = current_level;
		temp.kind = 0;
		table.push_back(temp);
		if (!get_next(in, bg)) err();
	} while (bg->sym == 41);
	if (bg->sym != 42) err();
	IsReadNext = false;
	return 0;
}

int var_sect() {
	read_next();//读过var
	int adr = 3;
	temp.kind = 1;
	temp.level = current_level;
	do {
		if (!get_next(in, bg)) err();
		if (bg->sym != 51) err();
		temp.name = bg->id;
		temp.adr = adr;
		adr++;
		table.push_back(temp);
		if (!get_next(in, bg)) err();
	} while (bg->sym == 41);
	if (bg->sym != 42) err();
	IsReadNext = false;
	return 0;
}

int proce_sect() {
	if (!get_next(in, bg)) err();
	if (bg->sym != 51) err();
	temp.kind = 2;
	temp.name = bg->id;
	temp.level = current_level;
	table.push_back(temp);
	if (!get_next(in, bg)) err();
	if (bg->sym != 42) err();
	IsReadNext = false;
	current_level++;
	sect_procedure();
	current_level--;
	read_next();
	if (bg->sym != 42) err();
	IsReadNext = false;
	return 0;
}

int statement() {
	read_next();
	if (bg->sym == 4)  complex_sta();
	else if (bg->sym == 7)  call_sta(); 
	else if (bg->sym == 8) cond_sta();
	else if (bg->sym == 10)  while_sta(); 
	else if (bg->sym == 12)  read_sta();
	else if (bg->sym == 13)  write_sta();
	else if (bg->sym == 51) assign_sta();
	else if (bg->sym == 42) {//读到分号
		IsReadNext = true;
		return 1;
	}
	else if (bg->sym == 0) {//空语句
		IsReadNext = true;
		return 1;
	}
	return 0;
}

int assign_sta() {
	get_add(bg->id);
	int tmp_l = id_level;
	int tmp_a = id_add;
	if (!get_next(in, bg)) err();
	if (bg->sym != 33) err();
	if (!get_next(in, bg)) err();
	IsReadNext = true;
	express_sta();
	instruction[current_state] =  "STO " + intTostring(tmp_l) + " " + intTostring(tmp_a);
	current_state++;
	return 0;
}

int cond_sta() {
	IsReadNext = false;
	cond();
	int tmp = current_state;
	current_state++;
	read_next();
	if (bg->sym != 9) err();
	IsReadNext = false;
	statement();
	instruction[tmp] = "JPC " + intTostring(current_state);
	return 0;
}

int while_sta() {
	int tmp = current_state;
	IsReadNext = false;
	cond();
	int tmp0 = current_state++;
	read_next();
	if (bg->sym != 11) err();
	IsReadNext = false;
	statement();
	instruction[current_state++] = "JMP " + intTostring(tmp);
	instruction[tmp0] = "JPC " + intTostring(current_state);
	return 0;
}

int call_sta() {
	if (!get_next(in, bg)) err();
	if (bg->sym != 51) err();
	get_add(bg->id);
	instruction[current_state++] = "CAL " + intTostring(id_level) + " " + intTostring(id_add);
	IsReadNext = false;
	return 0;
}

int read_sta() {
	if (!get_next(in, bg)) err();
	if (bg->sym != 31) err();
	do {
		if (!get_next(in, bg)) err();
		if (bg->sym != 51) err();
		if (!get_next(in, bg)) err();
	}
	while (bg->sym == 41);
	if (bg->sym != 32) err();
	IsReadNext = false;
	return 0;
}
int write_sta() {
	if (!get_next(in, bg)) err();
	if (bg->sym != 31) err();
	do {
		if (!get_next(in, bg)) err();
		if (bg->sym != 51) err();
		if (!get_next(in, bg)) err();
	} while (bg->sym == 41);
	if (bg->sym != 32) err();
	IsReadNext = false;
	return 0;
}

int complex_sta() {
	if (!get_next(in, bg)) err();
	do {
		if (!statement()) {
			read_next();//空(其他)语句不读
			IsReadNext = false;
		}
	}
	while (bg->sym == 42);
	if (bg->sym != 5) err();
	IsReadNext = false;
	return 0;
}

int express_sta() {
	read_next();
	instruction[current_state] = "LIT 0";
	current_state++;
	string tmp = "+";
	if (bg->sym == 22 || bg->sym == 21) {
		if (bg->sym == 22)
			tmp = "-";
		if (!get_next(in, bg)) err();
	}
	IsReadNext = true;
	item();
	instruction[current_state] =  "OPE " + tmp;
	current_state++;
//	cout << "" << endl;
	read_next();
	IsReadNext = false;
	while (bg->sym == 21 || bg->sym == 22) {
		tmp = bg->sym == 21 ? "+" : "-";
		if (!get_next(in, bg)) err();
		IsReadNext = true;
		item();
		instruction[current_state] = "OPE " + tmp;
		current_state++;
		read_next();
		IsReadNext = false;
	}
	IsReadNext = true;
	return 0;
}

int item() {
	factor();
	read_next();
	string tmp = "*";
	while (bg->sym == 23 || bg->sym == 24)
	{
		tmp = bg->sym == 23 ? "*" : "/";
		if (!get_next(in, bg)) err();
		item();
		instruction[current_state] = "OPE " + tmp;
		current_state++;
		get_next(in, bg);
	}
	IsReadNext = true;
	return 0;
}

int factor() {
	read_next();
	if (bg->sym == 51 || bg->sym == 52) {
		if (bg->sym == 51) {
			if (get_add(bg->id) == 2)
				instruction[current_state] = "LIT " + intTostring(id_add);
			else
				instruction[current_state] = "LOD " + intTostring(id_level) + " " + intTostring(id_add);
			current_state++;
		}
		else {
			instruction[current_state] = "LIT " + intTostring(bg->num);
			current_state++;
		}
		IsReadNext = false;
		return 0;
	}else if (bg->sym == 31) {
		IsReadNext = false;
		express_sta();
		read_next();
		if (bg->sym != 32) err();
		IsReadNext = false;
		return 0;
	}
	else err();
	return 1;
}

int cond() {
	read_next();
	if (bg->sym == 6) {
		IsReadNext = false;
		express_sta();
	}
	else {
		string tmp;
		express_sta();
		read_next();
		if (bg->sym >= 25 && bg->sym <= 30) {
			tmp = ope[bg->sym - 25];
			if (!get_next(in, bg)) err();
		}
		else err();
		IsReadNext = true;
		express_sta();
		instruction[current_state++] = "OPE " + tmp;
	}
	return 0;
}

void print_table() {
	for (int i = 0; i < table.size(); i++) {
		temp = table[i];
		cout << temp.name << ' ' << temp.kind << ' ' << temp.level;
		if (temp.kind == 1) cout << ' ' << temp.adr;
		cout << endl;
	}
}

void print_instruction() {
	int i = 0;
	for (; instruction[i] != ""; i++)
		cout << i << "\t" << instruction[i] << endl;
}


int main() {
	in.open("obj1.txt", ios::in);
	out.open("obj2.txt", ios::out);
	bg = new bgroup();
	main_procedure();
	print_table();
	cout << "-----------------" << endl;
	print_instruction();
	system("pause");
	return 0;
}
