
#ifndef __LATEXPARSER_COMMENT_H__
#define __LATEXPARSER_COMMENT_H__

#include "element.h"

#include <qstring.h>

class Comment: public Element
{
	public:
		Comment();

		~Comment();

		QString getComment() const { return _comment; }

		void setComment(QString comment) { _comment = comment; }

	private:
		QString _comment;

};

#endif /* __LATEXPARSER_COMMENT_H__ */
