#ifndef WINWORDDOC_H
#define WINWORDDOC_H

#include "myfile.h"

class WinWordDoc {

public:
	WinWordDoc(myFile in);
	~WinWordDoc();
	
private:
	myFile doc;   // raw data
};
#endif // WINWORDDOC_H
