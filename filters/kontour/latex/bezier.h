/*
** Header file for inclusion with killu_xml2latex.c
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

#ifndef __KILLU_BEZIER_H__
#define __KILLU_BEZIER_H__

#include <qlist.h>
#include <qtextstream.h>
#include "element.h"
#include "point.h"
#include "polyline.h"

/***********************************************************************/
/* Class: Bezier                                                       */
/***********************************************************************/

/**
 * This class hold a Bezier.
 */
class Bezier: public Element
{
	EArrow _arrow1, _arrow2;
	bool   _closed;
	QList<Point> _points;

	public:
		/**
		 * Constructor
		 *
		 * Creates a new instance of Bezier.
		 */
		Bezier();

		/**
		 * Destructor
		 *
		 */
		virtual ~Bezier();

		/**
		 * Accessors
		 */
		EArrow   getArrow1() const { return _arrow1;   }
		EArrow   getArrow2() const { return _arrow2;   }
		bool     getClosed() const { return _closed;   }

		/**
		 * Modifiors
		 */
		void setArrow1(int a)  { _arrow1   = (EArrow) a; }
		void setArrow2(int a)  { _arrow2   = (EArrow) a; }
		void setClosed(bool c) { _closed   = c;          }

		void analyse(const QDomNode);

		void generatePSTRICKS(QTextStream&);
		void generatePSTRICKSParam(QTextStream&);

	private:
		void  analyseParam(const QDomNode);
};

#endif /* __KILLU_BEZIER_H__ */
