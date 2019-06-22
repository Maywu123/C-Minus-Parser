// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件


#ifndef PCH_H
#define PCH_H

#include <fstream>
#include <string>
#include <iostream>
#include <cctype>
#include <cstring>
#include <list>
#include <memory>
using namespace std;

typedef enum {
	ELSE,
	IF,
	INT,
	RETURN,
	VOID,
	WHILE,
	ID,
	NUM,
	PLUS,
	MINUS,
	TIMES,
	OVER,
	SEMI,//,
	COMMA,//逗号
	LT,
	LEQ,
	GT,
	GEQ,
	EQ,
	NEQ,
	ASSIGN,//
	LPAREN,//(
	RPAREN,//)
	LBRACKET,//[
	RBRACKET,//]
	LBRACE,//{
	RBRACE,//}
	COMMENT,
	ERROR,
	ENDFILE
} TokenType;


typedef enum { 

	Var_DeclK, Arry_DeclK, FunK, ParamsK, ParamK, VoidK, IntK, UnkownK,
	CallK, ArgsK, CompK, Selection_StmtK, Iteration_StmtK, Return_StmtK,
	AssignK, OpK, ConstK, IdK, Arry_ElemK

}NodeType;

struct TreeNode {

	list<shared_ptr<struct TreeNode >> child;//子节点，为list
	shared_ptr < struct TreeNode>  sibling;//兄弟节点
	int line;
	NodeType nodetype;

	// 状态属性
	TokenType op;//op是+ or - or < ……
	int val;
	string name;

	TreeNode(NodeType nodetype) {
		this->nodetype = nodetype;
	}//赋值，初始化

};

#endif //PCH_H
