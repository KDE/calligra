
#ifndef __LATEXPARSER_TEXT_H__
#define __LATEXPARSER_TEXT_H__

#include "element.h"

#include <qstring.h>

class Text: public Element
{
	public:
		Text();

		~Text();

		QString getText() const { return _text; }

		void setText(QString text) { _text = text; }

	private:
		QString _text;

};

#endif /* __LATEXPARSER_TEXT_H__ */
