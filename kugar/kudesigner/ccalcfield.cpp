/***************************************************************************
                          ccalcfield.cpp  -  description
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
#include "ccalcfield.h"

CanvasCalculatedField::CanvasCalculatedField(int x, int y, int width, int height, QCanvas * canvas):
	CanvasField(x, y, width, height, canvas)
{
    std::pair<QString, QStringList> propValues;

    propValues.first = "1";
    propValues.second << i18n("CanvasCalculatedField", "");
    propValues.second << "int_from_list";
    propValues.second << i18n("CanvasCalculatedField", "0 - Count")
	    << i18n("CanvasCalculatedField", "1 - Sum")
	    << i18n("CanvasCalculatedField", "2 - Average")
	    << i18n("CanvasCalculatedField", "3 - Variance")
	    << i18n("CanvasCalculatedField", "4 - StandardDeviation");
    props["CalculationType"] = propValues;
    propValues.second.clear();
}

void CanvasCalculatedField::draw(QPainter &painter)
{
    CanvasField::draw(painter);
}

QString CanvasCalculatedField::getXml()
{
    return "\t\t<CalculatedField" + CanvasReportItem::getXml() + " />\n";
}
