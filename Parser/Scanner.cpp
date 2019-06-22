#include "pch.h"
#include "Scanner.h"

#define MAXLEN 256
char buf[MAXLEN];

char Scaner::getChar() {

	while (iterator == line.end() && !source.eof()) {
		source.getline(buf, MAXLEN);
		line = string(buf);
		iterator = line.begin();
	}
	if (iterator == line.end())
		return EOF;
	else {
		return *iterator++;
	}
}

void Scaner::ungetChar() {
	if (iterator != line.begin()) 
		iterator--;
}

TokenType Scaner::lookupReserved(const Token &t) {

	for (auto word : keyWords) {
		if (word.str == t.str)
			return word.type;
	}
	return TokenType::ID;
}

Token Scaner::getToken() {
	token.str.clear();
	STATE state = STATE::START;
	bool push = false;
	bool isComment = false;
	char ch;

	while (state != STATE::DONE) {
		push = true;
		ch = getChar();
		switch (state) {
		case STATE::START:
			if (isdigit(ch))
				state = INNUM;
			else if (isalpha(ch))
				state = INID;
			else
				switch (ch) {
				case '<':
					state = STATE::INSMAL;
					break;
				case '>':
					state = STATE::INBIG;
					break;
				case '=':
					state = STATE::INEQ;
					break;
				case '!':
					state = STATE::INEXCLAM;
					break;
				case ' ':
				case '\t':
				case '\n':
					push = false;
					break;
				case '/':
					state = STATE::INSLASH;
					break;
				default:
					state = DONE;
					switch (ch) {
					case EOF:
						push = false;
						token.type = ENDFILE;
						break;
					case '+':
						token.type = PLUS;
						break;
					case '-':
						token.type = MINUS;
						break;
					case '*':
						token.type = TIMES;
						break;
					case ';':
						token.type = SEMI;
						break;
					case ',':
						token.type = COMMA;
						break;
					case '(':
						token.type = LPAREN;
						break;
					case ')':
						token.type = RPAREN;
						break;
					case '[':
						token.type = LBRACKET;
						break;
					case ']':
						token.type = RBRACKET;
						break;
					case '{':
						token.type = LBRACE;
						break;
					case '}':
						token.type = RBRACE;
						break;
					default:
						token.type = ERROR;
						break;
					}
				}
			break;
		case INNUM:
			if (!isdigit(ch)) {
				ungetChar();
				push = false;
				state = STATE::DONE;
				token.type = NUM;
			}
			break;
		case INID:
			if (!isalpha(ch)) {
				ungetChar();
				push = false;
				state = STATE::DONE;
				token.type = TokenType::ID;
			}
			break;
		case INSMAL:
			state = STATE::DONE;
			if (ch == '=')
				token.type = TokenType::LEQ;
			else {
				ungetChar();
				push = false;
				token.type = TokenType::LT;
			}
			break;
		case INBIG:
			state = STATE::DONE;
			if (ch == '=')
				token.type = TokenType::GEQ;
			else {
				ungetChar();
				push = false;
				token.type = TokenType::GT;
			}
			break;
		case INEQ:
			state = STATE::DONE;
			if (ch == '=')
				token.type = TokenType::EQ;
			else {
				ungetChar();
				push = false;
				token.type = TokenType::ASSIGN;
			}
			break;
		case INEXCLAM:
			state = STATE::DONE;
			if (ch == '=')
				token.type = TokenType::NEQ;
			else {
				ungetChar();
				push = false;
				token.type = TokenType::ERROR;
			}
			break;
		case INSLASH:
			if (ch == '*')
				state = STATE::INASTERISK1;
			else {
				state = STATE::DONE;
				ungetChar();
				push = false;
				token.type = TokenType::OVER;
			}
			break;
		case INASTERISK1:
			if (ch == '*')
				state = STATE::INASTERISK2;
			else {
				push = false;
				token.type = TokenType::COMMENT;
			}
			break;
		case INASTERISK2:
			if (ch == '/') {	 //×¢ÊÍ
				isComment = true;
				state = STATE::START;
			}
			else if (ch == '*') {
				state = INASTERISK2;
			}
			else {
				state = STATE::INASTERISK1;
				ungetChar();
				push = false;
				token.type = TokenType::COMMENT;
			}
			break;
		case DONE:
		default:
			state = STATE::DONE;
			token.type = TokenType::ERROR;
			break;
		}
		if (push) token.str.push_back(ch);
		if (isComment) {
			token.str.clear();
			isComment = false;
		}
		if (state == STATE::DONE && token.type == TokenType::ID)
			token.type = lookupReserved(token);
	}

	return token;
}
