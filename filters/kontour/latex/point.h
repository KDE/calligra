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

#ifndef __KILLU_POINT_H__
#define __KILLU_POINT_H__

#include <qtextstream.h>

#include "xmlparser.h"

/***********************************************************************/
/* Class: Point                                                         */
/***********************************************************************/

/**
 * This class hold a point.
 */
class Point: public XmlParser
{
	double  _x;
	double  _y;

	public:
		/**
		 * Constructor
		 *
		 * Creates a new instance of Point.
		 */
		Point();

		/**
		 * Destructor
		 *
		 */
		virtual ~Point();

		/**
		 * Accessors
		 */
		double getX     () const { return _x;    }
		double getY     () const { return _y;    }

		/**
		 * Modifiors
		 */
		void   setX        (double x) { _x    = x; }
		void   setY        (double y) { _y    = y; }

		void   analyse(const QDomNode);

		void   generate(QTextStream&);

	private:
};

#endif /* __KILLU_POINT_H__ */
