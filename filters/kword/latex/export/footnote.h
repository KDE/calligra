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
** Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
**
*/

#ifndef __KWORD_LATEX_FOOTNOTE_H__
#define __KWORD_LATEX_FOOTNOTE_H__

#include <QString>
//Added by qt3to4:
#include <QTextStream>
#include "format.h"

/***********************************************************************/
/* Class: Footnote                                                     */
/***********************************************************************/

/**
 * This class hold a footnote. This class will be a lot modified because
 * it doesn't correspond whith the dtd 1.0.
 */
class Footnote: public Format
{
	int     _from,   _to;
	int     _start,  _end;
	QString _before, _after, _space;	// TODO: Change in QChar
	QString _ref;

	public:
		/**
		 * Constructors
		 *
		 * Creates a new instances of Footnote.
		 *
		 * @param para is the parent class
		 */
		Footnote(Para* para = 0);
		//Footnote(TextZone);

		/* 
		 * Destructor
		 *
		 * Nothing to do
		 */
		virtual ~Footnote();

		/**
		 * Accessors
		 */

		/**
		 * Modifiers
		 */
		void setFrom  (int   f) { _from   = f; }
		void setTo    (int   t) { _to     = t; }
		void setSpace (QString);
		void setStart (int   s) { _start  = s; }
		void setEnd   (int   e) { _end    = e; }
		void setBefore(QString);
		void setAfter (QString);
		void setRef   (QString);

		/**
		 * Helpfull functions
		 */
		void analyse (const QDomNode);
		void analyseInternal(const QDomNode);
		void analyseRange(const QDomNode);
		void analyseText(const QDomNode);
		void analyseDescript(const QDomNode);

		void generate(QTextStream&);

	//private:
};


#endif /* __KWORD_LATEX_FOOTNOTE_H__ */

