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

#include "diapathparser.h"

DiaPointFinder::DiaPointFinder(QValueList<float> *xlist,  QValueList<float> *ylist) :
	SVGPathParser(), m_xlist(xlist), m_ylist(ylist)
{
	;
}

void DiaPointFinder::svgMoveTo( double x1, double y1, bool )
{
	m_xlist->append(x1);
	m_ylist->append(y1);
}
void DiaPointFinder::svgLineTo( double x1, double y1, bool )
{
	m_xlist->append(x1);
	m_ylist->append(y1);
}
void DiaPointFinder::svgCurveToCubic( double x1, double y1, double x2, double y2, double x3, double y3, bool )
{
	m_xlist->append(x1);
	m_ylist->append(y1);
	m_xlist->append(x2);
	m_ylist->append(y2);
	m_xlist->append(x3);
	m_ylist->append(y3);
}
void DiaPointFinder::svgClosePath()
{

}
DiaPathParser::DiaPathParser(QDomDocument *doc, QDomElement *shape, float xscale, float yscale, float lowestx, float lowesty) :
	SVGPathParser(), m_doc(doc), m_shape(shape),
	m_xscale(xscale), m_yscale(yscale),
	m_lowestx(lowestx), m_lowesty(lowesty)
{
	lastX = 0.0;
	lastY = 0.0;
}

void DiaPathParser::svgMoveTo( double x1, double y1, bool )
{
	lastX = x1;
	lastY = y1;
}
void DiaPathParser::svgLineTo( double x1, double y1, bool )
{
	// Line
	float currentX = x1;
	float currentY = y1;

	// Create the line
	QDomElement kivioPointElement = m_doc->createElement("KivioPoint");
	kivioPointElement.setAttribute("x", QString::number(diaPointToKivio(lastX,true) * m_xscale));
	kivioPointElement.setAttribute("y", QString::number(diaPointToKivio(lastY, false) * m_yscale));
	m_shape->appendChild(kivioPointElement);

	kivioPointElement = m_doc->createElement("KivioPoint");
	kivioPointElement.setAttribute("x", QString::number(diaPointToKivio(currentX,true) * m_xscale));
	kivioPointElement.setAttribute("y", QString::number(diaPointToKivio(currentY, false) * m_yscale));
	m_shape->appendChild(kivioPointElement);
	lastX = currentX;
	lastY = currentY;
}

void DiaPathParser::svgCurveToCubic( double x1, double y1, double x2, double y2, double x3, double y3, bool )
{
	// Spline
	float lastControlX = x1;
	float lastControlY = y1;
	float currentControlX = x2;
	float currentControlY = y2;
	float currentX = x3;
	float currentY = y3;

	// Create the bezier
	QDomElement kivioPointElement = m_doc->createElement("KivioPoint");
	kivioPointElement.setAttribute("x",
		QString::number(diaPointToKivio(lastX,true) * m_xscale));
	kivioPointElement.setAttribute("y",
		QString::number(diaPointToKivio(lastY, false) * m_yscale));
	kivioPointElement.setAttribute("type", "bezier");
	m_shape->appendChild(kivioPointElement);

	kivioPointElement = m_doc->createElement("KivioPoint");
	kivioPointElement.setAttribute("x",
		QString::number(diaPointToKivio(lastControlX,true) * m_xscale));
	kivioPointElement.setAttribute("y",
		QString::number(diaPointToKivio(lastControlY, false) * m_yscale));
	kivioPointElement.setAttribute("type", "bezier");
	m_shape->appendChild(kivioPointElement);

	kivioPointElement = m_doc->createElement("KivioPoint");
	kivioPointElement.setAttribute("x",
			QString::number(diaPointToKivio(currentControlX,true) * m_xscale));
	kivioPointElement.setAttribute("y",
		QString::number(diaPointToKivio(currentControlY, false) * m_yscale));
	kivioPointElement.setAttribute("type", "bezier");
	m_shape->appendChild(kivioPointElement);

	kivioPointElement = m_doc->createElement("KivioPoint");
	kivioPointElement.setAttribute("x",
		QString::number(diaPointToKivio(currentX,true) * m_xscale));
	kivioPointElement.setAttribute("y",
		QString::number(diaPointToKivio(currentY, false) * m_yscale));
	kivioPointElement.setAttribute("type", "bezier");
	m_shape->appendChild(kivioPointElement);
	lastX = currentX;
	lastY = currentY;
}
void DiaPathParser::svgClosePath()
{

}
float DiaPathParser::diaPointToKivio(float point, bool xpoint)
{
	if(xpoint)
		return point - m_lowestx;
	else
		return point - m_lowesty;
}
