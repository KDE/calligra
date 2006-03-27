
#include "latexparser.h"

#include <fstream.h>
#include <iostream.h>
//Added by qt3to4:
#include <Q3PtrList>

//extern void yylex();
extern Q3PtrList<Element>* _root;
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

Q3PtrList<Element>* LatexParser::parse()
{
	texparse(_filename);
	return _root;
}
