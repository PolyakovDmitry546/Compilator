#pragma once
#include<string>
#include<vector>
#include<iostream>
#include<map>

using namespace std;

struct STextPosition
{
	int linenumber;
	int charnumber;
};


class CError
{
public:
	STextPosition position;
	int code;
	virtual string ToString();
	CError(int code, STextPosition position);
};

class CLexicalError : public CError
{
public:
	string ToString() override;
	CLexicalError(int code, STextPosition position);
};

class CSyntaxError : public CError
{
public:
	string ToString() override;
	CSyntaxError(int code, STextPosition position);
};

class CSemanticError : public CError
{
public:
	string ToString() override;
	CSemanticError(int code, STextPosition position);
};

class CErrorManager
{
public:
	void PrintErrors();
	void AddError(CError* error);
private:
	vector<CError*> errors;
};