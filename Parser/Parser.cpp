// Parser.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "Scanner.h"
#include "Parser.h"

void Parser::syntaxError(string msg) {
	cout << msg << endl;
	hasError = true;
}

void Parser::getToken(){
	curToken = scaner.getToken();
}

void Parser::match(TokenType type) {
	if (curToken.type != type)
		syntaxError("syntax error");
	else
		curToken = scaner.getToken();
}

void Parser::parse() {
	getToken();
	root = shared_ptr<TreeNode> (declaration_list());
}

void Parser::printTree(shared_ptr<TreeNode>  root, int x) {

	while (root != nullptr) {
		for (int i = 0; i < x; ++i)
			out << "   ";

		switch (root->nodetype) {
		case IntK:
			out << "IntK" << endl;
			break;
		case Arry_DeclK:
			out << "Arry_DeclK: " << endl;
			break;
		case FunK:
			out << "FuncK" << endl;
			break;
		case ParamsK:
			out << "ParamsK" << endl;
			break;
		case ParamK:
			out << "ParamK" << endl;
			break;
		case Var_DeclK:
			out << "Var_DeclK" << endl;
			break;
		case VoidK:
			out << "VoidK " << endl;
			break;
		case ArgsK:
			out << "ArgsK " << endl;
			break;
		case CallK:
			out << "CallK " << endl;
			break;
		case CompK:
			out << "CompK " << endl;
			break;
		case Iteration_StmtK:
			out << "While " << endl;
			break;
		case Return_StmtK:
			out << "Return " << endl;
			break;
		case Selection_StmtK:
			out << "If" << endl;
			break;
		case Arry_ElemK:
			out << "Arry_ElemK " << endl;
			break;
		case AssignK:
			out << "Assign " << endl;
			break;
		case ConstK:
			out << "ConstK : " << root->val << endl;
			break;
		case IdK:
			out << "IdK: " << root->name << endl;
			break;
		case UnkownK:
			out << "UnknownK" << endl;
			break;
		case OpK:
			switch (root->op) {
			case TokenType::PLUS:
				out << "Op:+";
				break;
			case TokenType::MINUS:
				out << "Op:-";
				break;
			case TokenType::TIMES:
				out << "Op:*";
				break;
			case TokenType::OVER:
				out << "Op:/";
				break;
			case TokenType::EQ:
				out << "Op:==";
				break;
			case TokenType::GT:
				out << "Op:>";
				break;
			case TokenType::GEQ:
				out << "Op:>=";
				break;
			case TokenType::LT:
				out << "Op:<";
				break;
			case TokenType::LEQ:
				out << "Op:<=";
				break;
			case TokenType::NEQ:
				out << "Op:!=";
				break;
			}
			out << endl;
			break;
		default:
			out << "error" << endl;
			break;
		}
		for (auto t : root->child) {
			if (t != nullptr)
				printTree(t, x + 1);
		}

		root = root->sibling;
	}

}

TreeNode* Parser::declaration_list() {//declaration_list -> declaration_list declaration|declaration
	TreeNode* res = declaration();
	TreeNode* temp = res;

	//过滤掉除int和void之外的token
	while (curToken.type != TokenType::ENDFILE && curToken.type != TokenType::INT &&
		curToken.type != TokenType::VOID && !hasError) {
		syntaxError("declaration error");
		getToken();
	}

	//找到int和void
	while ((curToken.type == TokenType::INT || curToken.type == TokenType::VOID) && !hasError) {
		TreeNode* next = declaration();

		if (next != nullptr)
			if (res == nullptr)
				res = temp = next;
			else {
				temp->sibling = shared_ptr<TreeNode> (next);
				temp = next;
			}
	}
	if (curToken.type != TokenType::ENDFILE)
		syntaxError("syntax error");

	match(TokenType::ENDFILE);
	return res;
}

TreeNode* Parser::declaration() {//declaration -> var_declaration  |  fun_declaration
	TreeNode* typeNode = nullptr;//var_declaration ->type_specifier ID|type_specifier ID[ NUM ]
	TreeNode* idNode = nullptr;//可以为var--id;id[];
	TreeNode* res = nullptr;

	if (curToken.type == TokenType::INT)
		typeNode = new TreeNode(IntK);

	else if (curToken.type == TokenType::VOID)
		typeNode = new TreeNode(VoidK);

	else
		syntaxError("syntax error");

	getToken();
	if (curToken.type == TokenType::ID) {
		idNode = new TreeNode(IdK);
		idNode->name = curToken.str;

		getToken();
		// fun_declaration -> type_specifier ID ( params )  compound_stmt
		if (curToken.type == TokenType::LPAREN) {
			res = new TreeNode(FunK);//FunK-dec

			TreeNode* par = nullptr;
			TreeNode* comp = nullptr;

			getToken();
			par = params();
			match(TokenType::RPAREN);
			comp = compound_stmt();

			res->child.push_back(shared_ptr<TreeNode> (typeNode));
			res->child.push_back(shared_ptr<TreeNode> (idNode));
			res->child.push_back(shared_ptr<TreeNode> (par));
			res->child.push_back(shared_ptr<TreeNode> (comp));
		}													//Var-dec
		else if (curToken.type == TokenType::LBRACKET) {	//ID[NUM]
			res = new TreeNode(Var_DeclK);

			TreeNode* elemNode = new TreeNode(Arry_ElemK);

			getToken();
			match(TokenType::NUM);

			TreeNode* indexNode = new TreeNode(ConstK);
			indexNode->val = std::stoi(curToken.str);//将“9”等转换为9

			elemNode->child.push_back(shared_ptr<TreeNode> (idNode));//添加到list中
			elemNode->child.push_back(shared_ptr<TreeNode> (indexNode));
			res->child.push_back(shared_ptr<TreeNode> (typeNode));
			res->child.push_back(shared_ptr<TreeNode> (elemNode));

			match(TokenType::RBRACKET);
			match(TokenType::SEMI);
		}
		else if (curToken.type == TokenType::SEMI) { //;
			res = new TreeNode(Var_DeclK);
			res->child.push_back(shared_ptr<TreeNode> (typeNode));
			res->child.push_back(shared_ptr<TreeNode> (idNode));
			getToken();
		}
		else
			syntaxError("syntax error");
	}
	else
		syntaxError("syntax error");

	return res;
}

TreeNode* Parser::params() {//params -> param_list  |  VOID
	TreeNode* paramNode = new TreeNode(ParamsK);

	if (curToken.type == TokenType::VOID) {
		getToken();

		TreeNode* typeNode = new TreeNode(VoidK);
		if (curToken.type == TokenType::ID)//参数列表为void id,……
			paramNode->child.push_back(shared_ptr<TreeNode> (param_list((typeNode))));
		else//参数列表为（void)
			paramNode->child.push_back(shared_ptr<TreeNode> (typeNode));
	}
	else if (curToken.type == TokenType::INT)//参数列表为int id,……
		paramNode->child.push_back(shared_ptr<TreeNode> (param_list(nullptr)));
	else
		syntaxError("parameters error");

	return paramNode;
}

TreeNode* Parser::param_list(TreeNode* node) {//param_list -> param_list , param  |  param
	TreeNode* res = param(node);
	TreeNode* temp = res;

	node = nullptr;
	while (curToken.type == TokenType::COMMA) { // ,
		getToken();

		TreeNode* next = param(node);
		if (next != nullptr)
			if (res == nullptr)
				res = temp = next;
			else {
				temp->sibling = shared_ptr<TreeNode> (next);
				temp = next;
			}
	}
	return res;
}

TreeNode* Parser::param(TreeNode* node) {//param->type_specifier ID|type_specifier ID[ ]
	TreeNode* paramsNode = new TreeNode(ParamK);
	TreeNode* typeNode = nullptr;
	TreeNode* idNode = nullptr;

	if (node == nullptr && curToken.type == TokenType::INT) {
		typeNode = new TreeNode(IntK);
		getToken();
	}
	else if (node == nullptr && curToken.type == TokenType::VOID) {
		typeNode = new TreeNode(VoidK);
		getToken();
	}
	else if (node != nullptr)
		typeNode = node;

	if (typeNode != nullptr) {
		paramsNode->child.push_back(shared_ptr<TreeNode> (typeNode));

		if (curToken.type == TokenType::ID) {
			idNode = new TreeNode(IdK);
			idNode->name = curToken.str;
			paramsNode->child.push_back(shared_ptr<TreeNode> (idNode));
			getToken();
		}
		else
			syntaxError("parameter error");

		if (curToken.type == TokenType::LBRACKET && paramsNode->child.size() > 1) {
			getToken();//  [ ]
			match(TokenType::RBRACKET);
		}
	}
	else
		syntaxError("parameter error");

	return paramsNode;
}

TreeNode* Parser::compound_stmt() {//compound_stmt->{ local_declarations statement_list }
	TreeNode* compNode = new TreeNode(CompK);//复合语句

	match(TokenType::LBRACE);// {

	compNode->child.push_back(shared_ptr<TreeNode> (local_declaration()));
	compNode->child.push_back(shared_ptr<TreeNode> (statement_list()));

	match(TokenType::RBRACE);

	return compNode;
}

TreeNode* Parser::local_declaration() {//local_declarations->local_declarations var_declaration|empty
	TreeNode* res = nullptr;		//statement_list -> statement_list statement  |  empty
	TreeNode* temp = nullptr;
	TreeNode* node = nullptr;

	while (curToken.type == TokenType::INT || curToken.type == TokenType::VOID) {

		temp = new TreeNode(Var_DeclK);	//var_decl

		if (curToken.type == TokenType::INT) {
			temp->child.push_back(shared_ptr<TreeNode> (new TreeNode(IntK)));
			getToken();
		}
		else if (curToken.type == TokenType::VOID) {
			temp->child.push_back(shared_ptr<TreeNode> (new TreeNode(VoidK)));
			getToken();
		}

		if (temp != nullptr && curToken.type == TokenType::ID) {
			TreeNode* idNode = new TreeNode(IdK);
			idNode->name = curToken.str;

			getToken();
			if (curToken.type == TokenType::LBRACKET) {  // [
				TreeNode* elemNode = new TreeNode(Arry_ElemK);

				getToken();
				match(TokenType::NUM);

				TreeNode* indexNode = new TreeNode(ConstK);
				indexNode->val = stoi(curToken.str);//转换为int

				elemNode->child.push_back(shared_ptr<TreeNode> (idNode));//j加到链表末尾
				elemNode->child.push_back(shared_ptr<TreeNode> (indexNode));
				temp->child.push_back(shared_ptr<TreeNode> (elemNode));

				match(TokenType::RBRACKET);// ]
				match(TokenType::SEMI);
			}
			else if (curToken.type == TokenType::SEMI)
				getToken();
			else
				getToken();
		}
		else
			syntaxError("syntax error");

		if (temp != nullptr) {
			if (res == nullptr)
				res = node = temp;
			else {
				node->sibling = shared_ptr<TreeNode>(temp);
				node = temp;
			}
		}
	}

	return res;
}

TreeNode* Parser::statement_list() {
	TreeNode* res = statement();
	TreeNode* temp = res;

	while (curToken.type == TokenType::IF || curToken.type == TokenType::ID ||
		curToken.type == TokenType::LBRACE || curToken.type == TokenType::WHILE ||
		curToken.type == TokenType::RETURN || curToken.type == TokenType::SEMI ||
		curToken.type == TokenType::LPAREN || curToken.type == TokenType::NUM) {

		TreeNode* next = statement();

		if (next != nullptr)
			if (res == nullptr)
				res = temp = next;
			else {
				temp->sibling = shared_ptr<TreeNode> (next);
				temp = next;
			}
	}

	return res;
}

TreeNode* Parser::statement() { //statement -> expression_stmt  |  compound_stmt 
								//| selection_stmt | iteration_stmt| return_stmt
	TreeNode* treeNode = nullptr;	

	switch (curToken.type) {
	case TokenType::IF:
			treeNode = selection_stmt();
			break;
		case TokenType::WHILE:
			treeNode = iteration_stmt();
			break;
		case TokenType::RETURN:
			treeNode = return_stmt();
			break;
		case TokenType::LBRACE://	{
			treeNode = compound_stmt();
			break;
		case ID:
		case SEMI:
		case LPAREN:
		case NUM:
			treeNode = expression_stmt();//等于四者的时候
			break;
		default:
			syntaxError("syntax error");
			getToken();
	}
	return treeNode;
}

TreeNode* Parser::selection_stmt() {// if(expression) statement|if(expression) statement else statment

	TreeNode* slecNode = new TreeNode(Selection_StmtK);

	match(TokenType::IF);
	match(TokenType::LPAREN);
	slecNode->child.push_back(shared_ptr<TreeNode> (expression()));

	match(TokenType::RPAREN);
	slecNode->child.push_back(shared_ptr<TreeNode> (statement()));

	//有ELSE的话，继续
	if (curToken.type == TokenType::ELSE) {
		getToken();
		slecNode->child.push_back(shared_ptr<TreeNode> (statement()));
	}

	return slecNode;
}

TreeNode* Parser::expression_stmt() {//expression_stmt -> expression ;  |  ;
	TreeNode* res = nullptr;

	if (curToken.type != TokenType::SEMI)
		res = expression();

	match(TokenType::SEMI);

	return res;
}

TreeNode* Parser::iteration_stmt() {// iteration_stmt->WHILE(expression) statement
	
	TreeNode* itNode = new TreeNode(Iteration_StmtK);

	match(TokenType::WHILE);
	match(TokenType::LPAREN);

	if (itNode != nullptr)
		itNode->child.push_back(shared_ptr<TreeNode> (expression()));
	match(TokenType::RPAREN);

	if (itNode != nullptr)
		itNode->child.push_back(shared_ptr<TreeNode> (statement()));

	return itNode;
}

TreeNode* Parser::return_stmt() {//return_stmt -> RETURN  |  RETURN expression

	TreeNode* retNode = new TreeNode(Return_StmtK);

	match(TokenType::RETURN);
	if (curToken.type == TokenType::SEMI)
		getToken();
	else if (retNode != nullptr) {
		retNode->child.push_back(shared_ptr<TreeNode> (expression()));
		match(TokenType::SEMI);
	}

	return retNode;
}

TreeNode* Parser::expression() {//expression -> var = expression  |  simple_expression

	TreeNode* res = var();

	if (res == nullptr)//var为空，则为simple-expression
		res = simple_expression(nullptr);
	else {
		TreeNode* expNode = nullptr;

		if (curToken.type == TokenType::ASSIGN) {
			expNode = new TreeNode(AssignK);
			expNode->name = curToken.str;

			getToken();
			expNode->child.push_back(shared_ptr<TreeNode> (res));
			expNode->child.push_back(shared_ptr<TreeNode> (expression()));

			res = expNode;
		}
		else
			res = simple_expression(res);
	}

	return res;
}

TreeNode* Parser::var() {	//var -> ID  |  ID [ expression ]
	TreeNode* res = nullptr;

	while (curToken.type == TokenType::ID) {
		TreeNode* expNode = new TreeNode(IdK);

		expNode->name = curToken.str;
		getToken();
		if (curToken.type == TokenType::LBRACKET) { // [
			getToken();
			TreeNode* temp = expression();
			match(TokenType::RBRACKET); // ]
			res = new TreeNode(Arry_ElemK);
			res->child.push_back(shared_ptr<TreeNode> (expNode));
			res->child.push_back(shared_ptr<TreeNode> (temp));
		}
		else
			res = expNode;
	}

	return res;
}

TreeNode* Parser::simple_expression(TreeNode* node) { 
						// simple_expression -> additive_expression relop additive_expression 
	TreeNode* res = additive_expression(node);		//	|additive_expression
	node = nullptr;

	switch (curToken.type) {
	case TokenType::LT:
	case TokenType::LEQ:
	case TokenType::GT:
	case TokenType::GEQ:
	case TokenType::EQ:
	case TokenType::NEQ:
		TreeNode* temp = new TreeNode(OpK);
		temp->op = curToken.type;
		temp->child.push_back(shared_ptr<TreeNode> (res));

		getToken();
		temp->child.push_back(shared_ptr<TreeNode> (additive_expression(node)));
		res = temp;
	}

	return res;
}

TreeNode* Parser::additive_expression(TreeNode* node) {// additive_expression->additive_expression addop term
	TreeNode* res = term(node);
	node = nullptr;

	while (curToken.type == TokenType::PLUS || curToken.type == TokenType::MINUS) {

		TreeNode* temp = new TreeNode(OpK);

		if (temp != nullptr) {
			temp->op = curToken.type;
			temp->child.push_back(shared_ptr<TreeNode> (res));

			getToken();
			temp->child.push_back(shared_ptr<TreeNode> (term(node)));
			res = temp;
		}
	}

	return res;
}

TreeNode* Parser::term(TreeNode* node) {//term -> term mulop factor  |  factor
	TreeNode* res = factor(node);	// * or /
	node = nullptr;

	while (curToken.type == TokenType::TIMES || curToken.type == TokenType::OVER) {

		TreeNode* temp = new TreeNode(OpK);

		temp->op = curToken.type;
		temp->child.push_back(shared_ptr<TreeNode> (res));

		getToken();
		temp->child.push_back(shared_ptr<TreeNode> (factor(node)));
		res = temp;
	}

	return res;
}

TreeNode* Parser::factor(TreeNode* node) {// factor ->( expression )|var|call|NUM
										//call -> ID ( args )
	TreeNode* res = nullptr;

	if (node != nullptr)//node为传下来的以ID开头的var，可能为call或者var
		if (curToken.type == TokenType::LPAREN && node->nodetype!= Arry_ElemK)
			res = call(node);
		else
			res = node;
	else {
		switch (curToken.type) {
		case TokenType::LPAREN://左括号
			getToken();
			res = expression();
			match(TokenType::RPAREN);//右括号
			break;
		case TokenType::NUM:
			res = new TreeNode(ConstK);
			if (res != nullptr)
				res->val = stoi(curToken.str);
			getToken();
			break;
		case TokenType::ID:
			node = var();
			if (curToken.type == TokenType::LPAREN && node->nodetype != Arry_ElemK)
				res = call(node);
			res = node;
			break;
		default:
			syntaxError("syntax error");
			getToken();
			break;
		}
	}

	return res;
}

TreeNode* Parser::call(TreeNode* node) {//call -> ID ( args )
	TreeNode* res = new TreeNode(CallK);

	if (node != nullptr)
		res->child.push_back(shared_ptr<TreeNode> (node));

	match(TokenType::LPAREN);

	if (curToken.type == TokenType::RPAREN)
		getToken();
	else if (node != nullptr) {
		res->child.push_back(shared_ptr<TreeNode> (args()));
		match(TokenType::RPAREN);
	}

	return res;
}

TreeNode* Parser::args() {// args -> arg_list  |  empty
	TreeNode* res = new TreeNode(ArgsK);
	TreeNode* expNode = res;

	if (curToken.type != TokenType::RPAREN) {  // )
		expNode = expression();
		TreeNode* temp = expNode;

		while (curToken.type == TokenType::COMMA) { // ,
			getToken();

			TreeNode* next = expression();
			if (next != nullptr)
				if (expNode == nullptr)
					expNode = temp = next;
				else {
					temp->sibling = shared_ptr<TreeNode> (next);
					temp = next;
				}
		}
	}
	if (expNode != nullptr)
		res->child.push_back(shared_ptr<TreeNode> (expNode));

	return res;
}

TreeNode* Parser::arg_list() {// arg_list ->arg_list ,expression | expression
	TreeNode* res = new TreeNode(AssignK);//expression->var=expression|simple_expression
											//var -> ID  |  ID [ expression ]
	if (curToken.type == TokenType::ID)
		res->name = curToken.str;

	match(TokenType::ID);
	match(TokenType::ASSIGN);

	res->child.push_back(shared_ptr<TreeNode> (expression()));

	return res;
}