#pragma once
#include<string>
#include<map>
#include<iostream>
#include<fstream>

#include"Token.h"
#include"Error.h"

#include<set>

using namespace std;

class CLexer
{
public:
	STextPosition position;
	CErrorManager* errorManager;

	CLexer(string file_name);
	~CLexer();
	CToken* getNextToken();
	CToken* skipTo(CToken* cur_token, set<EOperand> symbols);

private:
	string file_name;
	ifstream in;
	string line;
	char ch;

	void getNextChar();
	void passSpace();
};