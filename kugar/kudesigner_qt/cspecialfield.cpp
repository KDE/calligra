#include "cspecialfield.h"

CanvasSpecialField::CanvasSpecialField(int x, int y, int width, int height, QCanvas * canvas):
	CanvasLabel(x, y, width, height, canvas)
{
    pair<QString, QStringList> propValues;
    
    propValues.first = qApp->translate("CanvasSpecialField", "0");
    propValues.second << qApp->translate("CanvasSpecialField", "Field type to display");
    propValues.second << "int_from_list"; 
    propValues.second << qApp->translate("CanvasSpecialField", "0 - Date") 
	    << qApp->translate("CanvasSpecialField", "1 - PageNumber");
    props["Type"] = propValues;
    propValues.second.clear();
    
    propValues.first = "11";
    propValues.second << qApp->translate("CanvasSpecialField", "Date format");
    propValues.second << "int_from_list";
    propValues.second << qApp->translate("CanvasSpecialField", "0 - m/d/y")
	    << qApp->translate("CanvasSpecialField", "1 - m-d-y")
	    << qApp->translate("CanvasSpecialField", "2 - mm/dd/y")
	    << qApp->translate("CanvasSpecialField", "3 - mm-dd-y")
	    << qApp->translate("CanvasSpecialField", "4 - m/d/yyyy")
	    << qApp->translate("CanvasSpecialField", "5 - m-d-yyyy")
	    << qApp->translate("CanvasSpecialField", "6 - mm/dd/yyyy")
	    << qApp->translate("CanvasSpecialField", "7 - mm-dd-yyyy")
	    << qApp->translate("CanvasSpecialField", "8 - yyyy/m/d")
	    << qApp->translate("CanvasSpecialField", "9 - yyyy-m-d")
	    << qApp->translate("CanvasSpecialField", "10 - dd.mm.yy")
	    << qApp->translate("CanvasSpecialField", "11 - dd.mm.yyyy");
    props["DataType"] = propValues;
    propValues.second.clear();
}

void CanvasSpecialField::draw(QPainter &painter)
{
    props["Text"].first = "[" +  
			  QString(props["Type"].first.toInt()?"PageNo":"Date")
			  + "]";
    CanvasLabel::draw(painter);
}

QString CanvasSpecialField::getXml()
{
    return "\t\t<Special" + CanvasReportItem::getXml() + " />\n";
}
