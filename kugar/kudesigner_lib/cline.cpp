/***************************************************************************
                          cline.cpp  -  description
                             -------------------
    begin                : 07.06.2002
    copyright            : (C) 2002 by Alexander Dymo
    email                : cloudtemple@mksat.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/
#ifndef PURE_QT
#include <klocale.h>
#else
#include "qlocale.h"
#endif

#include <qpainter.h>

#include "cline.h"
#include "canvband.h"

CanvasLine::CanvasLine(int x, int y, int width, int height, QCanvas * canvas):
    CanvasReportItem(x, y, width, height, canvas)
{
    props["X1"] = *(new PropPtr(new Property(IntegerValue, "X1", i18n("X1 coordinate corresponding to section"), QString("%1").arg(x))));

    props["Y1"] = *(new PropPtr(new Property(IntegerValue, "Y1", i18n("Y1 coordinate corresponding to section"), QString("%1").arg(y))));

    props["X2"] = *(new PropPtr(new Property(IntegerValue, "X2", i18n("X2 coordinate corresponding to section"), QString("%1").arg(x + width))));

    props["Y2"] = *(new PropPtr(new Property(IntegerValue, "Y2", i18n("Y2 coordinate corresponding to section"), QString("%1").arg(y + height))));

    props["Color"] = *(new PropPtr(new Property(Color, "Color", i18n("Color"), "0,0,0")));

    props["Width"] = *(new PropPtr(new Property(IntegerValue, "Width", i18n("Width"), "1")));

    props["Style"] = *(new PropPtr(new Property(LineStyle, "Style", i18n("Line style"), "1")));
}

QString CanvasLine::getXml()
{
    return "\t\t<Line" + CanvasReportItem::getXml() + " />\n";
}

QPen CanvasLine::getPenForShape()
{
    PenStyle style = SolidLine;
    switch (props["Style"]->value().toInt())
    {
        case 0: style = NoPen;
            break;
        case 1: style = SolidLine;
            break;
        case 2: style = DashLine;
            break;
        case 3: style = DotLine;
            break;
        case 4: style = DashDotLine;
            break;
        case 5: style = DashDotDotLine;
            break;
    }
    return QPen(QColor(props["Color"]->value().section(',', 0, 0).toInt(),
        props["Color"]->value().section(',', 1, 1).toInt(),
        props["Color"]->value().section(',', 2, 2).toInt()),
        props["Width"]->value().toInt(), style);
}

void CanvasLine::draw(QPainter &painter)
{
    //update dimensions
    if (!section()) return;
    setX(props["X1"]->value().toInt() + section()->x());
    setY(props["Y1"]->value().toInt() + section()->y());
    setSize(props["X2"]->value().toInt()-props["X1"]->value().toInt(),
        props["Y2"]->value().toInt()-props["Y1"]->value().toInt());

    //draw border and background
    painter.setPen(getPenForShape());
    painter.setBrush(QColor(0, 0, 0));
    painter.drawLine(props["X1"]->value().toInt() + (int)section()->x(),
        props["Y1"]->value().toInt() + (int)section()->y(),
        props["X2"]->value().toInt() + (int)section()->x(),
        props["Y2"]->value().toInt() + (int)section()->y());

    painter.setPen(QColor(0, 0, 0));
    painter.setBrush(QColor(0, 0, 0));
//    painter.drawRect(topLeftResizableRect());
//    painter.drawRect(topRightResizableRect());
//    painter.drawRect(bottomLeftResizableRect());
    painter.drawRect(bottomRightResizableRect());

//    drawHolders(painter);
}

void CanvasLine::setSection(CanvasBand *section)
{
    props["X1"]->setValue(QString("%1").arg((int)(x() - section->x())));
    props["Y1"]->setValue(QString("%1").arg((int)(y() - section->y())));
    parentSection = section;
}

void CanvasLine::updateGeomProps()
{
    props["X1"]->setValue(QString("%1").arg((int)(x() - section()->x())));
    props["Y1"]->setValue(QString("%1").arg((int)(y() - section()->y())));
    props["X2"]->setValue(QString("%1").arg((int)(x() - section()->x() + width())));
    props["Y2"]->setValue(QString("%1").arg((int)(y() - section()->y() + height())));

//    if (width() )
}
