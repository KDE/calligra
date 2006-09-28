
#include "xml2latexparser.h"
//#include "texgraphexport.h"
#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#include <KoFilter.h>
#include <KoStore.h>
#include<qtextstream.h>


int main(int argc, char* argv[])
{
	kDebug(30522) << argv[1] << endl;
	kDebug(30522) << argv[2] << endl;

	QString fileIn = argv[1];
	QString fileOut = argv[2];
	
	kDebug(30522) << "TEXGRAPH FILTER --> BEGIN" << endl;
	Xml2LatexParser parser(fileIn, fileOut);
	parser.analyze();
	kDebug(30522) << "---------- generate file -------------" << endl;
	parser.generate();
	kDebug(30522) << "TEXGRAPH FILTER --> END" << endl;
}
