
#include "document.h"
#include "texgraphexport.h"
#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#include <koFilter.h>
#include <koStore.h>
#include<qtextstream.h>


int main(int argc, char* argv[])
{
	kdDebug() << argv[1] << endl;
	kdDebug() << argv[2] << endl;

	QString fileIn = argv[1];
	QString fileOut = argv[2];

	kdDebug() << "TEXGRAPH FILTER --> BEGIN" << endl;
	Document TEXGRAPHParser(fileIn, fileOut);
	TEXGRAPHParser.analyse();
	kdDebug() << "---------- generate file -------------" << endl;
	TEXGRAPHParser.generate();
	kdDebug() << "TEXGRAPH FILTER --> END" << endl;
}
