#define _CRT_SECURE_NO_WARNINGS
#pragma warning (disable:4996)
//递归下降法实现
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<stdlib.h>
#define maxTree 100
#define maxsymbolIndex 100

using namespace std;
typedef struct Tree {
	char elem[20];
	int layer;			//递归层数
	int parent_P;		//父节点的位置
}Tree;
Tree tree[300];

int Tree_num = 0;		//记录当前树的元素个数
int symbolPos_all = 0;
int exchange;			//用于存储传递的参数
FILE* fpTokenin;		//单词流文件
FILE* fpTreeout;		//语法树文件
FILE* fpCodeBinary;
FILE* fpCodeText;
char tokenfile[30];		//记录输入文件名字
char token[10];			//单词类别值
char token1[20];		//单词自身值
enum Category_symbol { variable, function };	//标志符的类型

struct
{
	char name[20];
	enum Category_symbol kind;
	int address;
	char function_area[20];
}symbol[maxsymbolIndex];
typedef struct Code {
	char opt[50];			//操作码
	int operand;			//操作数
}Code;
Code codes[150];

int codesIndex;			//code数组下标
int symbolIndex = 0;	//标识符在符号表中的位置
int offset;				//局部变量在函数中的相对地址
int linecode = 1;		//code行数
typedef struct Error {
	int E_es;
	int E_linecode;
}Error;
Error error[50];
int error_count = 0;

int insert_Symbol(enum Category_symbol category, const char* name);	//插入符号表函数
int lookup(char* name, int* pPosition);							//查符号表函数
int compile();
int  program();
int fun_declaration(int layer, int parent);
int main_declaration(int layer, int parent);
int function_body(int layer, int parent);
int declaration_list(int layer, int parent);
int declaration_stat(int layer, int parent);
int statement_list(int layer, int parent);
int statement(int layer, int parent);
int if_stat(int layer, int parent);
int while_stat(int layer, int parent);
int for_stat(int layer, int parent);
int write_stat(int layer, int parent);
int read_stat(int layer, int parent);
int expression(int layer, int parent);
int compound_stat(int layer, int parent);
int call_stat(int layer, int parent);
int expression_stat(int layer, int parent);
int bool_expr(int layer, int parent);
int additive_expr(int layer, int parent);
int term(int layer, int parent);
int factor(int layer, int parent);
int switch_stat(int layer, int parent);
int do_while_stat(int layer, int parent);
void case_list(int es);
//void tree_print();

int main() {
	/*if (compile()) {
		tree_print();
	}*/
	compile();
	system("pause");
	return 0;
}

void case_list(int es) {
	switch (es)
	{
	case 0: printf("语法分析成功\n"); break;
	case 1: printf("缺少main\n"); break;
	case 2: printf("main函数后面还有未结束的语句\n"); break;
	case 3: printf("缺少变量名\n"); break;
	case 4: printf("缺少(\n"); break;
	case 5: printf("缺少)\n"); break;
	case 6: printf("缺少{\n"); break;
	case 7: printf("缺少}\n"); break;
	case 8: printf("缺少标识符\n"); break;
	case 9: printf("缺少分号\n"); break;
	case 10: printf("缺少操作数\n"); break;
	case 11: printf("最后一个函数名字应该是main\n"); break;
	case 12: printf("缺少case\n"); break;
	case 13: printf("缺少:\n"); break;
	case 14: printf("case后应该是常量\n"); break;
	case 15: printf("缺少break\n"); break;
	case 16: printf("缺少while\n"); break;
	case 17: printf("缺少函数名\n"); break;
	case 18: printf("符号表溢出\n"); break;

	case 19: printf("函数名重定义\n"); break;
	case 20: printf("变量重定义\n"); break;
	case 21: printf("标识符没定义\n"); break;
	case 22: printf("read后面没有接变量名\n"); break;
	case 23: printf("call后面没有接函数名\n"); break;
	case 24: printf("赋值语句左部不是变量名\n"); break;
	case 25: printf("因子对应的标志符不是变量名\n"); break;
	case 26: printf("switch后面括号中应该写入变量名\n"); break;
	case 27: printf("使用不在此函数定义的局部变量\n"); break;
	default:
		break;
	}
}

int compile() {
	int es = 0;			//记录出错信息
//	printf("请输入单词流文件名:");
//	scanf("%s", tokenfile);
	strcpy(tokenfile, "Testout1.txt");
	fpTokenin = fopen(tokenfile, "r");

	es = program();
	fclose(fpTokenin);

	printf("-----语法分析结果-----\n");
	if (error_count == 0) {
		es = 0;
		case_list(es);
	}
	else {
		while (error_count) {
			error_count--;
			printf("错误在程序中的第%d行,错误原因:", error[error_count].E_linecode);
			case_list(error[error_count].E_es);
		}
		goto l1;
	}
	printf("输入生成中间代码的文件名：");
	scanf("%s", tokenfile);
	fpCodeText = fopen(tokenfile, "w");
	for (int i = 0; i < codesIndex; i++) {
		if (!strcmp(codes[i].opt, "BR") || !strcmp(codes[i].opt, "LOAD") || !strcmp(codes[i].opt, "LOADI") || !strcmp(codes[i].opt, "STO") || !strcmp(codes[i].opt, "BRF") || !strcmp(codes[i].opt, "CAL") || !strcmp(codes[i].opt, "ENTER") || !strcmp(codes[i].opt, "DADD")) {
			fprintf(fpCodeText, "%-5s %d\n", codes[i].opt, codes[i].operand);
		}
		else
			fprintf(fpCodeText, "%-5s\n", codes[i].opt);	//无操作数
	}
	fclose(fpCodeText);
	printf("输入生成二进制形式中间代码的文件名:");
	scanf("%s", tokenfile);
	fpCodeBinary = fopen(tokenfile, "wb");
	fwrite(codes, sizeof(Code), codesIndex, fpCodeBinary);
	fclose(fpCodeBinary);
	if (es == 0) {
		return 1;
	}
l1:	return 0;
}
int  program() {
	int es = 0;
	int tmp;
	tmp = Tree_num;
	strcat(tree[Tree_num].elem, "<program>");
	tree[Tree_num].layer = 0;
	Tree_num++;
	strcpy(codes[codesIndex++].opt, "BR");
	fscanf(fpTokenin, "%s %s\n", token, token1);
	//不同函数定义类型值为function
	while (!strcmp(token, "function")) {
		//普通函数
		fscanf(fpTokenin, "%s %s\n", token, token1);
		es = fun_declaration(tree[tmp].layer, tmp);
		fscanf(fpTokenin, "%s %s\n", token, token1);
	}
	if (strcmp(token, "ID")) {
		es = 1;
		return es;
	}
	if (strcmp(token1, "main")) {
		es = 11;
		return es;
	}
	fscanf(fpTokenin, "%s %s\n", token, token1);
	es = main_declaration(tree[tmp].layer, tmp);
	//程序还有未结束的语句
	if (!feof(fpTokenin)) return (es = 2);
	printf("符号表\n");
	printf("名字 \t类型 \t\t地址\t作用域\n");
	for (int i = 0; i < symbolIndex; i++) {
		if (symbol[i].kind == 1) {
			printf("%s \tfunction \t%d\t%s\n", symbol[i].name, symbol[i].address, "----");
		}
		else {
			printf("%s \tvarible \t%d\t%s\n", symbol[i].name, symbol[i].address, symbol[i].function_area);
		}
	}
	return es;
}

int fun_declaration(int layer, int parent) {
	int es = 0;
	int tmp;
	tmp = Tree_num;
	offset = 2;
	layer++;
	strcat(tree[Tree_num].elem, "<fun_declaration>");
	tree[Tree_num].layer = layer;
	tree[Tree_num].parent_P = parent;
	Tree_num++;
	strcat(tree[Tree_num].elem, "function");
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;
	if (strcmp(token, "ID")) {
		es = 17;
		return es;
	}
	insert_Symbol(function, token1);
	//printf("%d", symbol[0].kind);
	strcat(tree[Tree_num].elem, token);
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;
	fscanf(fpTokenin, "%s %s\n", token, token1);
	if (strcmp(token, "(")) {
		es = 4;
		return 4;
	}
	strcat(tree[Tree_num].elem, token);
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;
	fscanf(fpTokenin, "%s %s\n", token, token1);
	if (!strcmp(token, "int")) {
		fscanf(fpTokenin, "%s %s\n", token, token1);
		if (strcmp(token, "ID")) return (es = 8);
		insert_Symbol(variable, token1);
		fscanf(fpTokenin, "%s %s\n", token, token1);
	}
	if (strcmp(token, ")")) {
		es = 5;
		return es;
	}
	strcat(tree[Tree_num].elem, token);
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;
	fscanf(fpTokenin, "%s %s\n", token, token1);
	es = function_body(layer, tmp);
	return es;
}

int main_declaration(int layer, int parent) {
	int es = 0;
	insert_Symbol(function, "main");
	int tmp;
	layer++;
	tmp = Tree_num;
	strcat(tree[Tree_num].elem, "<main_declaration>");
	tree[Tree_num].layer = layer;
	tree[Tree_num].parent_P = parent;
	Tree_num++;
	strcat(tree[Tree_num].elem, "main");
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;
	if (strcmp(token, "(")) {
		es = 4;
		return es;
	}
	strcat(tree[Tree_num].elem, "(");
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;
	fscanf(fpTokenin, "%s %s\n", token, token1);
	if (strcmp(token, ")")) {
		es = 5;
		return es;
	}

	codes[0].operand = codesIndex;		//反填main函数的入口地址

	strcat(tree[Tree_num].elem, ")");
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;
	fscanf(fpTokenin, "%s %s\n", token, token1);
	es = function_body(layer, tmp);
	return es;
}

int function_body(int layer, int parent) {
	int es = 0;
	int tmp;
	layer++;
	tmp = Tree_num;
	strcpy(tree[Tree_num].elem, "<function_body>");
	tree[Tree_num].layer = layer;
	tree[Tree_num].parent_P = parent;
	Tree_num++;
	if (strcmp(token, "{")) {
		es = 6;
	}
	linecode++;
	if (!strcmp(symbol[symbolIndex - 1].name, "main")) {
		symbol[symbolIndex - 1].address = codesIndex;
		offset = 2;
	}
	else {
		symbol[symbolIndex - 2].address = codesIndex;		//ID/main的地址值
		offset = 3;
	}
	strcpy(tree[Tree_num].elem, "{");
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;
	fscanf(fpTokenin, "%s %s\n", token, token1);
	es = declaration_list(layer, tmp);
	strcpy(codes[codesIndex].opt, "ENTER");
	codes[codesIndex++].operand = offset;

	if (strcmp(symbol[symbolIndex - 4].name, "main")) {
		strcpy(codes[codesIndex++].opt, "GET");
		strcpy(codes[codesIndex].opt, "STO");
		codes[codesIndex++].operand = 2;
	}


	es = statement_list(layer, tmp);
	if (strcmp(token, "}")) {
		es = 7;
		return 7;
	}
	linecode++;
	strcat(tree[Tree_num].elem, "}");
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;
	strcpy(codes[codesIndex++].opt, "RETURN");
	return es;
}

int declaration_list(int layer, int parent) {
	int es = 0;
	int tmp;
	layer++;
	tmp = Tree_num;
	strcpy(tree[Tree_num].elem, "<declaration_list>");
	tree[Tree_num].layer = layer;
	tree[Tree_num].parent_P = parent;
	Tree_num++;
	while (!strcmp(token, "int")) {
		fscanf(fpTokenin, "%s %s\n", token, token1);
		es = declaration_stat(layer, tmp);
		fscanf(fpTokenin, "%s %s\n", token, token1);
	}
	return es;
}

int declaration_stat(int layer, int parent) {		//声明表达式
	int es = 0;
	int tmp;
	layer++;
	tmp = Tree_num;
	strcpy(tree[Tree_num].elem, "<declaration_stat>");
	tree[Tree_num].layer = layer;
	tree[Tree_num].parent_P = parent;
	Tree_num++;
	strcat(tree[Tree_num].elem, "int");
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;
	if (strcmp(token, "ID")) return (es = 8);

	es = insert_Symbol(variable, token1);

	strcat(tree[Tree_num].elem, "ID");
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;
	fscanf(fpTokenin, "%s %s\n", token, token1);
	if (strcmp(token, ";")) return (es = 9);
	linecode++;
	strcat(tree[Tree_num].elem, ";");
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;
	return es;
}

int statement_list(int layer, int parent) {
	int es = 0;
	int tmp;
	layer++;
	tmp = Tree_num;
	strcpy(tree[Tree_num].elem, "<statement_list>");
	tree[Tree_num].layer = layer;
	tree[Tree_num].parent_P = parent;
	Tree_num++;
	while (strcmp(token, "}")) {
		es = statement(layer, tmp);
	}
	return es;
}

int statement(int layer, int parent) {
	Tree_num = 0;
	int es = 0;
	int tmp;
	layer++;
	tmp = Tree_num;
	strcpy(tree[Tree_num].elem, "<statement>");
	tree[Tree_num].layer = layer;
	tree[Tree_num].parent_P = parent;
	Tree_num++;
	if (es == 0 && !strcmp(token, "if")) es = if_stat(layer, tmp);
	if (es == 0 && !strcmp(token, "while")) es = while_stat(layer, tmp);
	if (es == 0 && !strcmp(token, "for")) es = for_stat(layer, tmp);
	if (es == 0 && !strcmp(token, "read")) es = read_stat(layer, tmp);
	if (es == 0 && !strcmp(token, "write")) es = write_stat(layer, tmp);
	if (es == 0 && !strcmp(token, "{")) es = compound_stat(layer, tmp);
	if (es == 0 && !strcmp(token, "call")) es = call_stat(layer, tmp);
	if (es == 0 && !strcmp(token, "switch")) es = switch_stat(layer, tmp);
	if (es == 0 && !strcmp(token, "do")) es = do_while_stat(layer, tmp);
	if (es == 0 && (!strcmp(token, "ID") || !strcmp(token, "NUM") || !strcmp(token, "("))) es = expression_stat(layer, tmp);
	return es;
}

int do_while_stat(int layer, int parent) {
	int es = 0;
	int cxEntrance;
	int cx1;
	int tmp;
	layer++;
	tmp = Tree_num;
	strcpy(tree[Tree_num].elem, "<do_while_list>");
	tree[Tree_num].layer = layer;
	tree[Tree_num].parent_P = parent;
	Tree_num++;
	fscanf(fpTokenin, "%s %s\n", token, token1);
	if (strcmp(token, "{")) return (es = 6);
	strcat(tree[Tree_num].elem, token);
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = parent;
	Tree_num++;

	cxEntrance = codesIndex;

	fscanf(fpTokenin, "%s %s\n", token, token1);
	es = statement(layer, tmp);
	if (strcmp(token, "}")) return (es = 7);
	strcat(tree[Tree_num].elem, token);
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = parent;
	Tree_num++;
	fscanf(fpTokenin, "%s %s\n", token, token1);
	if (strcmp(token, "while")) return (es = 16);
	strcat(tree[Tree_num].elem, token);
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = parent;
	Tree_num++;
	fscanf(fpTokenin, "%s %s\n", token, token1);
	if (strcmp(token, "(")) return (es = 4);
	strcat(tree[Tree_num].elem, token);
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = parent;
	Tree_num++;
	fscanf(fpTokenin, "%s %s\n", token, token1);
	es = expression(layer, tmp);

	strcpy(codes[codesIndex].opt, "BRF");
	cx1 = codesIndex++;
	strcpy(codes[codesIndex].opt, "BR");
	codes[codesIndex++].operand = cxEntrance;
	codes[cx1].operand = codesIndex;

	if (strcmp(token, ")")) return (es = 5);
	strcat(tree[Tree_num].elem, token);
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = parent;
	Tree_num++;
	fscanf(fpTokenin, "%s %s\n", token, token1);
	if (strcmp(token, ";")) return (es = 9);
	strcat(tree[Tree_num].elem, token);
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = parent;
	Tree_num++;
	fscanf(fpTokenin, "%s %s\n", token, token1);
	return es;
}
int switch_stat(int layer, int parent) {
	int es = 0;
	int tmp;
	int flag = 0;
	int cx1[20], cx2[20];
	int cx_count = 0;
	int symbolPos;
	layer++;
	tmp = Tree_num;
	strcpy(tree[Tree_num].elem, "<switch_list>");
	tree[Tree_num].layer = layer;
	tree[Tree_num].parent_P = parent;
	Tree_num++;
	fscanf(fpTokenin, "%s %s\n", token, token1);
	if (strcmp(token, "(")) return (es = 4);
	strcat(tree[Tree_num].elem, token);
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = parent;
	Tree_num++;
	fscanf(fpTokenin, "%s %s\n", token, token1);
	if (strcmp(token, "ID")) return (es = 8);

	es = lookup(token1, &symbolPos);
	if (es == -1) {
		es = 0;
		if (!symbol[symbolPos].kind == variable) {
			error[error_count].E_es = 26;
			error[error_count++].E_linecode = linecode;
		}
	}

	strcat(tree[Tree_num].elem, token);
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = parent;
	Tree_num++;
	fscanf(fpTokenin, "%s %s\n", token, token1);
	if (strcmp(token, ")")) return (es = 5);
	strcat(tree[Tree_num].elem, token);
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = parent;
	Tree_num++;
	fscanf(fpTokenin, "%s %s\n", token, token1);
	if (strcmp(token, "{")) return (es = 6);
	strcat(tree[Tree_num].elem, token);
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = parent;
	Tree_num++;
	fscanf(fpTokenin, "%s %s\n", token, token1);
	if (strcmp(token, "case")) return (es = 12);
	do {
		if (flag == 1) {
			for (int i = 0; i < cx_count; i++) {
				codes[cx1[i]].operand = codesIndex;
			}
		}
		flag = 1;
		strcat(tree[Tree_num].elem, token);
		tree[Tree_num].layer = layer + 1;
		tree[Tree_num].parent_P = parent;
		Tree_num++;
		fscanf(fpTokenin, "%s %s\n", token, token1);
		if (strcmp(token, "NUM")) return (es = 14);

		strcpy(codes[codesIndex].opt, "LOAD");
		codes[codesIndex++].operand = symbol[symbolPos].address;
		strcpy(codes[codesIndex].opt, "LOADI");
		codes[codesIndex++].operand = atoi(token1);
		strcpy(codes[codesIndex++].opt, "EQ");
		strcpy(codes[codesIndex].opt, "BRF");
		cx1[cx_count] = codesIndex++;

		strcat(tree[Tree_num].elem, token);
		tree[Tree_num].layer = layer + 1;
		tree[Tree_num].parent_P = parent;
		Tree_num++;
		fscanf(fpTokenin, "%s %s\n", token, token1);
		if (strcmp(token, ":")) return (es = 13);
		strcat(tree[Tree_num].elem, token);
		tree[Tree_num].layer = layer + 1;
		tree[Tree_num].parent_P = parent;
		Tree_num++;
		fscanf(fpTokenin, "%s %s\n", token, token1);
		es = statement(layer, tmp);

		strcpy(codes[codesIndex].opt, "BR");
		cx2[cx_count] = codesIndex++;

		if (strcmp(token, "break")) return (es = 15);
		strcat(tree[Tree_num].elem, token);
		tree[Tree_num].layer = layer + 1;
		tree[Tree_num].parent_P = parent;
		Tree_num++;
		fscanf(fpTokenin, "%s %s\n", token, token1);
		if (strcmp(token, ";")) return (es = 9);
		strcat(tree[Tree_num].elem, token);
		tree[Tree_num].layer = layer + 1;
		tree[Tree_num].parent_P = parent;
		Tree_num++;
		fscanf(fpTokenin, "%s %s\n", token, token1);
		cx_count++;
	} while (!strcmp(token, "case"));

	codes[cx1[cx_count - 1]].operand = codesIndex;

	if (strcmp(token, "default")) return (es = 7);
	fscanf(fpTokenin, "%s %s\n", token, token1);
	if (strcmp(token, ":")) return (es = 13);
	fscanf(fpTokenin, "%s %s\n", token, token1);
	es = statement(layer, tmp);
	if (strcmp(token, "break")) return (es = 15);
	strcat(tree[Tree_num].elem, token);
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = parent;
	Tree_num++;
	fscanf(fpTokenin, "%s %s\n", token, token1);
	if (strcmp(token, ";")) return (es = 9);
	strcat(tree[Tree_num].elem, token);
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = parent;
	Tree_num++;
	fscanf(fpTokenin, "%s %s\n", token, token1);
	if (strcmp(token, "}")) return (es = 7);
	strcat(tree[Tree_num].elem, token);
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = parent;
	Tree_num++;

	for (int i = 0; i < cx_count; i++) {
		codes[cx2[i]].operand = codesIndex;
	}

	fscanf(fpTokenin, "%s %s\n", token, token1);
	return es;
}

int if_stat(int layer, int parent) {
	int es = 0;
	int cx1, cx2;
	int tmp;
	layer++;
	tmp = Tree_num;
	strcpy(tree[Tree_num].elem, "<if_stat>");
	tree[Tree_num].layer = layer;
	tree[Tree_num].parent_P = parent;
	Tree_num++;
	strcpy(tree[Tree_num].elem, "if");
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;
	fscanf(fpTokenin, "%s %s\n", token, token1);
	if (strcmp(token, "(")) return (es = 4);
	strcat(tree[Tree_num].elem, "(");
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;
	fscanf(fpTokenin, "%s %s\n", token, token1);
	es = expression(layer, tmp);
	if (strcmp(token, ")")) return (es = 5);

	strcpy(codes[codesIndex].opt, "BRF");
	cx1 = codesIndex++;						//保存假转位置

	strcat(tree[Tree_num].elem, ")");
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;
	fscanf(fpTokenin, "%s %s\n", token, token1);
	es = statement(layer, tmp);

	strcpy(codes[codesIndex].opt, "BR");
	cx2 = codesIndex++;
	codes[cx1].operand = codesIndex;

	if (!strcmp(token, "else")) {
		strcat(tree[Tree_num].elem, "else");
		tree[Tree_num].layer = layer + 1;
		tree[Tree_num].parent_P = tmp;
		Tree_num++;
		fscanf(fpTokenin, "%s %s\n", token, token1);
		es = statement(layer, tmp);
	}
	codes[cx2].operand = codesIndex;
	return es;
}

int while_stat(int layer, int parent) {
	int es = 0;
	int cx1;
	int cxEntrance;
	int tmp;
	layer++;
	tmp = Tree_num;
	strcpy(tree[Tree_num].elem, "<while_stat>");
	tree[Tree_num].layer = layer;
	tree[Tree_num].parent_P = parent;
	Tree_num++;
	strcpy(tree[Tree_num].elem, "while");
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;
	fscanf(fpTokenin, "%s %s\n", token, token1);
	if (strcmp(token, "(")) return (es = 4);
	strcat(tree[Tree_num].elem, "(");
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;
	fscanf(fpTokenin, "%s %s\n", token, token1);

	cxEntrance = codesIndex;

	es = expression(layer, tmp);
	if (strcmp(token, ")")) return (es = 5);

	strcpy(codes[codesIndex].opt, "BRF");
	cx1 = codesIndex++;

	strcat(tree[Tree_num].elem, ")");
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;
	fscanf(fpTokenin, "%s %s\n", token, token1);
	es = statement(layer, tmp);

	strcpy(codes[codesIndex].opt, "BR");
	codes[codesIndex++].operand = cxEntrance;
	codes[cx1].operand = codesIndex;

	return es;
}

int for_stat(int layer, int parent) {
	int es = 0;
	int tmp;
	int cx1, cx2, cxExp2, cxExp3;

	layer++;
	tmp = Tree_num;
	/*strcat(tree[Tree_num].elem, "<for_stat>");
	tree[Tree_num].layer = layer;
	tree[Tree_num].parent_P = parent;
	Tree_num++;
	strcat(tree[Tree_num].elem, "for");
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;*/
	fscanf(fpTokenin, "%s %s\n", token, token1);
	if (strcmp(token, "(")) return (es = 4);
	strcat(tree[Tree_num].elem, "(");
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;
	fscanf(fpTokenin, "%s %s\n", token, token1);
	es = expression(layer, tmp);
	if (strcmp(token, ";")) return (es = 9);
	strcat(tree[Tree_num].elem, ";");
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;

	cxExp2 = codesIndex;

	fscanf(fpTokenin, "%s %s\n", token, token1);
	es = expression(layer, tmp);

	strcpy(codes[codesIndex].opt, "BRF");
	cx1 = codesIndex++;
	strcpy(codes[codesIndex].opt, "BR");
	cx2 = codesIndex++;

	if (strcmp(token, ";")) return (es = 9);
	strcat(tree[Tree_num].elem, ";");
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;

	cxExp3 = codesIndex;

	fscanf(fpTokenin, "%s %s\n", token, token1);
	es = expression(layer, tmp);

	strcpy(codes[codesIndex].opt, "BR");
	codes[codesIndex++].operand = cxExp2;

	if (strcmp(token, ")")) return (es = 5);
	strcat(tree[Tree_num].elem, ")");
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;

	codes[cx2].operand = codesIndex;

	fscanf(fpTokenin, "%s %s\n", token, token1);
	es = statement(layer, tmp);

	strcpy(codes[codesIndex].opt, "BR");
	codes[codesIndex++].operand = cxExp3;
	codes[cx1].operand = codesIndex;

	return es;
}

int write_stat(int layer, int parent) {
	int es = 0;
	int tmp;
	layer++;
	tmp = Tree_num;
	/*strcat(tree[Tree_num].elem, "<write_stat>");
	tree[Tree_num].layer = layer;
	tree[Tree_num].parent_P = parent;
	Tree_num++;
	strcat(tree[Tree_num].elem, "write");
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;*/
	fscanf(fpTokenin, "%s %s\n", token, token1);
	es = expression(layer, tmp);
	if (strcmp(token, ";")) return (es = 9);
	linecode++;
	strcat(tree[Tree_num].elem, ";");
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;

	strcpy(codes[codesIndex++].opt, "OUT");

	fscanf(fpTokenin, "%s %s\n", token, token1);
	return es;
}

int read_stat(int layer, int parent) {
	int es = 0;
	int tmp;
	int symbolPos;
	layer++;
	tmp = Tree_num;
	/*strcat(tree[Tree_num].elem, "<read_stat>");
	tree[Tree_num].layer = layer;
	tree[Tree_num].parent_P = parent;
	Tree_num++;
	strcat(tree[Tree_num].elem, "read");
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;*/
	fscanf(fpTokenin, "%s %s\n", token, token1);
	if (strcmp(token, "ID")) return (es = 8);
	strcat(tree[Tree_num].elem, "ID");
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;

	es = lookup(token1, &symbolPos);
	if (es == -1) {
		es == 0;
		if (!symbol[symbolPos].kind == variable) {
			error[error_count].E_es = 22;
			error[error_count++].E_linecode = linecode;
		}
	}
	strcpy(codes[codesIndex++].opt, "IN");
	strcpy(codes[codesIndex].opt, "STO");
	if (es == -1) {
		es = 0;
		codes[codesIndex++].operand = symbol[symbolPos].address;
	}

	fscanf(fpTokenin, "%s %s\n", token, token1);
	if (strcmp(token, ";")) return (es = 9);
	strcat(tree[Tree_num].elem, ";");
	linecode++;
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;
	fscanf(fpTokenin, "%s %s\n", token, token1);
	return es;
}

int compound_stat(int layer, int parent) {
	int es = 0;
	int tmp;
	layer++;
	tmp = Tree_num;
	linecode++;
	/*strcat(tree[Tree_num].elem, "<compound_stat>");
	tree[Tree_num].layer = layer;
	tree[Tree_num].parent_P = parent;
	Tree_num++;
	strcat(tree[Tree_num].elem, "{");
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;*/
	fscanf(fpTokenin, "%s %s\n", token, token1);
	es = statement_list(layer, tmp);
	fscanf(fpTokenin, "%s %s\n", token, token1);
	if (strcmp(token, "}")) return (es = 7);
	linecode++;
	strcat(tree[Tree_num].elem, "}");
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;
	fscanf(fpTokenin, "%s %s\n", token, token1);
	return es;
}

int call_stat(int layer, int parent) {
	int es = 0;
	int tmp;
	int symbolPos1;
	int symbolPos2;
	int symbolPos3;
	layer++;
	tmp = Tree_num;
	/*strcat(tree[Tree_num].elem, "<call_stat>");
	tree[Tree_num].layer = layer;
	tree[Tree_num].parent_P = parent;
	Tree_num++;
	strcat(tree[Tree_num].elem, "call");
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;*/
	fscanf(fpTokenin, "%s %s\n", token, token1);
	if (strcmp(token, "ID")) return (es = 8);

	es = lookup(token1, &symbolPos1);
	if (es == -1) {
		es = 0;
		if (!symbol[symbolPos1].kind == function) {
			error[error_count].E_es = 23;
			error[error_count++].E_linecode = linecode;
		}
	}

	strcat(tree[Tree_num].elem, "ID");
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;
	fscanf(fpTokenin, "%s %s\n", token, token1);
	if (strcmp(token, "(")) return(es = 4);
	strcat(tree[Tree_num].elem, "(");
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;
	fscanf(fpTokenin, "%s %s\n", token, token1);
	if (!strcmp(token, "ID")) {
		es = lookup(token1, &symbolPos2);
		if (es == -1) {
			es = 0;
			if (symbol[symbolPos2].kind == function) {
				error[error_count].E_es = 23;
				error[error_count++].E_linecode = linecode;
			}
		}
		strcpy(codes[codesIndex].opt, "LOAD");
		codes[codesIndex++].operand = symbol[symbolPos2].address;
		es = lookup(symbol[symbolPos2].function_area, &symbolPos3);
		if (es == -1) {
			es = 0;
			if (!symbol[symbolPos3].kind == function) {
				error[error_count].E_es = 23;
				error[error_count++].E_linecode = linecode;
			}
		}
		/*strcpy(codes[codesIndex].opt, "STO");
		codes[codesIndex++].operand = symbol[symbolPos3 + 1].address;*/
		fscanf(fpTokenin, "%s %s\n", token, token1);
	}
	if (strcmp(token, ")")) return(es = 5);
	strcat(tree[Tree_num].elem, ")");
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;
	fscanf(fpTokenin, "%s %s\n", token, token1);
	if (strcmp(token, ";")) return (es = 9);
	linecode++;
	strcat(tree[Tree_num].elem, ";");
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;
	fscanf(fpTokenin, "%s %s\n", token, token1);

	strcpy(codes[codesIndex].opt, "CAL");
	codes[codesIndex++].operand = symbol[symbolPos1].address;
	return es;
}

int expression_stat(int layer, int parent) {
	int es = 0;
	int tmp;
	layer++;
	tmp = Tree_num;
	strcat(tree[Tree_num].elem, "<expression_stat>");
	tree[Tree_num].layer = layer;
	tree[Tree_num].parent_P = parent;
	Tree_num++;
	if (!strcmp(token, ";")) {
		fscanf(fpTokenin, "%s %s\n", token, token1);
		strcat(tree[Tree_num].elem, ";");
		tree[Tree_num].layer = layer + 1;
		tree[Tree_num].parent_P = tmp;
		Tree_num++;
		linecode++;
		return es;
	}
	es = expression(layer, tmp);
	if (strcmp(token, ";")) return (es = 9);
	linecode++;
	strcat(tree[Tree_num].elem, ";");
	tree[Tree_num].layer = layer + 1;
	tree[Tree_num].parent_P = tmp;
	Tree_num++;
	fscanf(fpTokenin, "%s %s\n", token, token1);
	return es;
}

int expression(int layer, int parent) {
	int es = 0;
	int tmp;
	int symbolPos;
	layer++;
	tmp = Tree_num;
	strcat(tree[Tree_num].elem, "<expression>");
	tree[Tree_num].layer = layer;
	tree[Tree_num].parent_P = parent;
	Tree_num++;
	int fileadd;
	int b;
	int c;
	char token2[20];
	char token3[40];
	if (!strcmp(token, "ID")) {
		b = Tree_num;
		c = layer;
		strcat(tree[Tree_num].elem, "ID");
		tree[Tree_num].layer = layer + 1;
		tree[Tree_num].parent_P = tmp;
		Tree_num++;

		es = lookup(token1, &symbolPos_all);

		if (es == -1) {
			es = 0;
			if (!symbol[symbolPos_all].kind == variable) {
				//error[error_count].E_es = 24;
				//error[error_count++].E_linecode = linecode;
			}
		}

		fileadd = ftell(fpTokenin);
		fscanf(fpTokenin, "%s %s\n", &token2, &token3);
		if (!strcmp(token2, "=")) {

			es = lookup(token1, &symbolPos);
			if (es == -1) {
				es = 0;
				if (!symbol[symbolPos].kind == variable) {
					//error[error_count].E_es = 24;
					//error[error_count++].E_linecode = linecode;
				}
			}

			strcat(tree[Tree_num].elem, "=");
			tree[Tree_num].layer = layer + 1;
			tree[Tree_num].parent_P = tmp;
			Tree_num++;
			fscanf(fpTokenin, "%s %s\n", token, token1);
			es = bool_expr(layer, tmp);

			strcpy(codes[codesIndex].opt, "STO");
			codes[codesIndex++].operand = symbol[symbolPos].address;

		}
		else {
			fseek(fpTokenin, fileadd, 0);
			Tree_num = b;
			layer = c;
			es = bool_expr(layer, tmp);
		}
	}
	else es = bool_expr(layer, tmp);
	return es;
}

int bool_expr(int layer, int parent) {
	int es = 0;
	int tmp;
	layer++;
	tmp = Tree_num;
	strcat(tree[Tree_num].elem, "<bool_expr>");
	tree[Tree_num].layer = layer;
	tree[Tree_num].parent_P = parent;
	Tree_num++;
	es = additive_expr(layer, tmp);
	if (es > 0) return es;
	if (!strcmp(token, "++")) {
		fscanf(fpTokenin, "%s %s\n", token, token1);
		strcpy(codes[codesIndex].opt, "DADD");
		if (symbolPos_all != 0) {
			codes[codesIndex++].operand = symbol[symbolPos_all].address;
		}
		return (es = 0);
	}
	if (!strcmp(token, ">") || !strcmp(token, "<") || !strcmp(token, ">=") || !strcmp(token, "<=") || !strcmp(token, "==") || !strcmp(token, "!=")) {
		char token2[20];
		strcpy(token2, token);

		strcat(tree[Tree_num].elem, token);
		tree[Tree_num].layer = layer + 1;
		tree[Tree_num].parent_P = tmp;
		Tree_num++;
		fscanf(fpTokenin, "%s %s\n", token, token1);
		es = additive_expr(layer, tmp);
		if (!strcmp(token2, ">"))
			strcpy(codes[codesIndex++].opt, "GT");
		if (!strcmp(token2, ">="))
			strcpy(codes[codesIndex++].opt, "GE");
		if (!strcmp(token2, "<"))
			strcpy(codes[codesIndex++].opt, "LES");
		if (!strcmp(token2, "<="))
			strcpy(codes[codesIndex++].opt, "LE");
		if (!strcmp(token2, "=="))
			strcpy(codes[codesIndex++].opt, "EQ");
		if (!strcmp(token2, "!="))
			strcpy(codes[codesIndex++].opt, "NOTEQ");
	}
	return es;
}

int additive_expr(int layer, int parent) {
	int es = 0;
	int tmp;
	layer++;
	tmp = Tree_num;
	strcat(tree[Tree_num].elem, "<additive_expr>");
	tree[Tree_num].layer = layer;
	tree[Tree_num].parent_P = parent;
	Tree_num++;
	es = term(layer, tmp);
	while (!strcmp(token, "+") || !strcmp(token, "-")) {
		char token2[20];
		strcpy(token2, token);
		strcat(tree[Tree_num].elem, token);
		tree[Tree_num].layer = layer + 1;
		tree[Tree_num].parent_P = tmp;
		Tree_num++;
		fscanf(fpTokenin, "%s %s\n", token, token1);
		es = term(layer, tmp);
		if (!strcmp(token2, "+"))
			strcpy(codes[codesIndex++].opt, "ADD");
		if (!strcmp(token2, "-"))
			strcpy(codes[codesIndex++].opt, "SUB");
	}
	return es;
}

int term(int layer, int parent) {
	int es = 0;
	int tmp;
	layer++;
	tmp = Tree_num;
	strcat(tree[Tree_num].elem, "<term>");
	tree[Tree_num].layer = layer;
	tree[Tree_num].parent_P = parent;
	Tree_num++;
	es = factor(layer, tmp);
	while (!strcmp(token, "*") || !strcmp(token, "/")) {
		char token2[20];
		strcpy(token2, token);
		strcat(tree[Tree_num].elem, token);
		tree[Tree_num].layer = layer + 1;
		tree[Tree_num].parent_P = tmp;
		Tree_num++;
		fscanf(fpTokenin, "%s %s\n", token, token1);
		es = factor(layer, tmp);
		if (!strcmp(token2, "*"))
			strcpy(codes[codesIndex++].opt, "MULT");
		if (!strcmp(token2, "/"))
			strcpy(codes[codesIndex++].opt, "DIV");
	}
	return es;
}

int factor(int layer, int parent) {
	int es = 0;
	int tmp;
	layer++;
	tmp = Tree_num;
	strcat(tree[Tree_num].elem, "<factor>");
	tree[Tree_num].layer = layer;
	tree[Tree_num].parent_P = parent;
	Tree_num++;
	if (!strcmp(token, "(")) {
		strcat(tree[Tree_num].elem, token);
		tree[Tree_num].layer = layer + 1;
		tree[Tree_num].parent_P = tmp;
		Tree_num++;
		fscanf(fpTokenin, "%s %s\n", token, token1);
		es = additive_expr(layer, tmp);
		if (strcmp(token, ")")) return (es = 5);
		strcat(tree[Tree_num].elem, token);
		tree[Tree_num].layer = layer + 1;
		tree[Tree_num].parent_P = tmp;
		Tree_num++;
		fscanf(fpTokenin, "%s %s\n", token, token1);
	}
	else if (!strcmp(token, "ID")) {
		int symbolPos;
		es = lookup(token1, &symbolPos);
		if (es == -1) {
			es = 0;
			if (!symbol[symbolPos].kind == variable) {
				//error[error_count].E_es = 25;
				//error[error_count++].E_linecode = linecode;
			}
		}
		strcpy(codes[codesIndex].opt, "LOAD");
		codes[codesIndex++].operand = symbol[symbolPos].address;

		strcat(tree[Tree_num].elem, token);
		tree[Tree_num].layer = layer + 1;
		tree[Tree_num].parent_P = tmp;
		Tree_num++;
		fscanf(fpTokenin, "%s %s\n", token, token1);
		return es;
	}
	else if (!strcmp(token, "NUM")) {

		strcpy(codes[codesIndex].opt, "LOADI");
		codes[codesIndex++].operand = atoi(token1);

		strcat(tree[Tree_num].elem, token);
		tree[Tree_num].layer = layer + 1;
		tree[Tree_num].parent_P = tmp;
		Tree_num++;
		fscanf(fpTokenin, "%s %s\n", token, token1);
		return es;
	}
	else
		return (es = 10);
	return es;
}

/*void tree_print() {
	printf("请输入语法树文件名:");
	scanf("%s", tokenfile);
	fpTreeout = fopen(tokenfile, "w");
	int count_layer = 0;
	int count = 0;
	int flag = 0;
	Tree* ptree = tree;
	while (ptree[count].parent_P >= 0) {
		if (ptree[count].layer == count_layer) {
			if (flag == 1) {
				fprintf(fpTreeout, "\n");
				for (int i = 0;i < ptree[count].layer;i++) {
					if (i == ptree[count].layer - 1) {
						fprintf(fpTreeout, "------------------------");
					}
					else {
						fprintf(fpTreeout, "\t\t\t");
					}
				}
			}
			fprintf(fpTreeout, "%s", ptree[count].elem);
			flag = 1;
			count++;
		}
		else if (ptree[count].layer > count_layer) {
			count--;
			count_layer++;
			fprintf(fpTreeout, "\n");
			for (int i = 0;i <= ptree[count].layer;i++) {
				if (i != ptree[count].layer) {
					fprintf(fpTreeout, "\t\t\t");
				}
				else {
					fprintf(fpTreeout, "------------------------");
				}

			}
			flag = 0;
			count++;
		}
		else {
			count_layer = ptree[count].layer;
			fprintf(fpTreeout, "\n");
		}
	}
	fclose(fpTreeout);
	return;
}*/

int insert_Symbol(enum Category_symbol category, const char* name) {
	int i;
	int es = 0;
	if (symbolIndex >= maxsymbolIndex) return 18;
	switch (category) {
	case function:
		for (i = symbolIndex - 1; i >= 0; i--) {
			if ((strcmp(symbol[i].name, name) == 0) && (symbol[i].kind == function)) {
				error[error_count].E_es = 19;
				error[error_count++].E_linecode = linecode;
				break;
			}
		}
		symbol[symbolIndex].kind = function;
		break;
	case variable:
		for (i = symbolIndex - 1; i >= 0; i--) {
			if ((strcmp(symbol[i].name, name) == 0) && (symbol[i].kind == variable)) {
				error[error_count].E_es = 20;
				error[error_count++].E_linecode = linecode;
				break;
			}
		}
		//设置作用域
		for (i = symbolIndex - 1; i >= 0; i--) {
			if (symbol[i].kind == function) {
				strcpy(symbol[symbolIndex].function_area, symbol[i].name);
				break;
			}
		}
		symbol[symbolIndex].kind = variable;
		symbol[symbolIndex].address = offset++;
		break;
	}
	strcpy(symbol[symbolIndex].name, name);
	symbolIndex++;
	return es;
}

int lookup(char* name, int* pPosition) {
	int i;
	int j;
	int es = 0;
	for (i = 0; i < symbolIndex; i++) {
		if (!strcmp(symbol[i].name, name)) {
			*pPosition = i;
			if (symbol[i].kind == variable) {
				for (j = symbolIndex - 1; j >= 0; j--) {
					if (symbol[j].kind == function) {
						if (strcmp(symbol[i].function_area, symbol[j].name)) {
							error[error_count].E_es = 27;
							error[error_count++].E_linecode = linecode;
						}
						break;
					}
				}
			}
			return (es = -1);
		}
	}

	error[error_count].E_es = 21;
	error[error_count++].E_linecode = linecode;
	return (es);
}