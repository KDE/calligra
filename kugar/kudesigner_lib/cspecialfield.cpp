/***************************************************************************
                          cspecialfield.cpp  -  description
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

#include "cspecialfield.h"
#include "property.h"

CanvasSpecialField::CanvasSpecialField(int x, int y, int width, int height, QCanvas * canvas):
    CanvasLabel(x, y, width, height, canvas)
{
    std::map<QString, QString> m;

    m["Date"] = "0";
    m["PageNumber"] = "1";
    props["Type"] = *(new PropPtr(new Property("Type", m, i18n("Field type to display"), "0")));
    m.clear();

    m["m/d/y"] = "0";
    m["m-d-y"] = "1";
    m["mm/dd/y"] = "2";
    m["mm-dd-y"] = "3";
    m["m/d/yyyy"] = "4";
    m["m-d-yyyy"] = "5";
    m["mm/dd/yyyy"] = "6";
    m["mm-dd-yyyy"] = "7";
    m["yyyy/m/d"] = "8";
    m["yyyy-m-d"] = "9";
    m["dd.mm.yy"] = "10";
    m["dd.mm.yyyy"] = "11";
    //TODO: make date format not hard-coded, use locale settings
    props["DateFormat"] = *(new PropPtr(new Property("DateFormat", m, i18n("Date format"), "11")));
}

void CanvasSpecialField::draw(QPainter &painter)
{
    props["Text"]->setValue("[" +
        QString(props["Type"]->value().toInt()?i18n("PageNo"):i18n("Date"))
        + "]");
    CanvasLabel::draw(painter);
}

QString CanvasSpecialField::getXml()
{
    return "\t\t<Special" + CanvasReportItem::getXml() + " />\n";
}
