#include "cline.h"

CanvasLine::CanvasLine(int x, int y, int width, int height, QCanvas * canvas):
	CanvasReportItem(x, y, width, height, canvas)
{
    pair<QString, QStringList> propValues;
      
    propValues.first = QString("%1").arg(x);
    propValues.second << qApp->translate("CanvasLabel", "X1 coordinate concerning to section"); 
    propValues.second << "int"; 
    props["X"] = propValues;
    propValues.second.clear();
    
    propValues.first = QString("%1").arg(y);
    propValues.second << qApp->translate("CanvasLabel", "Y1 coordinate"); 
    propValues.second << "int"; 
    props["Y"] = propValues;
    propValues.second.clear();

    propValues.first = QString("%1").arg(x + width);
    propValues.second << qApp->translate("CanvasLabel", "X2 coordinate");
    propValues.second << "int"; 
    props["X2"] = propValues;
    propValues.second.clear();
    
    propValues.first = QString("%1").arg(y + height);
    propValues.second << qApp->translate("CanvasLabel", "Y2 coordinate");
    propValues.second << "int"; 
    props["Y2"] = propValues;
    
    propValues.first = "255,255,255";
    propValues.second << qApp->translate("CanvasLabel", "Color");
    propValues.second << "color"; 
    props["BackgroundColor"] = propValues;
    propValues.second.clear();
    
    propValues.first = "1";
    propValues.second << qApp->translate("CanvasLine", "Line style");
    propValues.second << "int_from_list"; 
    propValues.second << qApp->translate("CanvasLine", "0 - No pen")
	    << qApp->translate("CanvasLabel", "1 - Solid line") 
	    << qApp->translate("CanvasLabel", "2 - Dash line") 
	    << qApp->translate("CanvasLabel", "3 - Dot line")
	    << qApp->translate("CanvasLabel", "4 - Dash dot line")
	    << qApp->translate("CanvasLabel", "5 - Dash dot dot line"); 
    props["Style"] = propValues;
    propValues.second.clear();    
}

QString CanvasLine::getXml()
{
    return "\t\t<Line" + CanvasReportItem::getXml() + " />\n";
}
