/***************************************************************************
                          clabel.cpp  -  description
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
#include "clabel.h"
#include <qpainter.h>

CanvasLabel::CanvasLabel(int x, int y, int width, int height, QCanvas * canvas):
	CanvasReportItem(x, y, width, height, canvas)
{
    std::pair<QString, QStringList> propValues;

    propValues.first = i18n("Text");
    propValues.second << i18n("Text to display");
    propValues.second << "string";
    props["Text"] = propValues;
    propValues.second.clear();

    propValues.first = QString("%1").arg(x);
    propValues.second << i18n("X coordinate concerning to section");
    propValues.second << "int";
    props["X"] = propValues;
    propValues.second.clear();

    propValues.first = QString("%1").arg(y);
    propValues.second << i18n("Y coordinate concerning to section");
    propValues.second << "int";
    props["Y"] = propValues;
    propValues.second.clear();

    propValues.first = QString("%1").arg(width);
    propValues.second << i18n("Width");
    propValues.second << "int";
    props["Width"] = propValues;
    propValues.second.clear();

    propValues.first = QString("%1").arg(height);
    propValues.second << i18n("Height");
    propValues.second << "int";
    props["Height"] = propValues;

    propValues.first = "255,255,255";
    propValues.second << i18n("Background color");
    propValues.second << "color";
    props["BackgroundColor"] = propValues;
    propValues.second.clear();

    propValues.first = "0,0,0";
    propValues.second << i18n("Text color");
    propValues.second << "color";
    props["ForegroundColor"] = propValues;
    propValues.second.clear();

    propValues.first = "0,0,0";
    propValues.second << i18n("Border color");
    propValues.second << "color";
    props["BorderColor"] = propValues;
    propValues.second.clear();

    propValues.first = "1";
    propValues.second << i18n("Border width");
    propValues.second << "int";
    props["BorderWidth"] = propValues;
    propValues.second.clear();

    propValues.first = "1";
    propValues.second << i18n("Border style");
    propValues.second << "int_from_list";
    propValues.second << i18n("0 - No pen")
	    << i18n("1 - Solid line")
	    << i18n("2 - Dash line")
	    << i18n("3 - Dot line")
	    << i18n("4 - Dash dot line")
	    << i18n("5 - Dash dot dot line");
    props["BorderStyle"] = propValues;
    propValues.second.clear();

    propValues.first = "Times New Roman";
    propValues.second << i18n("Font family");
    propValues.second << "font";
    props["FontFamily"] = propValues;
    propValues.second.clear();

    propValues.first = "12";
    propValues.second << i18n("Font size");
    propValues.second << "int";
    props["FontSize"] = propValues;
    propValues.second.clear();

    propValues.first = "50";
    propValues.second << i18n("Font weight");
    propValues.second << "int_from_list";
    propValues.second << i18n("25 - Light")
	    << i18n("50 - Normal")
	    << i18n("63 - DemiBold")
	    << i18n("75 - Bold")
	    << i18n("87 - Black");
    props["FontWeight"] = propValues;
    propValues.second.clear();

    propValues.first = "0";
    propValues.second << i18n("Italic font");
    propValues.second << "int_from_list";
    propValues.second << i18n("0 - False")
	    << i18n("1 - True");
    props["FontItalic"] = propValues;
    propValues.second.clear();

    propValues.first = "0";
    propValues.second << i18n("Text horizontal alignment");
    propValues.second << "int_from_list";
    propValues.second << i18n("0 - Left")
	    << i18n("1 - Center")
	    << i18n("2 - Right");
    props["HAlignment"] = propValues;
    propValues.second.clear();

    propValues.first = "1";
    propValues.second << i18n("Text vertical alignment");
    propValues.second << "int_from_list";
    propValues.second << i18n("0 - Top")
	    << i18n("1 - Middle")
	    << i18n("2 - Bottom");
    props["VAlignment"] = propValues;
    propValues.second.clear();

    propValues.first = "0";
    propValues.second << i18n("Word wrap");
    propValues.second << "int_from_list";
    propValues.second << i18n("0 - False")
	    << i18n("1 - True");
    props["WordWrap"] = propValues;
    propValues.second.clear();
}

int CanvasLabel::getTextAlignment()
{
    int result = 0;
    switch (props["HAlignment"].first.toInt())
    {
	case 0: result = AlignLeft;
		break;
	case 1: result = AlignHCenter;
		break;
	case 2: result = AlignRight;
		break;
	default: result = AlignHCenter;
    }
    switch (props["VAlignment"].first.toInt())
    {
	case 0: result = result | AlignTop;
		break;
	case 1: result = result | AlignVCenter;
		break;
	case 2: result = result | AlignBottom;
		break;
	default: result = result | AlignVCenter;
    }
    return result;
}

int CanvasLabel::getTextWrap()
{
    switch (props["WordWrap"].first.toInt())
    {
        case 0: return SingleLine;
		break;
	case 1: return WordBreak;
		break;
	default: return SingleLine;
    }
}

QFont CanvasLabel::getFont()
{
    return QFont(props["FontFamily"].first,
		 props["FontSize"].first.toInt(),
		 props["FontWeight"].first.toInt(),
		 props["FontItalic"].first.toInt());
}

QPen CanvasLabel::getPenForText()
{
    return QPen(QColor(props["ForegroundColor"].first.section(',', 0, 0).toInt(),
		       props["ForegroundColor"].first.section(',', 1, 1).toInt(),
		       props["ForegroundColor"].first.section(',', 2, 2).toInt()));
}

QPen CanvasLabel::getPenForShape()
{
    PenStyle style = SolidLine;
    switch (props["BorderStyle"].first.toInt())
    {
        case 0: style = NoPen;
		break;
	case 1: style = SolidLine;
		break;
	case 2: style = DashLine;
		break;
	case 3: style = DotLine;
		break;
	case 4: style = DashDotLine;
		break;
	case 5: style = DashDotDotLine;
		break;
    }
    return QPen(QColor(props["BorderColor"].first.section(',', 0, 0).toInt(),
		       props["BorderColor"].first.section(',', 1, 1).toInt(),
		       props["BorderColor"].first.section(',', 2, 2).toInt()),
		props["BorderWidth"].first.toInt(), style);
}

QBrush CanvasLabel::getBrush()
{
    return QBrush(QColor(props["BackgroundColor"].first.section(',', 0, 0).toInt(),
			 props["BackgroundColor"].first.section(',', 1, 1).toInt(),
			 props["BackgroundColor"].first.section(',', 2, 2).toInt()));
}

void CanvasLabel::draw(QPainter &painter)
{
    //update dimensions
    if (!section()) return;
    setX(props["X"].first.toInt() + section()->x());
    setY(props["Y"].first.toInt() + section()->y());
    setSize(props["Width"].first.toInt(), props["Height"].first.toInt());
    //draw border and background
    painter.setPen(getPenForShape());
    painter.setBrush(getBrush());
    painter.drawRect(rect());
    //draw text inside
    painter.setFont(getFont());
    painter.setPen(getPenForText());
    painter.drawText((int)x(), (int)y(), width(), height(),
		     getTextAlignment() | getTextWrap(),
		     props["Text"].first);
    //draw resizable region
    painter.setPen(QColor(0, 0, 0));
    painter.setBrush(QColor(0, 0, 0));
//    painter.drawRect(topLeftResizableRect());
//    painter.drawRect(topRightResizableRect());
//    painter.drawRect(bottomLeftResizableRect());
    painter.drawRect(bottomRightResizableRect());
//    CanvasReportItem::draw(painter);
}

QString CanvasLabel::getXml()
{
    return "\t\t<Label" + CanvasReportItem::getXml() + " />\n";
}
