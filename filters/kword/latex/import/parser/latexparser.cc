
#include "latexparser.h"

#include <fstream.h>
#include <iostream.h>

//extern void yylex();
extern QPtrList<Element>* _root;
extern bool texparse(QString);

LatexParser::LatexParser()
{
}

LatexParser::LatexParser(QString fileIn)
{
	_filename= fileIn;
}

LatexParser::~LatexParser()
{
}

QPtrList<Element>* LatexParser::parse()
{
	texparse(_filename);
	return _root;
}
