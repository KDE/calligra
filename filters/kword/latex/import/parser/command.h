

#ifndef __LATEXPARSER_COMMAND_H__
#define __LATEXPARSER_COMMAND_H__

#include "stack.h"

#include "param.h"
#include "option.h"
#include "element.h"

#include <qstring.h>
#include <qptrlist.h>

class Command: public Element
{
	public:
		Command();

		~Command();

		QString getName() const { return _name; }

		QPtrList<Param> getParams() const { return _params; }
		
		QPtrList<Option> getOptions() const { return _options; }
		
		QPtrList<Element> getChildren() const { return _elements; }

	private:
		QString _name;
		QPtrList<Param> _params;
		QPtrList<Option> _options;
		QPtrList<Element> _elements;

};

#endif /* __LATEXPARSER_COMMAND_H__ */

