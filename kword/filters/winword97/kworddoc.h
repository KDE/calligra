#ifndef KWORDDOC_H
#define KWORDDOC_H

#include <qstring.h>

class KWordDoc {

public:
	KWordDoc();
	~KWordDoc();
	QString kwdFile();
	
private:
	QString kwd;	
};
#endif // KWORDDOC_H
