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

#ifndef __KILLU_ELEMENT_H__
#define __KILLU_ELEMENT_H__

#include <qtextstream.h>
#include <qcolor.h>
#include <qstring.h>
#include <qwmatrix.h>

#include "xmlparser.h"

enum EStyle
{
	FS_NONE,
	FS_SOLID,
	FS_PATTERN,
	FS_UNKNOWN,
	FS_GRADIENT
};

/***********************************************************************/
/* Class: Element                                                      */
/***********************************************************************/

/**
 * This class hold a whole document with its rectangles, layers, ...
 * It can generate a latex file (with pstricks extension).
 */
class Element: public XmlParser
{
	EStyle  _fillStyle;
	EStyle  _strokeStyle;

	/* GOBJECT */
	double  _lineWidth;
	int     _fillpattern;
	QString _strokeColor,
	        _fillColor,
	        _gradientColor1,
                _gradientColor2;
	QString _fillcolorname;
	QString _strokecolorname;
	QString _gradientcolorname1;
	QString _gradientcolorname2;
	QWMatrix  _matrix;

	public:
		/**
		 * Constructor
		 *
		 * Creates a new instance of Document.
		 */
		Element();

		/**
		 * Destructor
		 *
		 * Remove the list of headers, footers and the body.
		 */
		virtual ~Element();

		/**
		 * Accessors
		 */
		QString  getBaseContentAttr() const;

		double   getLineWidth     () const { return _lineWidth;   }
		EStyle   getStrokeStyle   () const { return _strokeStyle; }
		EStyle   getFillStyle     () const { return _fillStyle;   }
		QString  getFillColor     () const { return _fillColor;   }
		QString  getStrokeColor   () const { return _strokeColor; }
		QString  getGradientColor1() const { return _gradientColor1; }
		QString  getGradientColor2() const { return _gradientColor2; }
		QWMatrix getMatrix        () const { return _matrix;      }
		int      getFillPattern   () const { return _fillpattern; }

		/**
		 * Modifiors
		 */
		void setLineWidth     (double lw) { _lineWidth      = lw; }
		void setStrokeStyle   (int ss)    { _strokeStyle    = (EStyle) ss; }
		void setFillStyle     (int fs)    { _fillStyle      = (EStyle) fs; }
		void setFillColor     (QString c) { _fillColor      = c;  }
		void setStrokeColor   (QString c) { _strokeColor    = c;  }
		void setGradientColor1(QString c) { _gradientColor1 = c;  }
		void setGradientColor2(QString c) { _gradientColor2 = c;  }
		void setFillPattern   (int fp)    { _fillpattern    = fp; }

		virtual void  analyse(const QDomNode);
		void          analyseMatrix(const QDomNode);
		void          analyseGObject(const QDomNode);

		virtual void  generatePSTRICKS(QTextStream&)= 0;
		void generateFillPattern(QString&) const;

		void concat(QString&, const QString) const;
		void concat(QString&, const float);
		void generateList(QTextStream&, const QString, const QString, const QString);
	private:
};

#endif /* __KILLU_ELEMENT_H__ */
