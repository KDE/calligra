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

#ifndef __KILLU_TEXT_H__
#define __KILLU_TEXT_H__

#include <qlist.h>
#include <qtextstream.h>

#include "element.h"
#include "font.h"

enum EAlign
{
	EA_LEFT,
	EA_CENTER,
	EA_RIGHT
};

/***********************************************************************/
/* Class: Text                                                         */
/***********************************************************************/

/**
 * This class hold a text.
 */
class Text: public Element
{
	double      _x, _y;
	EAlign      _align;
	QList<Font> _fonts;
	QString     _text;

	public:
		/**
		 * Constructor
		 *
		 * Creates a new instance of Text.
		 */
		Text();

		/**
		 * Destructor
		 *
		 */
		virtual ~Text();

		/**
		 * Accessors
		 */
		double   getX     () const { return _x;        }
		double   getY     () const { return _y;        }
		EAlign   getAlign () const { return _align;    }

		/**
		 * Modifiors
		 */
		void setX     (double x) { _x        = x;          }
		void setY     (double y) { _y        = y;          }
		void setAlign (int a)    { _align    = (EAlign) a; }

		void  analyse(const QDomNode);

		void  generatePSTRICKS(QTextStream&);

	private:
		void  analyseParam(const QDomNode);
};

#endif /* __KILLU_TEXT_H__ */
