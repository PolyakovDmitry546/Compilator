#include "Error.h"

map<int, string> error_message =
{
	{0, "не удалось открыть файл"},
	{11, "слишком большая целая константа"},
	{12, "слишком большая вещественная константа"},
	{13, "запрещенный символ"},
	{21, "пропущено слово program"},
	{22, "пропущен символ ."},
	{23, "пропущено имя программы"},
	{24, "пропущен символ ;"},
	{25, "пропущен символ ="},
	{26, "пропущено имя константы"},
	{27, "пропущено имя типа"},
	{28, "пропущен символ :"},
	{29, "ошибка в описании переменных"},
	{210, "пропущено слово begin"},
	{211, "пропущено слово end"},
	{212, "ожидалась переменная"},
	{213, "пропущен символ :="},
	{214, "ошибка в выражении"},
	{215, "ожидалcя оператор"},
	{216, "пропущено слово then"},
	{217, "пропущено слово do"},
	{31, "необявленный идентификатор"},
	{32, "повторное объявление идентификатора"},
	{33, "неверный идентификатор, ожидалась константа"},
	{34, "неверный идентификатор, ожидался тип"},
	{35, "неверный идентификатор, ожидалась переменная"},
	{36, "неверный тип выражения, ожидался логический тип"},
	{37, "неверный тип выражения, ожидался вещественный тип"},
	{38, "несовпадение типов переменной и выражения"},
	{39, "неверный идентификатор"},
	{391, "операнды DIV и MOD должны быть целыми"},
	{392, "операнды AND, NOT, OR должны быть булевыми"},
	{393, "недопустимые типы операндов операции *"},
	{394, "недопустимые типы операндов операции /"},
	{395, "недопустимые типы операндов операции + или —"},
	{396, "не удалось определить тип выражения"},
	{397, "несовместимые типы операндов"}
};

string CError::ToString()
{
	return "позиция: " + to_string(position.linenumber) + " " + to_string(position.charnumber) +
		"; код ошибки: " + to_string(code) + "; сообщение: " + error_message.at(code) + "\n";
}

CError::CError(int code, STextPosition position)
{
	this->code = code;
	this->position = position;
}

CLexicalError::CLexicalError(int code, STextPosition position) : CError(code, position) {}

string  CLexicalError::ToString()
{
	return "Лексическая ошибка: " + CError::ToString();
}

CSyntaxError::CSyntaxError(int code, STextPosition position) : CError(code, position) {}

string  CSyntaxError::ToString()
{
	return "Синтаксическая ошибка: " + CError::ToString();
}

CSemanticError::CSemanticError(int code, STextPosition position) : CError(code, position) {}

string  CSemanticError::ToString()
{
	return "Семантическая ошибка: " + CError::ToString();
}


void CErrorManager::PrintErrors()
{
	if (errors.size() == 0)
	{
		cout << "Ошибок не обнаружено" << endl;
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