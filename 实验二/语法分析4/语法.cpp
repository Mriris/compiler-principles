#pragma warning (disable:4996)
//语法、语义分析及代码生成
#include<stdio.h>
#include<ctype.h>
#include<conio.h>
#include<string.h>
#define maxvartablep 500
int TESTparse();
int program();
int compound_stat();
int statement();
int expression_stat();
int expression();
int bool_expr();
int additive_expr();
int term();
int factor();
int if_stat();
int while_stat();
int for_stat();
int write_stat();
int read_stat();
int declaration_stat();
int declaration_list();
int statement_list();
int compound_stat();
int name_def(char* name);
char token[20], token1[40];
char Scanin[300], Scanout[300], Codeout[300];
FILE* fp, * fou; struct {
	char name[8];
	//定义符号表的容量
	//token保存单词符号,token1保存单词值//保存词法分析输出文件名//用于指向输入输出文件的指针//定义符号表结构
	int address;
}vartable[maxvartablep];
int vartablep = 0, labelp = 0, datap = 0;

extern int TESTscan();
extern int TESTparse();

//改符号表最多容纳maxvartablep个记录
//插入符号表动作ename-def;,的程序如下：
int name_def(char* name)
{
	int i, es = 0;
	if (vartablep >= maxvartablep) 
		return(21);
	for (i = vartablep - 1; i == 0; i--)
	{
		if (strcmp(vartable[i].name, name) == 0)
		{
			es = 22;
			break;
	}
}
	if (es > 0)
		return(es);
	strcpy(vartable[vartablep].name, name);
	vartable[vartablep].address = datap;
	datap++;
	vartablep++;
	return(es);
}
//查询符号表返回地址
int lookup(char* name, int* paddress)
{
	int i, es = 0;
	for (i = 0; i < vartablep; i++)
	{
		if (strcmp(vartable[i].name, name) == 0)
		{
			*paddress = vartable[i].address;
			return(es);
		}
	}
	es = 23;
	return(es);
}
//查符号表
//22表示变量重复声明
//分配一个单元,数据区指针加1
//变量没有声明
//语法、语义分析及代码生成程序
int TESTparse()
{
	int es = 0;
	strcpy(Scanout, "Testout.txt");
	if ((fp = fopen(Scanout, "r")) == NULL)
	{
		printf("\n打开%s错误!\n", Scanout);
		es = 10;
		return(es);
	}
//	printf("请输入目标文件名(包括路径):");
//	scanf("%s", Codeout);
	strcpy(Codeout, "Codeout.txt");
	if ((fou = fopen(Codeout, "w")) == NULL)
	{
		printf("\n创建%s错误!\n", Codeout);
		es = 10;
		return(es);
	}
	if (es == 0)
		es = program();
	printf("==语法、语义分析及代码生成程序结果==\n");
	switch (es) {
	case 0: printf("语法、语义分析成功并抽象机汇编生成代码\n"); break;
	case 10:printf("打开文件名%s失败\n", Scanout); break;
	case 1: printf("缺少{\n"); break;
	case 2:printf("缺少}\n"); break;
	case 3:printf("缺少标识符!\n"); break;
	case 4: printf("少分号\n"); break;
	case 5: printf("缺少(\n"); break;
	case 6: printf("缺少)\n"); break;
	case 7: printf("缺少操作数\n"); break;
	case 21:printf("符号表溢出\n"); break;
	case 22:printf("变量重复定义\n"); break;
	case 23:printf("变量未声明\n"); break;
	}
	fclose(fp);
	fclose(fou);
	return(es);
}
//program::={<declaration_list><statement_list>}
int program() {
	int es = 0, i;
	fscanf(fp, "%s %s\n", token, token1);
	printf("%s %s\n", token, token1);
	if (strcmp(token, "{"))
	{
		es = 1;
		return(es);
	}
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s %s\n", token, token1);
	es = declaration_list();
	if (es > 0)
		return(es);
	printf("		符号表\n");
	printf("		名字		地址\n");
	for (i = 0; i < vartablep; i++)
		printf("		%s	%d\n", vartable[i].name, vartable[i].address);
	es = statement_list();
	if (es > 0)
		return(es);
	if (strcmp(token, "}"))//判断是否'
	{
		es = 2;
		return(es);
	}
	//产生停止指令
	fprintf(fou, "		STOP\n");
	return(es);
}
	//<declaration_list>::=<declaration_list><declaration_stat>|<declaration_stat>
	//改成<declaration_list>::={<declaration_stat>)
int declaration_list() {
	int es = 0;
	while (strcmp(token, "int") == 0)
	{
		es = declaration_stat();
		if (es > 0)
			return(es);
	}
	return(es);
}
		//<declaration_stat>;wrtasip,tap,coap→int ID,,@name-def;mt;
int declaration_stat() {
	int es = 0;
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s %s\n", token, token1);
	if (strcmp(token, "ID"))
		return(es = 3);
	//不是标识符
	es = name_def(token1);
	//插入符号表
	if (es > 0)
		return(es);
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s %s\n", token, token1);
	if (strcmp(token, ";"))
		return(es = 4);
	fscanf(fp, "%s %s\n", &token, &token1); printf("%s %s\n", token, token1);
	return(es);
}
		//<statement_list>::=<statement_list><statement>!<statement>
		//改成<statement list>::={<statement>}
int statement_list() {
	int es = 0;
	while (strcmp(token, ")")) {
		es = statement();
		if (es > 0)
			return(es);
	}
	return(es);
}
		//<statement>::=<if_stat>|<while_stat>|<for_stat>
		//		| <compound_stat> | <expression_stat>
int statement() {
	int es = 0;
	if (es == 0 && strcmp(token, "if") == 0)
		es = if_stat();
	//<IF语句>
	if (es == 0 && strcmp(token, "while") == 0)
		es = while_stat();
	//<while语句>
	if (es == 0 && strcmp(token, "for") == 0)
		es = for_stat();
	//<for语句>
	//可在此处添加do语句调用
	if (es == 0 && strcmp(token, "read") == 0)
		es = read_stat();
	//<read语句>
	if (es == 0 && strcmp(token, "write") == 0)
		es = write_stat();
	//<write语句>
	if (es == 0 && strcmp(token, "{") == 0)
		es = compound_stat();
	//<复合语句>
	if (es == 0 && (strcmp(token, "ID") == 0 || strcmp(token, "number") == 0 || strcmp(token, "(") == 0))
		es = expression_stat();
	//<表达式语句>
	return(es);
}
/*<if_stat>:: = if (<expr>)<statement>[else<statement>]
				if (<expression>)eBRFtiae<statement>eBRtiaelzESETlabel, iaelt
					[else<statement>]@SETlabel, 1mbel2
					其中动作符号的含义如下
					CBRFtuabln : 输出BRF label1
					241

					242
					编译原理及实现(第2版)
					Q BR + iabeiz; 输出BR Iabe12
					CSETlabel, iabeli:设置标号label1
					@SETlabel, iabeiz:设置标号label2
					*/
int if_stat() {
	int es = 0, label1, label2;//if
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s %s\n", token, token1);
	if (strcmp(token, "("))
		return(es = 5);
	//少左括号
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s %s\n", token, token1);
	es = expression();
	if (es > 0)
		return(es);
	if (strcmp(token, ")"))
		return(es = 6);//少右括号
	label1=labelp++;//用label1记住条件为假时要转向的标号
	fprintf(fou,"		BRF LABEL%d\n",label1);//输出假转移指令
	fscanf(fp,"%s %s\n\n",&token,&token1);
	printf("%s %s\n",token,token1);
	es=statement();
	if (es > 0)
		return(es);
	label2 = labelp++;//用label2记住要转向的标号
	fprintf(fou, "		BR LABEL%d\n", label2);//输出无条件转移指令
	fprintf(fou, "		LABEL%d:\n", label1);//设置label1记住的标号
	if (strcmp(token, "else") == 0) {//else部分处理
		fscanf(fp, "%s %s\n", &token, &token1);
		printf("%s %s\n", token, token1);
		es = statement();
		if (es > 0)
			return(es);
	}
	fprintf(fou, "LABEL%d:\n", label2);
	//设置label2记住的标号
	return(es);
}
				//<while_stat>::=while(<expr>)<statement>
				//<while_stat>::=while esETlabeltisheil(<expression>)eBRF+1ael₂
				//				<statement>eBRiabeile SETlabelyiatol₂
				//动作解释如下：
				//e SETlabel+ioelt:设置标号label1
				//eBRF+iabaiz:输出BRF label2
				//e BR4isben:输出BR label1
				//eSETlabelyiaelz:设置标号label2
int while_stat() {
	int es = 0, label1, label2;
	label1 = labelp++;
	fprintf(fou, "LABEL%d:\n", label1);//设置label1标号
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s %s\n", token, token1);
	if (strcmp(token, "("))
		return(es = 5);//少左括号
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s %s\n", token, token1);
	es = expression();
	if (es > 0)
		return(es);
	if (strcmp(token, ")"))
		return(es = 6);//少右括号
	label2 = labelp++;
	fprintf(fou, "BRF LABEL%d\n", label2);//输出假转移指令
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s %s\n", token, token1);
	es = statement();
	if (es > 0)
		return(es);
	fprintf(fou, "	BR LABEL%d\n", label1);//输出无条件转移指令
	fprintf(fou, "LABEL%d:\n", label2);//设置label2标号
	return(es);
}
			//<for_stat>::=for(<expr>,<expr>,<expr>)<statement>
			/*
			<for_stat>::=for(<expression>ePOP;
			eSETlabelrisell<expression>eBRFtiaeizeBR+labeis;
			@SETlabel+iabel4<expression>@ POPeBR,Iasell)
			eSETlabelyabels<语句>eBRVlabel4eSETlabel,zabel₂
			动作解释：
			1.    @SETlabel,iaben:设置标号label1
			2.    QBRFflabeiz:输出BRF label2
			3.    @BR+label₃:输出BR label3
			4.    @SETlabel,iabel4:设置标号label4
			5.    @BR+iabell:输出BR label1
			6.    @SETlabel;iatels:设置标号label3
			7.    @BR+label₄:输出BR label4
			8.    @SETlabel,iabeiz;设置标号label2
			*/
int for_stat() {
	int es = 0, label1, label2, label3, label4;
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s %s\n", token, token1);
	if (strcmp(token, "("))
		return(es = 5);
	//少左括号
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s %s\n", token, token1);
	es = expression();
	if (es > 0)
		return(es);
	fprintf(fou, "		POP\n");
	//输出出栈指令
	if (strcmp(token, ";"))
		return(es = 4);//少分号
	label1 = labelp++;
	fprintf(fou, "		LABEL%d:\n", label1);//设置label1标号
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s %s\n", token, token1);
	es = expression();
	if (es > 0)
		return(es);
	label2 = labelp++;
	fprintf(fou, "		BRF LABEL%d\n", label2);//输出假条件转移指令
	label3 = labelp++;
	fprintf(fou, "		BR LABEL%d\n", label3);//输出无条件转移指令
	if (strcmp(token, ";"))
		return(es = 4);//少分号
	label4 = labelp++;
	fprintf(fou, "LABEL%d:\n", label4);//设置label4标号
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s %s\n", token, token1);
	es = expression();
	if (es > 0)
		return(es);
	fprintf(fou, "		POP\n");//输出出栈指令
	fprintf(fou, "		BR LABEL%d\n", label1);
	//输出无条件转移指令
	if (strcmp(token, ")"))
		return(es = 6);//少右括号
	fprintf(fou, "		LABEL%d:\n", label3);//设置label3标号
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s %s\n", token, token1);
	es = statement();
	if (es > 0)
		return(es);
	fprintf(fou, "		BR LABEL%d\n", label4);//输出无条件转移指令
	fprintf(fou, "		LABEL%d:\n", label2);//设置label2标号
	return(es);
}
			//<write_stat>::=write<expression>;
			//<write_stat>::=write<expression>@ouT;
			//动作解释：
			//@0ur:输出ouT
int write_stat() {
	int es = 0;
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s %s\n", token, token1);
	es = expression();
	if (es > 0)
		return(es);
	if (strcmp(token, ";"))
		return(es = 4);
	fprintf(fou, "		0UT\n");
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s %s\n", token, token1);
	return(es);
}
			//少分号
			//输出OUT指令
			//<read stat>::=read ID;
			//<read stat>::=read ID+nLOOKintdCINCSTO,aQPOP;
			//动作解释：
			//@L0OK,nta:查符号表n,给出变量地址d;若没有,则变量未定义
			//@IN:输出IN
			//@STI,a:输出指令代码STI d
int read_stat() {
	int es = 0, address;
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s %s\n", token, token1);
	if (strcmp(token, "ID"))
		return(es = 3);
	es = lookup(token1, &address);
	if (es > 0)
		return(es);
	fprintf(fou, "		IN	\n");
	fprintf(fou, "		STO	%d\n", address);
	fprintf(fou, "		POP\n");
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s %s\n", token, token1);
	if (strcmp(token, ";"))
		return(es = 4);
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s %s\n", token, token1);
	return(es);
}
			//少标识符
			//输入指令
			//输出STo指令
			//少分号
			//<compound stat>::={<statement list>)
int compound_stat() {
	int es = 0;
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s %s\n", token, token1);
	//复合语句函数
	es = statement_list();
	return(es);
}
			//<expression_stat>:;=<expression>ePOp;I;
int expression_stat() {
	int es = 0;
	if (strcmp(token, ";") == 0)
	{
		fscanf(fp, "%s %s\n", &token, &token1);
		printf("%s %s\n", token, token1);
		return(es);
	}
	es = expression();
	if (es > 0)
		return(es);
	fprintf(fou, "		POP\n");
	if (strcmp(token, ";") == 0)
	{
		fscanf(fp, "%s %s\n", &token, &token1);
		printf("%s %s\n", token, token1);
		return(es);
	}
	else
	{
		es = 4;
		return(es);
	}
}
			//输出POP指令
			//少分号
			//<expression>::=ID+n@LOOK,n+aeASSIGN=<boo1_expr>eSTO,a@POPI<bool_expr>
int expression() {
	int es = 0, fileadd;
	char token2[20], token3[40];
	if (strcmp(token, "ID") == 0)
	{
		fileadd = ftell(fp);
		//eASSIGN记住当前文件位置
		fscanf(fp, "%s %s\n", &token2, &token3);
		printf("%s %s\n", token2, token3);
		if (strcmp(token2, "=") == 0)
			//'='
		{
			int address;
			es = lookup(token1, &address);
			if (es > 0) return(es);
			fscanf(fp, "%s %s\n", &token, &token1);
			printf("%s %s\n", token, token1);
			es = bool_expr();
			if (es > 0) return(es);
			fprintf(fou, "		STO%d\n", address);
		}
		else {
			//若非'='则文件指针回到'='前的标识符
			fseek(fp, fileadd, 0);
			printf("%s %s\n", token, token1);
			es = bool_expr();
			if (es > 0)
				return(es);
		}
	}
	else
		es = bool_expr();
	return(es);
}
				/*<bool_expr>:: = <additive_expr>
				I<additive_expr>(> | < I>=|<=I == 1 != ) < additive_expr >
					<bool_expr>:: = <additive_expr>
					I<additive_expr >> <additive_expr>eGT
					I<additive_expr> << additive_expr > eLES
					I < additive_expr >>= <additive_expr>@GE
					I<additive_expr> <= <additive_expr>eLE
					l < additive_expr >= = <additive_expr>eEQ
					I<additive_expr> != <additive_expr>eNOTBQ
					*/
int bool_expr() {
	int es = 0;
	es = additive_expr();
	if (es > 0)
		return(es);
	if (strcmp(token, ">") == 0 || strcmp(token, ">=") == 0
		|| strcmp(token, "<") == 0 || strcmp(token, "<=") == 0
		|| strcmp(token, "==") == 0 || strcmp(token, "!=") == 0) {
		char token2[20];
		strcpy(token2, token);
		fscanf(fp, "%s %s\n", &token, &token1);
		printf("%s %s\n", token, token1);
		es = additive_expr();
		if (es > 0)
			return(es);
		//保存运算符
		if (strcmp(token2, ">") == 0)
			fprintf(fou, "		GT\n");
		if (strcmp(token2, ">=") == 0)
			fprintf(fou, "		GE\n");
		if (strcmp(token2, "<") == 0)
			fprintf(fou, "		LES\n");
		if (strcmp(token2, "<=") == 0)
			fprintf(fou, "		LE\n");
		if (strcmp(token2, "==") == 0)
			fprintf(fou, "		EQ\n");
		if (strcmp(token2, "!=") == 0)
			fprintf(fou, "		NOTEQ\n");
	}
	return(es);
}	
			//<additive_expr>::=<term>{(+1-)<term>}
			//<additive_expr>::=<term>{(+<term>@ADDI-<项>eSUB)}
int additive_expr() {
	int es = 0;
	es = term();
	if (es > 0)
		return(es);
	while (strcmp(token, "+") == 0 || strcmp(token, "-") == 0)
	{
		char token2[20];
		strcpy(token2, token);
		fscanf(fp, "%s %s\n", &token, &token1);
		printf("%s %s\n", token, token1);
		es = term();
		if (es > 0)
			return(es);
		if (strcmp(token2, "+") == 0)
			fprintf(fou, "		ADD\n");
		if (strcmp(token2, "-") == 0)
			fprintf(fou, "		SUB\n");
	}
	return(es);
}
		//<term>::=<factor>{(×(/)<factor>}
		//<term>::=<factor>{(*<factor>eMULT|/<factor>eDIv)}
int term() {
	int es = 0;
	es = factor();
	if (es > 0)
		return(es);
	while (strcmp(token, "*") == 0 || strcmp(token, "/") == 0)
	{
		char token2[20];
		strcpy(token2, token);
		fscanf(fp, "%s %s\n", &token, &token1);
		printf("%s %s\n", token, token1);
		es = factor();
		if (es > 0)
			return(es);
		if (strcmp(token2, "*") == 0)
			fprintf(fou, "		MULT\n");
		if (strcmp(token2, "/") == 0)
			fprintf(fou, "		DIV\n");
	}
	return(es);
}
		//<factor>::=(<additive_expr>)IIDINUM
		//<factor>:;=(<expression>)|ID+n@LOOK,nta@LOAD,a|NUM+CLOADI
int factor() {
	int es = 0;
	if (strcmp(token, "(") == 0) {
		fscanf(fp, "%s %s\n", &token, &token1);
		printf("%s %s\n", token, token1);
		es = expression();
		if (es > 0)
			return(es);
		if (strcmp(token, ")")) {
			es = 6;
			return(es);
		}
		fscanf(fp, "%s %s\n", &token, &token1);
		printf("%s %s\n", token, token1);
	}
	else {
		//少右括号
		if (strcmp(token, "ID") == 0) {
			int address;
			//查符号表,获取变量地址
			es = lookup(token1, &address);
			if (es > 0)
				return(es);
			//变量没声明
			fprintf(fou, "		LOAD %d\n", address);
			fscanf(fp, "%s %s\n", &token, &token1);
			printf("%s %s\n", token, token1);
			return(es);
		}
		if (strcmp(token, "number") == 0) {
			fprintf(fou, "		LOADI%s\n", token1);
			fscanf(fp, "%s %s\n", &token, &token1);
			printf("%s %s\n", token, token1);
			return(es);
		}
		else {
			es = 7;
			return(es);
		}
	}
	return(es);
}
//缺少操作数
//主程序

int main() {
	int es = 0;
	es = TESTscan();
	if (es > 0)
		printf("词法分析有错,编译停止!");
	else
		printf("词法分析成功!\n");
	if (es == 0) {
		es = TESTparse();
		if (es == 0)
			printf("语法分析成功!\n");
		else
			printf("语法分析错误!\n");
	}
	return 0;
}

