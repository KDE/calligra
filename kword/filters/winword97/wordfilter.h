#ifndef WORDFILTER_H
#define WORDFILTER_H

#include <qstring.h>
#include <kdebug.h>
#include "winworddoc.h"
#include "kworddoc.h"
#include "myfile.h"

class WordFilter {

public:
	WordFilter(myFile in);
	~WordFilter();
	bool filterIt();
	QString kwdFile();
	
private:
	myFile doc;   // raw data + length info, see myfile.h
	QString kwd;
	WinWordDoc *myDoc;
	KWordDoc *myKwd;
};
#endif // WORDFILTER_H
