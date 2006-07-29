
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
	kdDebug(30522) << argv[1] << endl;
	kdDebug(30522) << argv[2] << endl;

	QString fileIn = argv[1];
	QString fileOut = argv[2];
	
	kdDebug(30522) << "TEXGRAPH FILTER --> BEGIN" << endl;
	Xml2LatexParser parser(fileIn, fileOut);
	parser.analyse();
	kdDebug(30522) << "---------- generate file -------------" << endl;
	parser.generate();
	kdDebug(30522) << "TEXGRAPH FILTER --> END" << endl;
}
