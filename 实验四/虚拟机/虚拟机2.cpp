#pragma warning (disable:4996)
#include<stdio.h>
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<map>
#include<stack>
#include<vector>
using namespace std;

extern int parse();

struct Code
{        //中间代码
	string opt;
	int operand;
	Code(string opt, int operand)
	{
		this->opt = opt;
		this->operand = operand;
	}
};
enum opts
{
	LOAD, LOADI, STO, ADD, SUB, MULT, DIV, BR, BRF, EQ, NOTEQ, GT, LES, GE, LE, AND,
	OR, NOT, IN, OUT, CAL, ENTER, RETURN
};
map<string, int> choseOpt;     //用map集合方便switch中选择不同case，判断字符串方便
//初始化map，方便之后switch中选择case
void mapInit()
{
	choseOpt["LOAD"] = LOAD;   choseOpt["LOADI"] = LOADI;     choseOpt["STO"] = STO;
	choseOpt["ADD"] = ADD;     choseOpt["SUB"] = SUB;     choseOpt["MULT"] = MULT;
	choseOpt["DIV"] = DIV;     choseOpt["BR"] = BR;       choseOpt["BRF"] = BRF;
	choseOpt["EQ"] = EQ;       choseOpt["NOTEQ"] = NOTEQ;     choseOpt["GT"] = GT;
	choseOpt["LES"] = LES;     choseOpt["GE"] = GE;       choseOpt["LE"] = LE;
	choseOpt["AND"] = AND;     choseOpt["OR"] = OR;       choseOpt["NOT"] = NOT;
	choseOpt["IN"] = IN;       choseOpt["OUT"] = OUT;     choseOpt["CAL"] = CAL;
	choseOpt["ENTER"] = ENTER;     choseOpt["RETURN"] = RETURN;

}
//虚拟机
void getOpeNum(int* op1, int* op2, stack<int> operatorStack)
{
	*op2 = operatorStack.top();//获取操作数2
	operatorStack.pop();//出栈
	*op1 = operatorStack.top();//获取操作数1
	operatorStack.pop();//出栈
}
void getOpeNum(int* op, stack<int> operatorStack)
{
	*op = operatorStack.top();//获取操作数2
	operatorStack.pop();//出栈
}

void TESTmachine()
{
	stack<int>operatorStack; //操作数栈，和寄存器一样的功能
	int stack[1000] = { 0 };        //栈区，存放局部变量
	int allStack[1000] = { 0 };  //静态区，存放全局变量
	vector<Code>code;
	ifstream in;
	string codein = "Parseout.txt";       //输入文件名
	int codenum = 0;          //指令条数
	int top = 0, base = 0;     //栈顶和栈底
	int ip = 0;               //当前指令位置
	cout << "开始运行程序" << endl;
	in.open(codein, ios::in);
	if (!in.is_open()) {//打开输入文件
		cout << "打开文件错误" << endl;
		exit(-1);      //出错就运行结束
	}
	string buffer;
	while (getline(in, buffer))
	{   //读取中间代码
		string opt;
		int operand;
		istringstream str(buffer);
		str >> opt >> operand;
		code.push_back(Code(opt, operand));
		codenum++;
	}
	codenum--;     //最后一次读取会多加1
	in.close();
	mapInit();     //将map初始化
	int op1, op2;//定义操作数1，操作数2
	while (ip < codenum) {        //执行指令直到执行到最后一条指令为止，最后一条指令代表主函数结束
		Code temp = code[ip];  //用一个临时变量来执行操作
		ip++;                  //每执行一个指令地址就往后移动一位
		switch (choseOpt[temp.opt]) {    //根据操作码进行选择要执行的指令
		case LOAD: {       //LOAD D 将D中的内容加载到操作数栈
			int opt = temp.operand;
			if (opt > 1000)   //全局变量
			{
				operatorStack.push(allStack[opt - 1000]);
			}
			else  //局部变量
			{
				operatorStack.push(stack[opt + base]);
			}

			break;
		}
		case LOADI: {      //LOADI a 将常量a压入操作数栈
			operatorStack.push(temp.operand);  //将操作数压入到寄存器中
			stack[top] = temp.operand;
			break;
		}
		case STO: {        //STO D将操作数栈顶单元内容存入D
			int opt = temp.operand;
			if (opt > 1000)   //全局变量
			{
				allStack[opt - 1000] = operatorStack.top();
			}
			else  //局部变量
			{
				stack[opt + base] = operatorStack.top();
			}

			operatorStack.pop();//出栈
			break;
		}
		case ADD: {        //ADD 将栈顶单元与次栈顶单元出栈并相加，和置于栈顶
			getOpeNum(&op1, &op2, operatorStack);//获取操作数
			operatorStack.push(op1 + op2); //压栈
			break;
		}
		case SUB: {   		//将次栈顶单元减去栈顶单元并出栈，差置于栈顶。
			getOpeNum(&op1, &op2, operatorStack);//获取操作数
			operatorStack.push(op1 - op2); //压栈
			break;
		}
		case MULT: {      	//将次栈顶与栈顶单元出栈并相乘，积置于栈顶。
			getOpeNum(&op1, &op2, operatorStack);//获取操作数
			operatorStack.push(op1 * op2); //压栈
			break;
		}
		case DIV: {		//将次栈顶与栈顶单元出栈并相除，商置于栈顶
			getOpeNum(&op1, &op2, operatorStack);//获取操作数
			operatorStack.push(op1 / op2); //压栈
			break;
		}
		case BR: {         //BR lab 无条件转移到lab
			ip = temp.operand;      //操作数记录的就是要跳转的位置
			break;
		}
		case BRF: {		//BRF 若栈顶单元逻辑值，假(0)则转移到lab
			if (operatorStack.top() == 0)
			{
				ip = temp.operand;  //操作数记录的就是要跳转的位置
			}
			operatorStack.pop();
			break;
		}
		case EQ: {		    //将栈顶两单元做相等比较，并将结果真或假(1或0)置于栈顶
			getOpeNum(&op1, &op2, operatorStack);//获取操作数
			operatorStack.push(op1 == op2); //压栈
			break;
		}
		case NOTEQ: {		//栈顶两单元做不等于比较，并将结果 (1或0)置于栈顶

			getOpeNum(&op1, &op2, operatorStack);//获取操作数
			operatorStack.push(stack[top - 2] != stack[top - 1]);
			break;
		}
		case GT: {		    //次栈顶大于栈顶操作数，则栈顶置1，否则置0
			getOpeNum(&op1, &op2, operatorStack);//获取操作数
			operatorStack.push(stack[top - 2] > stack[top - 1]);
			break;
		}
		case LES: {		//次栈顶小于栈顶操作数，则栈顶置1，否则置0
			getOpeNum(&op1, &op2, operatorStack);//获取操作数
			operatorStack.push(stack[top - 2] < stack[top - 1]);
			break;
		}
		case GE: {		    //次栈顶大于等于栈顶操作数，则栈顶置1，否则置0
			getOpeNum(&op1, &op2, operatorStack);//获取操作数
			operatorStack.push(stack[top - 2] >= stack[top - 1]);

			break;
		}
		case LE: {		    //次栈顶小于等于栈顶操作数，则栈顶置1，否则置0
			getOpeNum(&op1, &op2, operatorStack);//获取操作数
			operatorStack.push(stack[top - 2] <= stack[top - 1]);
			break;
		}
		case AND: {		//将栈顶两单元做逻辑与运算，并将结果 (1或0)置于栈顶
			getOpeNum(&op1, &op2, operatorStack);//获取操作数
			operatorStack.push(stack[top - 2] && stack[top - 1]);
			break;
		}
		case OR: {		    //将栈顶两单元做逻辑或运算，并将结果 (1或0)置于栈顶
			getOpeNum(&op1, &op2, operatorStack);//获取操作数
			operatorStack.push(stack[top - 2] || stack[top - 1]);
			break;
		}
		case NOT: {		//将栈顶的逻辑值取反
			getOpeNum(&op1, operatorStack);
			operatorStack.push(!op1);
			break;
		}
		case IN: {        	//从标准输入设备(键盘)读入一个整型数据，并入操作数栈
			cout << "输入数据:";
			int num = 0;
			cin >> num;
			operatorStack.push(num);
			break;
		}
		case OUT: {		//将栈顶单元内容出栈，并输出到标准输出设备上(显示器)
			cout << "输出: " << operatorStack.top() << endl;
			operatorStack.pop();
			break;
		}
		case CAL: {        //调用函数
			stack[top] = base;     //记录主函数的基地址
			stack[top + 1] = ip;     //记录函数执行完要返回主函数的位置
			ip = temp.operand;     //执行指令位置跳转到函数开始的位置
			base = top;            //进入函数后对于当前函数的基地址
			break;
		}
		case ENTER: {      //进入函数体
			top += temp.operand;   //为函数开辟空间
			break;
		}
		case RETURN: {             //函数返回
			top = base;            //释放函数开辟的空间
			ip = stack[top + 1];     //第二个位置存放的是返回到主函数的位置
			base = stack[top];     //回到栈底就是主函数的基地址，重新赋值
			break;
		}
		}
	}
}
int main() {
	int flag = 0;
	flag = parse();
	TESTmachine();
	return 0;
}

