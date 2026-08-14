#include <iostream>// 输入输出流
#include <map>//表驱动
#pragma warning (disable:4996)
using namespace std;
enum state { S0 = 0, ID, number, singleword, firstword, doubleword, note0, note1, note2, note3, ERR, sto, note4, note5, jump };// 状态
const int MAXROW = 20;
const int MAXCOL = 20;
const int MAX = 888;
const int keywordSum = 50;
extern int TESTscan();
int countN = 1;
int success = 0;
char filein[500] = "\0";
char fileout[500] = "\0";

state sp = S0;//前一个状态
char cn = '\0';//后一个字符
state transTable[MAXROW][MAXCOL] =
{	//0		1		2		3				4			5			6		7			8
	//空		数字		字母		单分界符			首分界符		=			/		*			sto
	{ERR,	number,	ID,		singleword,		firstword,	firstword,	note0,	singleword,	sto		},	//S0			0
	{ERR,	ID,		ID,		jump,			jump,		jump,		jump,	jump,		sto		},	//ID			1
	{ERR,	number,	ERR,	jump,			jump,		jump,		jump,	jump,		sto		},	//number		2
	{ERR,	jump,	jump,	jump,			jump,		jump,		ERR,	ERR,		sto		},	//singleword	3
	{ERR,	jump,	jump,	ERR,			ERR,		doubleword,	ERR,	ERR,		sto		},	//firstword		4
	{ERR,	jump,	jump,	jump,			ERR,		jump,		ERR,	ERR,		sto		},	//doubleword	5
	{ERR,	ERR,	ERR,	ERR,			ERR,		ERR,		note4,	note1,		sto		},	//note0			6
	{note1,	note1,	note1,	note1,			note1,		note1,		note1,	note2,		note1	},	//note1			7
	{note1,	note1,	note1,	note1,			note1,		note1,		note3,	note2,		note1	},	//note2			8
	{ERR,	jump,	jump,	jump,			jump,		jump,		note0,	jump,		sto		},	//note3			9
	{ERR,	ERR,	ERR,	ERR,			ERR,		ERR,		ERR,	ERR,		sto		},	//ERR			10
	{ERR,	ERR,	ERR,	ERR,			ERR,		ERR,		ERR,	ERR,		sto		},	//sto			11
	{note4,	note4,	note4,	note4,			note4,		note4,		note5,	note4,		note4	},	//note4			12
	{note4,	note4,	note4,	note4,			note4,		note4,		ERR,	note4,		note4	},	//note5			13
	{ERR,	jump,	jump,	jump,			jump,		jump,		jump,	jump,		sto	}		//jump			14

};

map<char, int> alphabet;

FILE* fin, * fout;
char c;
char keyword[keywordSum][50] = { "if","else","for","while","do","int","write","read","return","call"};
void tableDrive();    // 表驱动函数
state F(state, char);    // 状态转换函数
void initialization();//初始化字母表
int TESTscan()
{
	cout << "请输入待词法分析的txt文件名(无需后缀)：";
	gets_s(filein);
	strcat(filein, ".txt");
//	strcpy(filein, "TESTinD.txt");
	if ((fin = fopen(filein, "r")) == NULL) {
		cout << "\n文件打开失败\n";
		return(1);
	}
	strcpy(fileout, "Testout.txt");
	if ((fout = fopen(fileout, "w")) == NULL) {
		cout << "\n文件写入失败\n";
		return(2);
	}
	initialization();
	while (c != EOF) {
		tableDrive();
	}
	fclose(fin);
	fclose(fout);
	return success;

}
void initialization() {
	for (char n = '0'; n <= '9'; n++) {
		alphabet[n] = 1;
	}
	for (char n = 'a'; n <= 'z'; n++) {
		alphabet[n] = 2;
	}
	for (char n = 'A'; n <= 'Z'; n++) {
		alphabet[n] = 2;
	}
	alphabet['+'] = 3;
	alphabet['-'] = 3;
	alphabet[':'] = 3;
	alphabet['('] = 3;
	alphabet[')'] = 3;
	alphabet['['] = 3;
	alphabet[']'] = 3;
	alphabet['{'] = 3;
	alphabet['}'] = 3;
	alphabet[','] = 3;
	alphabet[';'] = 3;

	alphabet['>'] = 4;
	alphabet['<'] = 4;
	alphabet['!'] = 4;

	alphabet['='] = 5;

	alphabet['/'] = 6;

	alphabet['*'] = 7;

	alphabet[' '] = 8;
	alphabet['\t'] = 8;
}
void tableDrive()
{
	int cNadd = 0;
	state s = S0;
	char string[1000] = "\0";
	int i = 0;
	for (i = 0; i < MAX; i++) {
		if (sp == jump)
			c = cn;
		else {
			c = getc(fin);
			if (c == '\n') {
				cNadd = 1;
				break;
			}
		}
		sp = s;
		if (c == EOF) {
			break;
		}
		s = F(s, c);
		if ((sp == note4 || sp == note5) && c == '\n') {
			break;
		}
		if (s == ERR) {
			while (alphabet[c] != 8 && c != EOF) {
				string[i++] = c;
				c = getc(fin);
			}
			break;
		}
		if (s == sto) {
			s = sp;
			break;
		}
		if (s == jump) {
			cn = c;
			s = sp;
			sp = jump;
			break;
		}
		string[i] = c;
	}
	char des[50] = "\0";
	char YoN[10] = "\0";
	strcpy(YoN, "[○]");
	switch (s) {
	case S0:strcat(des, "空");
		return;
		break;
	case ID: {
		char temString[1000];
		strcpy(temString, string);
		int judge = 0;
		for (int j = 0; j < keywordSum; j++) {
			if (strcmp(strlwr(temString), keyword[j]) == 0) {
				//				strcat(des, "keyword");
				strcat(des, temString);
				judge = 1;
				break;
			}
		}
		if (judge == 0)
			strcat(des, "ID");
		break;
	}
	case number:strcat(des, "number");
		break;
	case singleword:
		//		strcat(des, "singleword");
		strcat(des, string);
		break;
	case firstword:strcat(des, string);
		break;
	case doubleword:strcat(des, string);
		break;
	case note0:
		//		strcat(des, "singleword");
		strcat(des, string);
		break;
	case note1:strcat(des, "未查找到注释尾");
		break;
	case note2:strcat(des, string);
		break;
	case note3:
	case note4:
	case note5:
		strcat(des, "注释");
		break;
	case ERR:
		strcpy(YoN, "[×]");
		switch (sp)
		{
		case ID:
			strcat(des, "ID出现非法字符");
			success = 1;
			break;
		case number:
			strcat(des, "数字中含有非数字或ID以数字开头");
			success = 2;
			break;
		case note3:
		case note5:
			strcat(des, "注释嵌套");
			success = 3;
			break;
		case singleword:
			strcat(des, "连续使用单分界符或注释嵌套");
			success = 4;
			break;
		case S0:
			strcat(des, "非法字符");
			success = 5;
			break;
		default:
			return;
			break;
		}
		break;
	defualt: strcpy(des, "ERROR");
	}
	printf("%-5d%s%-40s\t%-20s\n", countN, YoN, des, string);
	if (s == note3 || s == note4 || s == note5) {
		countN += cNadd;
		return;
	}
	else {
		fprintf(fout, "%-5d %s %s\n", countN, des, string);
		countN += cNadd;
	}
}

state F(state s, char c)//  s为当前状态, c为当前字符
{
	state ret;
	int col = alphabet[c];    // 如果c不在字母表,map为0
	ret = transTable[s][col];
	return ret;
}
