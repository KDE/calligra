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
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/
#include <klocale.h>
#include "ccalcfield.h"
#include "property.h"

CanvasCalculatedField::CanvasCalculatedField(int x, int y, int width, int height, QCanvas * canvas):
    CanvasField(x, y, width, height, canvas,false)
{
    std::map<QString, QString> m;

    m["Count"] = "0";
    m["Sum"] = "1";
    m["Average"] = "2";
    m["Variance"] = "3";
    m["StandardDeviation"] = "4";
    props["CalculationType"] = *(new PropPtr(new Property("CalculationType", m, i18n("Calculation type"), "1")));
    registerAs(KuDesignerRttiCanvasCalculated);
}

void CanvasCalculatedField::draw(QPainter &painter)
{
    CanvasField::draw(painter);
}

QString CanvasCalculatedField::getXml()
{
    return "\t\t<CalculatedField" + CanvasReportItem::getXml() + " />\n";
}
