

#ifndef __LATEXPARSER_ELEMENT_H__
#define __LATEXPARSER_ELEMENT_H__

#include "stack.h"

class Element
{
	public:
		Element();

		~Element();

		EType getType() const { return _type; }

	private:
		EType _type;

};

#endif /* __LATEXPARSER_ELEMENT_H__ */

