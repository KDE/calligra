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

#ifndef __KILLU_POLYLINE_H__
#define __KILLU_POLYLINE_H__

#include <qtextstream.h>
#include "element.h"
#include <qlist.h>
#include "point.h"

enum EArrow
{
	EF_NONE
};

/***********************************************************************/
/* Class: Polyline                                                     */
/***********************************************************************/

/**
 * This class hold a Polyline.
 */
class Polyline: public Element
{
	EArrow _arrow1, _arrow2;
	protected:
		QList<Point> _points;

	public:
		/**
		 * Constructor
		 *
		 * Creates a new instance of Polyline.
		 */
		Polyline();

		/**
		 * Destructor
		 *
		 */
		virtual ~Polyline();

		/**
		 * Accessors
		 */
		EArrow   getArrow1() const { return _arrow1;   }
		EArrow   getArrow2() const { return _arrow2;   }

		/**
		 * Modifiors
		 */
		void setArrow1(int a) { _arrow1   = (EArrow) a; }
		void setArrow2(int a) { _arrow2   = (EArrow) a; }

		void analyse(const QDomNode);

		void generatePSTRICKS(QTextStream&);
		void generatePSTRICKSParam(QTextStream&);

	private:
		void analyseParam(const QDomNode);
};

#endif /* __KILLU_POLYLINE_H__ */
