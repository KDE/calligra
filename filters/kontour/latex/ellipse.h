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

#ifndef __KILLU_ELLIPSE_H__
#define __KILLU_ELLIPSE_H__

#include <qtextstream.h>
#include "element.h"

enum EKind
{
	EK_FULL,
	EK_ARC,
	EK_PIE
};

/***********************************************************************/
/* Class: Ellipse                                                      */
/***********************************************************************/

/**
 * This class hold a ellipse.
 */
class Ellipse: public Element
{
	double _x, _y;
	double _rx, _ry;
	double _angle1, _angle2;
	EKind  _kind;

	public:
		/**
		 * Constructor
		 *
		 * Creates a new instance of Ellipse.
		 */
		Ellipse();

		/**
		 * Destructor
		 *
		 */
		virtual ~Ellipse();

		/**
		 * Accessors
		 */
		EKind    getKind  () const { return _kind;   }
		double   getX     () const { return _x;      }
		double   getY     () const { return _y;      }
		double   getRx    () const { return _rx;     }
		double   getRy    () const { return _ry;     }
		double   getAngle1() const { return _angle1; }
		double   getAngle2() const { return _angle2; }

		/**
		 * Modifiors
		 */
		void setKind  (int k)     { _kind   = (EKind) k; }
		void setX     (double x)  { _x      = x;         }
		void setY     (double y)  { _y      = y;         }
		void setRx    (double rx) { _rx     = rx;        }
		void setRy    (double ry) { _ry     = ry;        }
		void setAngle1(double a)  { _angle1 = a;         }
		void setAngle2(double a)  { _angle2 = a;         }

		void  analyse(const QDomNode);

		void  generatePSTRICKS(QTextStream&);

	private:
};

#endif /* __KILLU_ELLIPSE_H__ */
