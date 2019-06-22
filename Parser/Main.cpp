#include"pch.h"
#include"Scanner.h"
#include"Parser.h"


int main() {

	fstream in;
	in.open("gcd.txt", std::ios::in);
	fstream out;
	out.open("result.txt", std::ios::out);

	cout << "�﷨������ʼ" << endl;

	out << endl;
	out << "C- PARSING: gcd-result.txt" << endl;
	out << endl;
	out << "Syntax tree:" << endl;

	Parser parser(in,out);
	parser.parse();

	parser.printTree(parser.root);

	cout << "�﷨��������" << endl;
	system("PAUSE");

	return 0;
}