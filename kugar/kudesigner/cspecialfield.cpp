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
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <klocale.h>
#include "cspecialfield.h"

CanvasSpecialField::CanvasSpecialField(int x, int y, int width, int height, QCanvas * canvas):
	CanvasLabel(x, y, width, height, canvas)
{
    std::pair<QString, QStringList> propValues;

    propValues.first = "0";
    propValues.second << i18n("Field type to display");
    propValues.second << "int_from_list";
    propValues.second << i18n("0 - Date")
	    << i18n("1 - PageNumber");
    props["Type"] = propValues;
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
    props["DataType"] = propValues;
    propValues.second.clear();
}

void CanvasSpecialField::draw(QPainter &painter)
{
    props["Text"].first = "[" +
			  QString(props["Type"].first.toInt()?i18n("PageNo"):i18n("Date"))
			  + "]";
    CanvasLabel::draw(painter);
}

QString CanvasSpecialField::getXml()
{
    return "\t\t<Special" + CanvasReportItem::getXml() + " />\n";
}
