
#ifndef __LATEXPARSER_ENV_H__
#define __LATEXPARSER_ENV_H__

#include "element.h"

#include <qstring.h>

class Env: public Element
{
	public:
		Env();

		Env(QString);

		~Env();

		QString getName() const { return _name; }

		void setName(QString name) { _name = name; }

	private:
		QString _name;
};

#endif /* __LATEXPARSER_ENV_H__ */
