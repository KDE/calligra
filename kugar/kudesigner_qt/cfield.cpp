#include "cfield.h"

CanvasField::CanvasField(int x, int y, int width, int height, QCanvas * canvas):
	CanvasLabel(x, y, width, height, canvas)
{
    pair<QString, QStringList> propValues;
    
    propValues.first = qApp->translate("CanvasField", "Field");
    propValues.second << qApp->translate("CanvasField", "Field to display");
    propValues.second << "string"; 
    props["Field"] = propValues;
    propValues.second.clear();
    
    propValues.first = "0";
    propValues.second << qApp->translate("CanvasField", "Data type");
    propValues.second << "int_from_list";
    propValues.second << qApp->translate("CanvasField", "0 - String")
	    << qApp->translate("CanvasField", "1 - Integer")
	    << qApp->translate("CanvasField", "2 - Float")
	    << qApp->translate("CanvasField", "3 - Date")
	    << qApp->translate("CanvasField", "4 - Currency");
    props["DataType"] = propValues;
    propValues.second.clear();
    
    propValues.first = "11";
    propValues.second << qApp->translate("CanvasField", "Date format");
    propValues.second << "int_from_list";
    propValues.second << qApp->translate("CanvasField", "0 - m/d/y")
	    << qApp->translate("CanvasField", "1 - m-d-y")
	    << qApp->translate("CanvasField", "2 - mm/dd/y")
	    << qApp->translate("CanvasField", "3 - mm-dd-y")
	    << qApp->translate("CanvasField", "4 - m/d/yyyy")
	    << qApp->translate("CanvasField", "5 - m-d-yyyy")
	    << qApp->translate("CanvasField", "6 - mm/dd/yyyy")
	    << qApp->translate("CanvasField", "7 - mm-dd-yyyy")
	    << qApp->translate("CanvasField", "8 - yyyy/m/d")
	    << qApp->translate("CanvasField", "9 - yyyy-m-d")
	    << qApp->translate("CanvasField", "10 - dd.mm.yy")
	    << qApp->translate("CanvasField", "11 - dd.mm.yyyy");
    props["DateFormat"] = propValues;
    propValues.second.clear();
    
    propValues.first = "2";
    propValues.second << qApp->translate("CanvasField", "Number of digits after comma");
    propValues.second << "int"; 
    props["Precision"] = propValues;
    propValues.second.clear();
    
    propValues.first = "32";
    propValues.second << qApp->translate("CanvasField", "Currency symbol");
    propValues.second << "int_from_list";
    propValues.second << qApp->translate("CanvasField", "20 - ' '")
	    << qApp->translate("CanvasField", "36 - '$'");
    props["Currency"] = propValues;
    propValues.second.clear();
    
    propValues.first = "0,0,0";
    propValues.second << qApp->translate("CanvasField", "Negative value color");
    propValues.second << "color"; 
    props["NegValueColor"] = propValues;
    propValues.second.clear();
    
    propValues.first = "44";
    propValues.second << qApp->translate("CanvasField", "Comma separator");
    propValues.second << "int_from_list";
    propValues.second << qApp->translate("CanvasField", "44 - ','")
	    << qApp->translate("CanvasField", "46 - '.'");
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
