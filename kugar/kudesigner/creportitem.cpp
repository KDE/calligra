/***************************************************************************
                          creportitem.cpp  -  description
                             -------------------
    begin                : 07.06.2002
    copyright            : (C) 2002 by Alexander Dymo
    email                : cloudtemple@mksat.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <klocale.h>
#include "creportitem.h"
#include <qrect.h>

QRect CanvasReportItem::topLeftResizableRect()
{
    return QRect((int)x(), (int)y(), 8, 8);
}

QRect CanvasReportItem::bottomLeftResizableRect()
{
    return QRect((int)x(), (int)(y()+width()-8), 8, 8);
}

QRect CanvasReportItem::topRightResizableRect()
{
    return QRect((int)(x()+width()-8), (int)y(), 8, 8);
}

QRect CanvasReportItem::bottomRightResizableRect()
{
    return QRect((int)(x()+width()-8), (int)(y()+height()-8), 8, 8);
}

void CanvasReportItem::updateGeomProps()
{
    if (!section()) return;
    props["X"].first = QString("%1").arg((int)(x()-section()->x()));
    props["Y"].first = QString("%1").arg((int)(y()-section()->y()));
    props["Width"].first = QString("%1").arg(width());
    props["Height"].first = QString("%1").arg(height());
}

CanvasBand *CanvasReportItem::section()
{
    return parentSection;
}

void CanvasReportItem::setSection(CanvasBand *section)
{
    props["X"].first = QString("%1").arg((int)(x() - section->x()));
    props["Y"].first = QString("%1").arg((int)(y() - section->y()));
    parentSection = section;
}

void CanvasReportItem::setSectionUndestructive(CanvasBand *section)
{
    parentSection = section;
}

QString CanvasReportItem::getXml()
{
    QString result = "";
    std::map<QString, std::pair<QString, QStringList> >::const_iterator it;
    int i = 1;
    for (it = props.begin(); it != props.end(); ++it)
    {
	if (!(i%3)) result += "\n\t\t  ";
	result += " " + it->first + "=" + "\"" + it->second.first + "\"";
	i++;
    }
    return result;
}

