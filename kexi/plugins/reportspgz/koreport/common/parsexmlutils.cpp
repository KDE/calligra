/*
 * OpenRPT report writer and rendering engine
 * Copyright (C) 2001-2007 by OpenMFG, LLC
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

#include "parsexmlutils.h"


#include "reportpageoptions.h"

#include <qdom.h>
#include <kdebug.h>
#include <QPainter>
#include <KoGlobal.h>
#include <KoUnit.h>

//TODO Graph
//bool ORGraphData::isGraph() { return TRUE; }
//ORGraphData * ORGraphData::toGraph() { return this; }

ORDetailGroupSectionData::ORDetailGroupSectionData()
{
    pagebreak = BreakNone;
    _subtotCheckPoints.clear();
    groupHeader = 0;
    groupFooter = 0;
}


//
// functions
//

bool parseReportTextStyleData(const QDomElement & elemSource, ORTextStyleData & ts)
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

bool parseReportLineStyleData(const QDomElement & elemSource, ORLineStyleData & ls)
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


bool parseReportRect(const QDomElement & elemSource, QRectF & rectTarget)
{
    if (elemSource.tagName() == "rect") {
        QDomNode  nodeCursor = elemSource.firstChild();

        while (!nodeCursor.isNull()) {
            if (nodeCursor.isElement()) {
                QDomElement elemThis = nodeCursor.toElement();
                int         intTemp;
                bool        valid;

                if (elemThis.tagName() == "x") {
                    intTemp = elemThis.text().toFloat(&valid);
                    if (valid)
                        rectTarget.setX(intTemp);
                    else
                        return FALSE;
                } else if (elemThis.tagName() == "y") {
                    intTemp = elemThis.text().toFloat(&valid);
                    if (valid)
                        rectTarget.setY(intTemp);
                    else
                        return FALSE;
                } else if (elemThis.tagName() == "width") {
                    intTemp = elemThis.text().toFloat(&valid);
                    if (valid)
                        rectTarget.setWidth(intTemp);
                    else
                        return FALSE;
                } else if (elemThis.tagName() == "height") {
                    intTemp = elemThis.text().toFloat(&valid);
                    if (valid)
                        rectTarget.setHeight(intTemp);
                    else
                        return FALSE;
                }
            }
            nodeCursor = nodeCursor.nextSibling();
        }
        return TRUE;
    }
    return FALSE;
}

bool parseReportFont(const QDomElement & elemSource, QFont & fontTarget)
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

