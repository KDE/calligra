#ifndef WORDFILTER_H
#define WORDFILTER_H

#include <kdebug.h>
#include "winworddoc.h"
#include "kworddoc.h"
#include "myfile.h"

class QString;


class WordFilter {

public:
	WordFilter(myFile in);
	~WordFilter();
	bool filterIt();
	QString kwdFile();
	
private:
	WinWordDoc *myDoc;
	KWordDoc *myKwd;
};
#endif // WORDFILTER_H
