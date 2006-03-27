
#ifndef __LATEXPARSER_GROUP_H__
#define __LATEXPARSER_GROUP_H__

#include <q3ptrlist.h>

#include "element.h"

class Group: public Element, public Q3PtrList<Element>
{
	public:
		Group();

		~Group();

};

#endif /* __LATEXPARSER_GROUP_H__ */
