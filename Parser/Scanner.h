#pragma once
#include"pch.h"

typedef enum {
	START,
	INSLASH,
	INASTERISK1,
	INASTERISK2,
	INID,
	INNUM,
	INSMAL,
	INBIG,
	INEQ,
	INEXCLAM,
	DONE
} STATE;

struct Token {
	string str;
	TokenType type;
};

static Token keyWords[] = { { "else", ELSE },{ "if", IF },{ "int", INT },
{ "return", RETURN },{ "void", VOID },{ "while", WHILE } };


class Scaner {
public:
	Scaner(fstream &_source) : source(_source) //³õÊ¼»¯
	{ 
		iterator = line.begin();
	}
	Scaner(Scaner &) = delete;

	Token getToken();

private:
	fstream &source;
	string line;
	Token token;
	string::iterator iterator;//µü´úÆ÷

	char getChar();
	void ungetChar();
	TokenType lookupReserved(const Token &t);
};


