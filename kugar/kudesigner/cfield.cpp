/***************************************************************************
                          cfield.cpp  -  description
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
#include "cfield.h"

CanvasField::CanvasField(int x, int y, int width, int height, QCanvas * canvas):
	CanvasLabel(x, y, width, height, canvas)
{
    std::pair<QString, QStringList> propValues;

    propValues.first = i18n("Field");
    propValues.second << i18n("Field to display");
    propValues.second << "string";
    props["Field"] = propValues;
    propValues.second.clear();

    propValues.first = "0";
    propValues.second << i18n("Data type");
    propValues.second << "int_from_list";
    propValues.second << i18n("0 - String")
	    << i18n("1 - Integer")
	    << i18n("2 - Float")
	    << i18n("3 - Date")
	    << i18n("4 - Currency");
    props["DataType"] = propValues;
    propValues.second.clear();

    propValues.first = "11";
    propValues.second << i18n("Date format");
    propValues.second << "int_from_list";
    propValues.second << i18n("0 - m/d/y")
	    << i18n("1 - m-d-y")
	    << i18n("2 - mm/dd/y")
	    << i18n("3 - mm-dd-y")
	    << i18n("4 - m/d/yyyy")
	    << i18n("5 - m-d-yyyy")
	    << i18n("6 - mm/dd/yyyy")
	    << i18n("7 - mm-dd-yyyy")
	    << i18n("8 - yyyy/m/d")
	    << i18n("9 - yyyy-m-d")
	    << i18n("10 - dd.mm.yy")
	    << i18n("11 - dd.mm.yyyy");
    props["DateFormat"] = propValues;
    propValues.second.clear();

    propValues.first = "2";
    propValues.second << i18n("Number of digits after comma");
    propValues.second << "int";
    props["Precision"] = propValues;
    propValues.second.clear();

    propValues.first = "32";
    propValues.second << i18n("Currency symbol");
    propValues.second << "int_from_list";
    propValues.second << i18n("20 - ' '")
	    << i18n("36 - '$'");
    props["Currency"] = propValues;
    propValues.second.clear();

    propValues.first = "0,0,0";
    propValues.second << i18n("Negative value color");
    propValues.second << "color";
    props["NegValueColor"] = propValues;
    propValues.second.clear();

    propValues.first = "44";
    propValues.second << i18n("Comma separator");
    propValues.second << "int_from_list";
    propValues.second << i18n("44 - ','")
	    << i18n("46 - '.'");
    props["CommaSeparator"] = propValues;
    propValues.second.clear();
}

void CanvasField::draw(QPainter &painter)
{
    props["Text"].first = "[" + props["Field"].first + "]";
    CanvasLabel::draw(painter);
}

QString CanvasField::getXml()
{
    return "\t\t<Field" + CanvasReportItem::getXml() + " />\n";
}
