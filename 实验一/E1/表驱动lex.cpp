#include <iostream>    // for cin, cout
#include <map>    // for map
using std::cin;
using std::cout;
using std::map;
// the enumeration of DFA states
enum state { S0 = 0, S1, S2, S3, ERR };
// the max row and colum of transition-table
const int MAXROW = 5;
const int MAXCOL = 3;
//===----------------------------------------===//
//  state-transition table definition
//          a   b   
//     S0  S1  S1  
//     S1  S2  S2  
//     S2  S3  S3  
//     S3  ERR ERR 
//===----------------------------------------===//
// the first column set for invalid char that not in the alphabet
state transTable[MAXROW][MAXCOL] =
{
	{  ERR,  S1,  S1  },
	{  ERR,  S2,  S2  },
	{  ERR,  S3,  S3  },
	{  ERR, ERR, ERR  },
	{  ERR, ERR, ERR  }
};
// the map table for alphabet of (a|b)^n,4>n>=1 
map<char, int> alphabet;
//{
	// 'a' map to the 1 column and 'b' map to the 2 column of transTable
 //  { 'a' , 1 },
  // { 'b', 2 }
//};

void tableDrive();    // table-drive function
state F(state, char);    // state-transition function
char nextChar();    // get next input char
int main()
{
	cout << "================================\n";
	cout << " String-model: (a|b)^n,4>n>=1       \n";
	cout << " End-of-input: #                \n";
	cout << "================================\n";
	alphabet['a'] = 1;
	alphabet['b'] = 2;
	while (1) {
		cout << ">";
		tableDrive();
	}
	return 0;

}
// table-drive function for recognize the string-model:(a|b)*abb
void tableDrive()
{
	state s = S0;
	char c = nextChar();
	while (c != '#') {    // '#' indicate the end of input
		s = F(s, c);
		c = nextChar();
	}
	// S3 is the only accept state
	if (s == S1 || s == S2 || s == S3)
		cout << "accept\n";
	else
		cout << "reject\n";

}
//===----------------------------------------===//
//  state-transition function
//  s is the current state, c is the current char
//  base on the transition-table above of DFA
//===----------------------------------------===//
state F(state s, char c)
{

	state ret;
	int col = alphabet[c];    // if c not in alphabet, the maped value will set to 0
	ret = transTable[s][col];
	return ret;
}
// get the next input char
char nextChar()
{
	char ret;

	cin >> ret;
	return ret;
}
