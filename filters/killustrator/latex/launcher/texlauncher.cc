
#include "document.h"
#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#include <koFilter.h>
#include <koStore.h>

int main(int argc, char* argv[])
{
	kdDebug() << argv[0] << endl;
	kdDebug() << argv[1] << endl;

	QString fileIn = argv[0];
	QString fileOut = argv[1];

	KoStore in = KoStore(QString(fileIn), KoStore::Read);
	if(!in.open("root")) 
	{
		kdError(30503) << "Unable to open input file!" << endl;
		in.close();
		return false;
	}
	QByteArray array=in.read(0xffffffff);
	QString buf = QString::fromUtf8((const char*)array, array.size());
	in.close();

	int begin = buf.find( "<DOC" ); // skip <?...?>
	buf.remove(0, begin);
	kdDebug() << "TEXGRAPH FILTER --> BEGIN" << endl;
	Document TEXGRAPHParser(fileOut, buf.latin1());
	TEXGRAPHParser.analyse();
	kdDebug() << "---------- generate file -------------" << endl;
	TEXGRAPHParser.generate();
	kdDebug() << "TEXGRAPH FILTER --> END" << endl;

}
