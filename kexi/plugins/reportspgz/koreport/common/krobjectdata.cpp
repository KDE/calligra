/*
 * Kexi Report Plugin
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact info@openmfg.com with any questions on this license.
 */
#include "krobjectdata.h"
#include <kdebug.h>
#include <QRectF>

KRObjectData::KRObjectData()
{
    Z = 0;
    m_name = new KoProperty::Property("Name", "", "Name", "Object Name");
    m_name->setAutoSync(0);
}

KRObjectData::~KRObjectData() { }

KRLineData * KRObjectData::toLine()
{
    return 0;
}
KRLabelData * KRObjectData::toLabel()
{
    return 0;
}
KRFieldData * KRObjectData::toField()
{
    return 0;
}
KRTextData * KRObjectData::toText()
{
    return 0;
}
KRBarcodeData * KRObjectData::toBarcode()
{
    return 0;
}
KRImageData * KRObjectData::toImage()
{
    return 0;
}
KRChartData * KRObjectData::toChart()
{
    return 0;
}
KRShapeData * KRObjectData::toShape()
{
    return 0;
}
KRCheckData * KRObjectData::toCheck()
{
    return 0;
}

bool KRObjectData::parseReportTextStyleData(const QDomElement & elemSource, KRTextStyleData & ts)
{
    if (elemSource.tagName() == "textstyle") {
	QDomNode  nodeCursor = elemSource.firstChild();
	ts.backgroundOpacity = 255;
	while (!nodeCursor.isNull()) {
	    
	    if (nodeCursor.isElement()) {
		QDomElement elemThis = nodeCursor.toElement();
		if (elemThis.tagName() == "bgcolor") {
		    ts.backgroundColor = elemThis.text();
		} else if (elemThis.tagName() == "fgcolor") {
		    ts.foregroundColor = elemThis.text();
		} else if (elemThis.tagName() == "bgopacity") {
		    ts.backgroundOpacity = elemThis.text().toInt();
		} else if (elemThis.tagName() == "font") {
		    parseReportFont(elemThis, ts.font);
		} else {
		    // we have encountered a tag that we don't understand.
		    // for now we will just inform a debugger about it
		    kDebug() << "Tag not Parsed at <textstle>:" << elemThis.tagName();
		}
	    }
	    
	    nodeCursor = nodeCursor.nextSibling();
	}
	return TRUE;
    }
    return FALSE;
}

bool KRObjectData::parseReportLineStyleData(const QDomElement & elemSource, KRLineStyleData & ls)
{
    if (elemSource.tagName() == "linestyle") {
	QDomNode  nodeCursor = elemSource.firstChild();
	
	while (!nodeCursor.isNull()) {
	    
	    if (nodeCursor.isElement()) {
		QDomElement elemThis = nodeCursor.toElement();
		if (elemThis.tagName() == "color") {
		    ls.lineColor = elemThis.text();
		} else if (elemThis.tagName() == "weight") {
		    ls.weight = elemThis.text().toInt();
		} else if (elemThis.tagName() == "style") {
		    QString l = elemThis.text();
		    if (l == "nopen") {
			ls.style = Qt::NoPen;
		    } else if (l == "solid") {
			ls.style = Qt::SolidLine;
		    } else if (l == "dash") {
			ls.style = Qt::DashLine;
		    } else if (l == "dot") {
			ls.style = Qt::DotLine;
		    } else if (l == "dashdot") {
			ls.style = Qt::DashDotLine;
		    } else if (l == "dashdotdot") {
			ls.style = Qt::DashDotDotLine;
		} else {
		    ls.style = Qt::SolidLine;
		}
	    } else {
		// we have encountered a tag that we don't understand.
		// for now we will just inform a debugger about it
		kDebug() << "Tag not Parsed at <linestle>:" << elemThis.tagName();
	    }
	}
	
	nodeCursor = nodeCursor.nextSibling();
    }
    return TRUE;
}
return FALSE;
}


bool KRObjectData::parseReportRect(const QDomElement & elemSource, QRectF & rectTarget)
{
    if (elemSource.tagName() == "report:rect") {
	rectTarget.setX(elemSource.attribute("report:x", "10").toFloat());
	rectTarget.setY(elemSource.attribute("report:y", "10").toFloat());
	rectTarget.setWidth(elemSource.attribute("report:width", "10").toFloat());
	rectTarget.setHeight(elemSource.attribute("report:height", "10").toFloat());
	return true;
    }
    
    return false;
}

bool KRObjectData::parseReportFont(const QDomElement & elemSource, QFont & fontTarget)
{
    if (elemSource.tagName() == "font") {
	fontTarget.fromString(elemSource.text());
	#if 0
	QDomNode  nodeCursor = elemSource.firstChild();
	
	while (!nodeCursor.isNull()) {
	    if (nodeCursor.isElement()) {
		QDomElement elemThis = nodeCursor.toElement();
		int intTemp;
		bool valid;
		
		if (elemThis.tagName() == "face")
		    fontTarget.setFamily(elemThis.text());
		else if (elemThis.tagName() == "size") {
		    intTemp = elemThis.text().toInt(&valid);
		    if (valid)
			fontTarget.setPointSize(intTemp);
		    else
			kDebug() << "Text not Parsed at <font>:" << elemThis.text();
    } else if (elemThis.tagName() == "weight") {
	if (elemThis.text() == "normal")
	    fontTarget.setWeight(50);
	else if (elemThis.text() == "bold")
	    fontTarget.setWeight(75);
	else {
	    // This is where we want to convert the string to an int value
	    // that should be between 1 and 100
	    intTemp = elemThis.text().toInt(&valid);
	    if (valid && intTemp >= 1 && intTemp <= 100)
		fontTarget.setWeight(intTemp);
	    else
		kDebug() << "Text not Parsed at <font>:" << elemThis.text();
    }
    } else {
	// we have encountered a tag that we don't understand.
	// for now we will just inform a debugger about it
	kDebug() << "Tag not Parsed at <font>:" << elemThis.tagName();
    }
    }
    nodeCursor = nodeCursor.nextSibling();
    }
    #endif
    return TRUE;
    }
    return FALSE;
}


