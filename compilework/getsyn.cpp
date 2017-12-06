#include<iostream>
#include<fstream>
#include<string>

using namespace std;

const int maxl = 10;//标识符最大长度

struct bgroup
{
	bool id_num;
	int sym;
	string id;
	int num;
};

string* sym_map = new string[13]{ "const","var","procedure","begin","end","odd","call","if","then","while","do","read","write" };//关键字映射
string* ope_map = new string[13]{ "+","-","*","/","=","#","<","<=",">",">=","(",")" ,":=" };
string* dem_map = new string[2]{ ",",";" };
string* dou_ope = new string[3]{ ">=","<=",":=" };
string str;
bgroup g;

//判断字母
int IsAlpha(char c) {
	if ((c >= 'a'&&c <= 'z') || (c >= 'A'&&c <= 'Z')) return 1;
	return 0;
}
//判断字母或数据
int IsId(string s) {
	if (!IsAlpha(s[0])) return 0;
	for (int i = 1; i < s.length(); i++) {
		if (IsAlpha(s[i]) || (s[i] >= '0'&&s[i] <= '9'));
		else return 0;
	}
	return 1;
}

int IsNum(string s) {
	for (int i = 0; i < s.length(); i++) {
		if (s[i]<'0'&&s[i]>'9') return 0;
	}
	return 1;
}
//分隔符
int IsSep(char c) {
	str = c;
	for (int i = 0; i < 13; i++) {
		if (str == ope_map[i])  return 1; 
	}
	for (int i = 0; i < 2; i++) {
		if (str == dem_map[i])  return 1;
	}
	return c == ':';
	
}

//关键字1-20，运算符21-40，界符41-50，标识符51数字52
void judge(string s, bgroup* g) {
	int i;
	for (i = 0; i < 13; i++) {
		if (s == sym_map[i]) { g->sym = i + 1; return; }
	}
	for (i = 0; i < 13; i++) {
		if (s == ope_map[i]) { g->sym = i + 21; return; }
	}
	for (i = 0; i < 2; i++) {
		if (s == dem_map[i]) { g->sym = i + 41; return; }
	} 
	if (IsId(s)) {
		g->id_num = 0;
		g->sym = 51;
		g->id = s;
		return;
	}
	if (IsNum(s)) {
		g->id_num = 1;
		g->sym = 52;
		g->num = 0;
		for (i = 0; i < s.length(); i++)
			g->num = g->num * 10 + (s[i] - '0');
		return;
	}
	
}

void printstr(char* ch,ofstream& out) {
	str = ch;
	judge(str, &g);
	if (g.sym <= 50) out << "<" << g.sym << ">" << endl;
	else
		if (g.id_num) out << "<" << g.sym << "," << g.num << ">" << endl;
		else out << "<" << g.sym << "," << g.id << ">" << endl;
}

int main() {
	ifstream in;
	in.open("source.txt", ios::in);
	ofstream out;
	out.open("obj1.txt", ios::out);
	char c = '\0';
	char char_array[maxl * 100];
	char ch[maxl];
	int i = 0,j = 0,a = 0;
	while (in.getline(char_array, 100 * maxl)) {
		for (i = 0; (i < 100 * maxl) && char_array[i]; i++) {
			if (char_array[i] == ' ') {
				if (ch[0]) {
					ch[j] = '\0';
					printstr(ch, out);
					ch[0] = '\0'; j = 0;
				}
				continue;
			}
			if (IsSep(char_array[i])) {
				if (ch[0]) {
					ch[j] = '\0';
					printstr(ch, out);
					ch[0] = '\0'; j = 0;
				}
				ch[0] = char_array[i]; ch[1] = char_array[i + 1]; ch[2] = '\0';
				str = ch;
				a = 0;
				for (int m = 0; m < 3; m++) {
					if (str == dou_ope[m]) {
						a = 1;
						break;
					}
				}
				if(a) {
					printstr(ch, out);
					i += 1;
				}
				else {
					ch[1] = '\0';
					printstr(ch, out);
				}
				ch[0] = '\0'; j = 0;
				continue;
			}
			ch[j++] = char_array[i];
		}
		if (ch[0]) {
			ch[j] = '\0';
			printstr(ch, out);
			ch[0] = '\0'; j = 0;
		}
	}





/*	while (!in.eof()) {
		for (i = 0; i < maxl; i++) ch[i] = '\0';//临时字符变量清掉
		if (!in.get(c)) break;
		while ((!in.eof()) && (c == ' ' || c == '\n'))
			in.get(c);//去空格
		if (in.eof()) break;
		for (i = 0; i <= maxl; i++) {
			if (i == maxl) {//防止字符超过长度
				throw("String is too long!");
				exit(0);
			}
			if (IsSep(c)) {
				if(ch[0])
					printstr(ch, out);
				ch[0] = c; ch[1] = '\0';
				printstr(ch, out);
				break;
			}
			if (c == ' ' || c == '\n') {
				printstr(ch, out);
				break;
			}
			ch[i] = c;
			if (!in.get(c)) //防结尾				
				break;
		}
		
	} 
	*/
//	std::system("pause");
	return 0;
}