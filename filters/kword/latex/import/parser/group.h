
#ifndef __LATEXPARSER_GROUP_H__
#define __LATEXPARSER_GROUP_H__

#include <qptrlist.h>

#include "element.h"

class Group: public Element, public QPtrList<Element>
{
	public:
		Group();

		~Group();

};

#endif /* __LATEXPARSER_GROUP_H__ */
