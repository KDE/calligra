
/*
** Header file for inclusion with kspread_xml2latex.c
**
** Copyright (C) 2003 Robert JACOLIN
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

#ifndef __KSPREAD_LATEX_PEN_H__
#define __KSPREAD_LATEX_PEN_H__

#include <qtextstream.h>
#include <qstring.h>
#include <qcolor.h>
#include "xmlparser.h"

/***********************************************************************/
/* Class: Pen                                                          */
/***********************************************************************/

/**
 * This class describe a pen which is used to draw borders.
 */
class Pen: public XmlParser
{
	double _width;
	int _style;
	QColor _color;

	public:
		/**
		 * Constructors
		 *
		 * Creates a new instance of Format.
		 */
		Pen();

		/* 
		 * Destructor
		 *
		 * Nothing to do
		 */
		virtual ~Pen() {}

		/**
		 * getters
		 */
		double getWidth() const { return _width; }
		int getStyle() const { return _style; }
		QColor getColor() const { return _color; }
		
		/**
		 * setters
		 */
		void setWidth(double w) { _width = w; }
		void setStyle(int s) { _style = s; }
		void setColor(QString color) { _color.setNamedColor(color); }

		/**
		 * Helpfull functions
		 */

		/**
		 * Get informations from a markup tree (only param of a format).
		 */
		virtual void analyse(const QDomNode);

		virtual void generate(QTextStream&);

};

#endif /* __KSPREAD_LATEX_PEN_H__ */
