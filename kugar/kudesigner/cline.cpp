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
#include "cline.h"

CanvasLine::CanvasLine(int x, int y, int width, int height, QCanvas * canvas):
	CanvasReportItem(x, y, width, height, canvas)
{
    pair<QString, QStringList> propValues;
      
    propValues.first = QString("%1").arg(x);
    propValues.second << i18n("CanvasLabel", "X1 coordinate concerning to section"); 
    propValues.second << "int"; 
    props["X"] = propValues;
    propValues.second.clear();
    
    propValues.first = QString("%1").arg(y);
    propValues.second << i18n("CanvasLabel", "Y1 coordinate"); 
    propValues.second << "int"; 
    props["Y"] = propValues;
    propValues.second.clear();

    propValues.first = QString("%1").arg(x + width);
    propValues.second << i18n("CanvasLabel", "X2 coordinate");
    propValues.second << "int"; 
    props["X2"] = propValues;
    propValues.second.clear();
    
    propValues.first = QString("%1").arg(y + height);
    propValues.second << i18n("CanvasLabel", "Y2 coordinate");
    propValues.second << "int"; 
    props["Y2"] = propValues;
    
    propValues.first = "255,255,255";
    propValues.second << i18n("CanvasLabel", "Color");
    propValues.second << "color"; 
    props["BackgroundColor"] = propValues;
    propValues.second.clear();
    
    propValues.first = "1";
    propValues.second << i18n("CanvasLine", "Line style");
    propValues.second << "int_from_list"; 
    propValues.second << i18n("CanvasLine", "0 - No pen")
	    << i18n("CanvasLabel", "1 - Solid line") 
	    << i18n("CanvasLabel", "2 - Dash line") 
	    << i18n("CanvasLabel", "3 - Dot line")
	    << i18n("CanvasLabel", "4 - Dash dot line")
	    << i18n("CanvasLabel", "5 - Dash dot dot line"); 
    props["Style"] = propValues;
    propValues.second.clear();    
}

QString CanvasLine::getXml()
{
    return "\t\t<Line" + CanvasReportItem::getXml() + " />\n";
}
