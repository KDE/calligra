
/*
** Header file for inclusion with kword_xml2latex.c
**
** Copyright (C) 2002 Robert JACOLIN
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

#ifndef __KSPREAD_LATEX_FORMAT_H__
#define __KSPREAD_LATEX_FORMAT_H__

#include <qtextstream.h>
#include <qstring.h>
#include <qcolor.h>
#include "xmlparser.h"

/***********************************************************************/
/* Class: Format                                                       */
/***********************************************************************/

/**
 * This class describe a cell, row or column format.
 */
class Format: public XmlParser
{
	long _align;
	long _alignY;
	QColor _bgColor;
	long _multirow;
	bool _verticalText;
	double _angle;
	QColor _brushColor;
	int _brushStyle;
	int _indent;
	bool _dontprinttext;

	/* pen */
	double _width;
	int _style;
	QColor _color;

	/* font */
	int _size;
	QString _family;
	int _weight;

	public:
		/**
		 * Constructors
		 *
		 * Creates a new instance of Format.
		 *
		 * @param Para the parent class of the format.
		 */
		Format();

		/* 
		 * Destructor
		 *
		 * Nothing to do
		 */
		virtual ~Format() {}

		/**
		 * getters
		 */
		long getMultirow() const { return _multirow; }
		long getAlign() const { return _align; }
		long getAlignY() const { return _alignY; }
		QColor getBgColor() const { return _bgColor; }
		bool getVerticalText() const { return _verticalText; }
		double getAngle() const { return _angle; }
		QColor getBrushColor() const { return _brushColor; }
		int getBrushStyle() const { return _brushStyle; }
		int getIndent() const { return _indent; }
		bool getDontPrintText() const { return _dontprinttext; }
	
		double getPenWidth() const { return _width; }
		int getPenStyle() const { return _style; }
		QColor getPenCOlor() const { return _color; }
		
		int getFontSize() const { return _size; }
		QString getFontFamily() const { return _family; }
		int getFontWeight() const { return _weight; }

		/**
		 * setters
		 */
		void setAlign(long a) { _align = a; }
		void setAlignY(long a) { _alignY = a; }
		void setBgColor(QColor b) { _bgColor = b; }
		void setMultirow(long mr)  { _multirow = mr; }
		void setVerticalText(bool vt) { _verticalText = vt; }
		void setAngle(double a) { _angle = a; }
		void setBrushColor(QString bc) { _brushColor.setNamedColor(bc); }
		void setBrushStyle(int bs) { _brushStyle = bs; }
		void setIndent(int indent) { _indent = indent; }
		void setDontPrintText(bool dpt) { _dontprinttext = dpt; }

		void setPenWidth(double w) { _width = w; }
		void setPenStyle(int s) { _style = s; }
		void setPenColor(QString color) { _color.setNamedColor(color); }

		void setFontSize(int s) { _size = s; }
		void setFontFamily(QString f) { _family = f; }
		void setFontWeight(int w) { _weight = w; }

		/**
		 * Helpfull functions
		 */

		/**
		 * Get informations from a markup tree (only param of a format).
		 */
		virtual void analyse(const QDomNode);
		virtual void analysePen(const QDomNode);
		virtual void analyseFont(const QDomNode);

		virtual void generate(QTextStream&);

};

#endif /* __KSPREAD_LATEX_FORMAT_H__ */
