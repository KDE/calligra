/* A PARA IS A TITLE, A SET OF WORDS OR A LIST. TO KNOW ITS TYPE,
 * YOU MUST LOOK AT IN THE LAYOUT CLASS.
 */
/*
** Header file for inclusion with kword_xml2latex.c
**
** Copyright (C) 2000 Robert JACOLIN
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** To receive a copy of the GNU Library General Public License, write to the
** Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA  02111-1307, USA.
**
*/

#ifndef __KWORD_FOOTNOTE_H__
#define __KWORD_FOOTNOTE_H__

#include <qstring.h>
#include "format.h"

class Footnote: public Format
{
	int     _from,   _to;
	int     _start,  _end;
	char    _before, _after, _space;
	QString _ref;

	public:
		Footnote(Para* para = 0);
		//Footnote(TextZone);

		virtual ~Footnote();

		/* Modifiers */
		void setFrom  (int   f) { _from   = f; }
		void setTo    (int   t) { _to     = t; }
		void setSpace (char*);
		void setStart (int   s) { _start  = s; }
		void setEnd   (int   e) { _end    = e; }
		void setBefore(char*);
		void setAfter (char*);
		void setRef   (char*);

		void analyse (const Markup*);
		void analyseInternal(const Markup*);
		void analyseRange(const Markup*);
		void analyseText(const Markup*);
		void analyseDescript(const Markup*);

		void generate(QTextStream&);

	//private:
};


#endif /* __KWORD_FOOTNOTE_H__ */

