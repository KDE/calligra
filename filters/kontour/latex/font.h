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

#ifndef __KILLU_FONT_H__
#define __KILLU_FONT_H__

#include <qtextstream.h>
#include <qstring.h>

#include "xmlparser.h"

/***********************************************************************/
/* Class: Font                                                         */
/***********************************************************************/

/**
 * This class hold a font.
 */
class Font: public XmlParser
{
	QString _face;
	double  _pointSize;
	double  _weight;

	public:
		/**
		 * Constructor
		 *
		 * Creates a new instance of Font.
		 */
		Font();

		/**
		 * Destructor
		 *
		 */
		virtual ~Font();

		/**
		 * Accessors
		 */
		QString getFace     () const { return _face;       }
		double  getPointSize() const { return _pointSize;  }

		/**
		 * Modifiors
		 */

		void   setFace     (QString f) { _face      = f;  }
		void   setPointSize(double ps) { _pointSize = ps; }
		void   setWeight   (double w)  { _weight    = w;  }
		void   analyse(const QDomNode);

		void  generatePSTRICKS(QTextStream&);

	private:
		void  analyseParam(const QDomNode);
};

#endif /* __KILLU_FONT_H__ */
