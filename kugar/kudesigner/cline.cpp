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
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <klocale.h>
#include <qpainter.h>
#include "cline.h"

CanvasLine::CanvasLine(int x, int y, int width, int height, QCanvas * canvas):
	CanvasReportItem(x, y, width, height, canvas)
{
    std::pair<QString, QStringList> propValues;

    propValues.first = QString("%1").arg(x);
    propValues.second << i18n("X1 coordinate corresponding to section");
    propValues.second << "int";
    props["X1"] = propValues;
    propValues.second.clear();

    propValues.first = QString("%1").arg(y);
    propValues.second << i18n("Y1 coordinate corresponding to section");
    propValues.second << "int";
    props["Y1"] = propValues;
    propValues.second.clear();

    propValues.first = QString("%1").arg(x + width);
    propValues.second << i18n("X2 coordinate corresponding to section");
    propValues.second << "int";
    props["X2"] = propValues;
    propValues.second.clear();

    propValues.first = QString("%1").arg(y + height);
    propValues.second << i18n("Y2 coordinate corresponding to section");
    propValues.second << "int";
    props["Y2"] = propValues;
    propValues.second.clear();

    propValues.first = "0,0,0";
    propValues.second << i18n("Color");
    propValues.second << "color";
    props["Color"] = propValues;
    propValues.second.clear();

    propValues.first = "1";
    propValues.second << i18n("Width");
    propValues.second << "int";
    props["Width"] = propValues;
    propValues.second.clear();		    
		    
    propValues.first = "1";
    propValues.second << i18n("Line style");
    propValues.second << "int_from_list";
    propValues.second << i18n("0 - No pen")
	    << i18n("1 - Solid line")
	    << i18n("2 - Dash line")
	    << i18n("3 - Dot line")
	    << i18n("4 - Dash dot line")
	    << i18n("5 - Dash dot dot line");
    props["Style"] = propValues;
    propValues.second.clear();
}

QString CanvasLine::getXml()
{
    return "\t\t<Line" + CanvasReportItem::getXml() + " />\n";
}

QPen CanvasLine::getPenForShape()
{
    PenStyle style = SolidLine;
    switch (props["Style"].first.toInt())
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
    return QPen(QColor(props["Color"].first.section(',', 0, 0).toInt(),
	props["Color"].first.section(',', 1, 1).toInt(),
	props["Color"].first.section(',', 2, 2).toInt()),
	props["Width"].first.toInt(), style);
}

void CanvasLine::draw(QPainter &painter)
{
    //update dimensions
    if (!section()) return;
    setX(props["X1"].first.toInt() + section()->x());
    setY(props["Y1"].first.toInt() + section()->y());
    setSize(props["X2"].first.toInt()-props["X1"].first.toInt(),
    	props["Y2"].first.toInt()-props["Y1"].first.toInt());
    //draw border and background
    painter.setPen(getPenForShape());
    painter.setBrush(QColor(0, 0, 0));
    painter.drawLine(props["X1"].first.toInt() + (int)section()->x(),
    	props["Y1"].first.toInt() + (int)section()->y(),
     	props["X2"].first.toInt() + (int)section()->x(),
      	props["Y2"].first.toInt() + (int)section()->y());

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
    props["X1"].first = QString("%1").arg((int)(x() - section->x()));
    props["Y1"].first = QString("%1").arg((int)(y() - section->y()));
    parentSection = section;
}

void CanvasLine::updateGeomProps()
{
    props["X1"].first = QString("%1").arg((int)(x() - section()->x()));
    props["Y1"].first = QString("%1").arg((int)(y() - section()->y()));
    props["X2"].first = QString("%1").arg((int)(x() - section()->x() + width()));
    props["Y2"].first = QString("%1").arg((int)(y() - section()->y() + height()));

//    if (width() )
}
