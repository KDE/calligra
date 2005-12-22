/* This file is part of the KDE project
   Copyright (C) 2003, The Kivio Developers

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef __DIAPATHPARSER_H__
#define __DIAPATHPARSER_H__

#include "svgpathparser.h"
#include <qvaluelist.h>
#include <qdom.h>

class DiaPointFinder : public SVGPathParser
{
	public:
		DiaPointFinder(QValueList<float> *xlist,  QValueList<float> *ylist);
		void svgMoveTo( double x1, double y1, bool abs = true );
		void svgLineTo( double x1, double y1, bool abs = true );
		void svgCurveToCubic( double x1, double y1, double x2, double y2, double x3, double y3, bool abs = true );
		void svgClosePath();

	private:
		QValueList<float> *m_xlist;
		QValueList<float> *m_ylist;
};

class DiaPathParser : public SVGPathParser
{
	public:
		DiaPathParser(QDomDocument *doc, QDomElement *shape, float xscale, float yscale, float lowestx, float lowesty);

		void svgMoveTo( double x1, double y1, bool abs = true );
		void svgLineTo( double x1, double y1, bool abs = true );
		void svgCurveToCubic( double x1, double y1, double x2, double y2, double x3, double y3, bool abs = true );
		void svgClosePath();

	private:
		float diaPointToKivio(float point, bool xpoint);

		QDomDocument *m_doc;
		QDomElement *m_shape;
		float lastX;
		float lastY;
		float m_xscale;
		float m_yscale;
		float m_lowestx;
		float m_lowesty;

};
#endif
