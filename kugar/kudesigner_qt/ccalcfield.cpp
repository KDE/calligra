#include "ccalcfield.h"

CanvasCalculatedField::CanvasCalculatedField(int x, int y, int width, int height, QCanvas * canvas):
	CanvasField(x, y, width, height, canvas)
{
    pair<QString, QStringList> propValues;
      
    propValues.first = "1";
    propValues.second << qApp->translate("CanvasCalculatedField", ""); 
    propValues.second << "int_from_list";
    propValues.second << qApp->translate("CanvasCalculatedField", "0 - Count")
	    << qApp->translate("CanvasCalculatedField", "1 - Sum")
	    << qApp->translate("CanvasCalculatedField", "2 - Average")
	    << qApp->translate("CanvasCalculatedField", "3 - Variance")
	    << qApp->translate("CanvasCalculatedField", "4 - StandardDeviation");
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
