
#ifndef __LATEXPARSER_ENV_H__
#define __LATEXPARSER_ENV_H__

#include "element.h"

#include <qstring.h>
#include <qptrlist.h>

class Env: public Element
{
	QPtrList<Element> _children;
	QString _name;

	public:
		Env();

		Env(const char*);

		Env(QString);

		~Env();

		QString getName() const { return _name; }

		void setName(QString name) { _name = name; }
		void setChildren(QPtrList<Element>*);
		
};

#endif /* __LATEXPARSER_ENV_H__ */
