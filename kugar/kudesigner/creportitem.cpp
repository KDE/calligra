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
#include <kglobalsettings.h>

#include "creportitem.h"

#include <qrect.h>
#include <qpainter.h>

QRect CanvasReportItem::topLeftResizableRect()
{
    return QRect((int)x(), (int)y(), HolderSize, HolderSize);
}

QRect CanvasReportItem::bottomLeftResizableRect()
{
    return QRect((int)x(), (int)(y()+height()-HolderSize), HolderSize, HolderSize);
}

QRect CanvasReportItem::topRightResizableRect()
{
    return QRect((int)(x()+width()-HolderSize), (int)y(), HolderSize, HolderSize);
}

QRect CanvasReportItem::bottomRightResizableRect()
{
    return QRect((int)(x()+width()-HolderSize), (int)(y()+height()-HolderSize), HolderSize, HolderSize);
}

QRect CanvasReportItem::topMiddleResizableRect()
{
    return QRect((int)(x()+width()/2-HolderSize/2.), (int)y(), HolderSize, HolderSize);
}

QRect CanvasReportItem::bottomMiddleResizableRect()
{
    return QRect((int)(x()+width()/2-HolderSize/2.), (int)(y()+height()-HolderSize), HolderSize, HolderSize);
}

QRect CanvasReportItem::leftMiddleResizableRect()
{
    return QRect((int)x(), (int)(y()+height()/2-HolderSize/2.), HolderSize, HolderSize);
}

QRect CanvasReportItem::rightMiddleResizableRect()
{
    return QRect((int)(x()+width()-HolderSize), (int)(y()+height()/2-HolderSize/2.), HolderSize, HolderSize);
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
        if (it->first.isNull()) continue;
	if (!(i%3)) result += "\n\t\t  ";
	result += " " + it->first + "=" + "\"" + it->second.first + "\"";
	i++;
    }
    return result;
}

bool CanvasReportItem::isInHolder(const QPoint p)
{
    if (topLeftResizableRect().contains(p) ||
        bottomLeftResizableRect().contains(p) ||
        topRightResizableRect().contains(p) ||
        bottomRightResizableRect().contains(p) ||
        topMiddleResizableRect().contains(p) ||
        bottomMiddleResizableRect().contains(p) ||
        leftMiddleResizableRect().contains(p) ||
        rightMiddleResizableRect().contains(p))
        return true;
    return false;
}

void CanvasReportItem::drawHolders(QPainter &painter)
{
    painter.setPen(QColor(0, 0, 0));
    painter.setBrush(KGlobalSettings::highlightColor());
    painter.drawRect(topLeftResizableRect());
    painter.drawRect(topRightResizableRect());
    painter.drawRect(bottomLeftResizableRect());
    painter.drawRect(bottomRightResizableRect());
    painter.drawRect(topMiddleResizableRect());
    painter.drawRect(bottomMiddleResizableRect());
    painter.drawRect(leftMiddleResizableRect());
    painter.drawRect(rightMiddleResizableRect());
}
