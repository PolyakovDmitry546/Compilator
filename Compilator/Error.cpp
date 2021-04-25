#include "Error.h"

map<int, string> error_message =
{
	{0, "�� ������� ������� ����"},
	{11, "������� ������� ����� ���������"},
	{12, "������� ������� ������������ ���������"},
	{13, "����������� ������"},
	{21, "��������� ����� program"},
	{22, "�������� ������ ."},
	{23, "��������� ��� ���������"},
	{24, "�������� ������ ;"},
	{25, "�������� ������ ="},
	{26, "��������� ��� ���������"},
	{27, "��������� ��� ����"},
	{28, "�������� ������ :"},
	{29, "������ � �������� ����������"},
	{210, "��������� ����� begin"},
	{211, "��������� ����� end"},
	{212, "��������� ����������"},
	{213, "�������� ������ :="},
	{214, "������ � ���������"},
	{215, "������c� ��������"},
	{216, "��������� ����� then"},
	{217, "��������� ����� do"},
	{31, "������������ �������������"},
	{32, "��������� ���������� ��������������"},
	{33, "�������� �������������, ��������� ���������"},
	{34, "�������� �������������, �������� ���"},
	{35, "�������� �������������, ��������� ����������"},
	{36, "�������� ��� ���������, �������� ���������� ���"},
	{37, "�������� ��� ���������, �������� ������������ ���"},
	{38, "������������ ����� ���������� � ���������"},
	{39, "�������� �������������"},
	{391, "�������� DIV � MOD ������ ���� ������"},
	{392, "�������� AND, NOT, OR ������ ���� ��������"},
	{393, "������������ ���� ��������� �������� *"},
	{394, "������������ ���� ��������� �������� /"},
	{395, "������������ ���� ��������� �������� + ��� �"},
	{396, "�� ������� ���������� ��� ���������"},
	{397, "������������� ���� ���������"}
};

string CError::ToString()
{
	return "�������: " + to_string(position.linenumber) + " " + to_string(position.charnumber) +
		"; ��� ������: " + to_string(code) + "; ���������: " + error_message.at(code) + "\n";
}

CError::CError(int code, STextPosition position)
{
	this->code = code;
	this->position = position;
}

CLexicalError::CLexicalError(int code, STextPosition position) : CError(code, position) {}

string  CLexicalError::ToString()
{
	return "����������� ������: " + CError::ToString();
}

CSyntaxError::CSyntaxError(int code, STextPosition position) : CError(code, position) {}

string  CSyntaxError::ToString()
{
	return "�������������� ������: " + CError::ToString();
}

CSemanticError::CSemanticError(int code, STextPosition position) : CError(code, position) {}

string  CSemanticError::ToString()
{
	return "������������� ������: " + CError::ToString();
}


void CErrorManager::PrintErrors()
{
	if (errors.size() == 0)
	{
		cout << "������ �� ����������" << endl;
	}
	for (int i = 0; i < errors.size(); i++)
	{
		cout << errors[i]->ToString();
	}
}

void CErrorManager::AddError(CError* error)
{
	errors.push_back(error);
}