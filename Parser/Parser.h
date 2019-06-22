#pragma once
#include"pch.h"
#include"Scanner.h"

class Parser {
public:
	shared_ptr<TreeNode> root;//创建类型为TreeNode的智能指针

	Parser(fstream& _source,fstream& _out) : scaner(_source),out(_out){};//初始化
	Parser(Parser&) = delete;

	void parse();//语法分析
	void printTree(shared_ptr<TreeNode> root, int x=0);//打印语法树

private:
	fstream& out;
	Scaner scaner;
	Token curToken;
	bool hasError;

	void syntaxError(string msg);//错误提示
	void getToken();//get token
	void match(TokenType type);//判断是否match

	TreeNode* declaration_list();
	TreeNode* declaration();
	TreeNode* params();
	TreeNode* param_list(TreeNode* node);
	TreeNode* param(TreeNode* node);
	TreeNode* compound_stmt();
	TreeNode* local_declaration();
	TreeNode* statement_list();
	TreeNode* statement();
	TreeNode* expression_stmt();
	TreeNode* selection_stmt();
	TreeNode* iteration_stmt();
	TreeNode* return_stmt();
	TreeNode* expression();
	TreeNode* simple_expression(TreeNode* node);
	TreeNode* additive_expression(TreeNode* node);
	TreeNode* term(TreeNode* node);
	TreeNode* factor(TreeNode* node);
	TreeNode* var();
	TreeNode* call(TreeNode* node);
	TreeNode* args();
	TreeNode* arg_list();

};

