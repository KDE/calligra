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
    pair<QString, QStringList> propValues;
    
    propValues.first = i18n("CanvasField", "Field");
    propValues.second << i18n("CanvasField", "Field to display");
    propValues.second << "string"; 
    props["Field"] = propValues;
    propValues.second.clear();
    
    propValues.first = "0";
    propValues.second << i18n("CanvasField", "Data type");
    propValues.second << "int_from_list";
    propValues.second << i18n("CanvasField", "0 - String")
	    << i18n("CanvasField", "1 - Integer")
	    << i18n("CanvasField", "2 - Float")
	    << i18n("CanvasField", "3 - Date")
	    << i18n("CanvasField", "4 - Currency");
    props["DataType"] = propValues;
    propValues.second.clear();
    
    propValues.first = "11";
    propValues.second << i18n("CanvasField", "Date format");
    propValues.second << "int_from_list";
    propValues.second << i18n("CanvasField", "0 - m/d/y")
	    << i18n("CanvasField", "1 - m-d-y")
	    << i18n("CanvasField", "2 - mm/dd/y")
	    << i18n("CanvasField", "3 - mm-dd-y")
	    << i18n("CanvasField", "4 - m/d/yyyy")
	    << i18n("CanvasField", "5 - m-d-yyyy")
	    << i18n("CanvasField", "6 - mm/dd/yyyy")
	    << i18n("CanvasField", "7 - mm-dd-yyyy")
	    << i18n("CanvasField", "8 - yyyy/m/d")
	    << i18n("CanvasField", "9 - yyyy-m-d")
	    << i18n("CanvasField", "10 - dd.mm.yy")
	    << i18n("CanvasField", "11 - dd.mm.yyyy");
    props["DateFormat"] = propValues;
    propValues.second.clear();
    
    propValues.first = "2";
    propValues.second << i18n("CanvasField", "Number of digits after comma");
    propValues.second << "int"; 
    props["Precision"] = propValues;
    propValues.second.clear();
    
    propValues.first = "32";
    propValues.second << i18n("CanvasField", "Currency symbol");
    propValues.second << "int_from_list";
    propValues.second << i18n("CanvasField", "20 - ' '")
	    << i18n("CanvasField", "36 - '$'");
    props["Currency"] = propValues;
    propValues.second.clear();
    
    propValues.first = "0,0,0";
    propValues.second << i18n("CanvasField", "Negative value color");
    propValues.second << "color"; 
    props["NegValueColor"] = propValues;
    propValues.second.clear();
    
    propValues.first = "44";
    propValues.second << i18n("CanvasField", "Comma separator");
    propValues.second << "int_from_list";
    propValues.second << i18n("CanvasField", "44 - ','")
	    << i18n("CanvasField", "46 - '.'");
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
