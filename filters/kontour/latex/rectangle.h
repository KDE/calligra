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

#ifndef __KILLU_RECTANGLE_H__
#define __KILLU_RECTANGLE_H__

#include <qtextstream.h>
#include "element.h"

/***********************************************************************/
/* Class: Rectangle                                                    */
/***********************************************************************/

/**
 * This class hold a rectangle.
 */
class Rectangle: public Element
{
	double _x, _y;
	double _width, _height;
	double _rounding;

	public:
		/**
		 * Constructor
		 *
		 * Creates a new instance of Rectangle.
		 */
		Rectangle();

		/**
		 * Destructor
		 *
		 * Remove the list of headers, footers and the body.
		 */
		virtual ~Rectangle();

		/**
		 * Accessors
		 */
		double   getX       () const { return _x;             }
		double   getY       () const { return _y;             }
		double   getWidth   () const { return _width;         }
		double   getHeight  () const { return _height;        }
		double   getRound   () const { return _rounding;      }

		/**
		 * Modifiors
		 */
		void setX     (double x) { _x        = x;      }
		void setY     (double y) { _y        = y;      }
		void setWidth (double w) { _width    = w;      }
		void setHeight(double h) { _height   = h;      }
		void setRound (double r) { _rounding = r;      }

		void  analyse(const QDomNode);
		void  analyseParam(const QDomNode);

		void  generatePSTRICKS(QTextStream&);

	private:
};

#endif /* __KILLU_RECTANGLE_H__ */
