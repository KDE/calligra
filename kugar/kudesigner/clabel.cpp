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
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/
#include <klocale.h>
#include "clabel.h"
#include "property.h"
#include <qpainter.h>

CanvasLabel::CanvasLabel(int x, int y, int width, int height, QCanvas * canvas):
    CanvasReportItem(x, y, width, height, canvas)
{
    std::map<QString, QString> m;

    props["Text"] = *(new PropPtr(new Property(StringValue, "Text", i18n("Text to display"), i18n("Text"))));

    props["X"] = *(new PropPtr(new Property(IntegerValue, "X", i18n("X coordinate corresponding to section"), QString("%1").arg(x))));

    props["Y"] = *(new PropPtr(new Property(IntegerValue, "Y", i18n("Y coordinate corresponding to section"), QString("%1").arg(y))));

    props["Width"] = *(new PropPtr(new Property(IntegerValue, "Width", i18n("Width"), QString("%1").arg(width))));

    props["Height"] = *(new PropPtr(new Property(IntegerValue, "Height", i18n("Height"), QString("%1").arg(height))));

    props["BackgroundColor"] = *(new PropPtr(new Property(Color, "BackgroundColor", i18n("Background color"), "255,255,255")));

    props["ForegroundColor"] = *(new PropPtr(new Property(Color, "ForegroundColor", i18n("Text color"), "0,0,0")));

    props["BorderColor"] = *(new PropPtr(new Property(Color, "BorderColor", i18n("Border color"), "0,0,0")));

    props["BorderWidth"] = *(new PropPtr(new Property(IntegerValue, "BorderWidth", i18n("Border width"), "1")));

    props["BorderStyle"] = *(new PropPtr(new Property(LineStyle, "BorderStyle", i18n("Border style"), "1")));

    props["FontFamily"] = *(new PropPtr(new Property(FontName, "FontFamily", i18n("Font family"), "Times New Roman")));

    props["FontSize"] = *(new PropPtr(new Property(IntegerValue, "FontSize", i18n("Font size"), "12")));

    m["Light"] = "25";
    m["Normal"] = "50";
    m["DemiBold"] = "63";
    m["Bold"] = "75";
    m["Black"] = "87";
    props["FontWeight"] = *(new PropPtr(new Property("FontWeight", m, i18n("Font weight"), "50")));
    m.clear();

    m["Regular"] = "0";
    m["Italic"] = "1";
    props["FontItalic"] = *(new PropPtr(new Property("FontItalic", m, i18n("Italic font"), "0")));
    m.clear();

    m["Left"] = "0";
    m["Center"] = "1";
    m["Right"] = "2";
    props["HAlignment"] = *(new PropPtr(new Property("HAlignment", m, i18n("Text horizontal alignment"),
        QString("").isRightToLeft()?"2":"0")));
    m.clear();

    m["Top"] = "0";
    m["Middle"] = "1";
    m["Bottom"] = "2";
    props["VAlignment"] = *(new PropPtr(new Property("VAlignment", m, i18n("Text vertical alignment"), "1")));
    m.clear();

    m["False"] = "0";
    m["True"] = "1";
    props["WordWrap"] = *(new PropPtr(new Property("WordWrap", m, i18n("Word wrap"), "0")));
}

int CanvasLabel::getTextAlignment()
{
    int result = 0;
    switch (props["HAlignment"]->value().toInt())
    {
        case 0: result = AlignLeft;
            break;
        case 1: result = AlignHCenter;
            break;
        case 2: result = AlignRight;
            break;
        default: result = AlignHCenter;
    }
    switch (props["VAlignment"]->value().toInt())
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
    switch (props["WordWrap"]->value().toInt())
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
    return QFont(props["FontFamily"]->value(),
        props["FontSize"]->value().toInt(),
        props["FontWeight"]->value().toInt(),
        props["FontItalic"]->value().toInt());
}

QPen CanvasLabel::getPenForText()
{
    return QPen(QColor(props["ForegroundColor"]->value().section(',', 0, 0).toInt(),
        props["ForegroundColor"]->value().section(',', 1, 1).toInt(),
        props["ForegroundColor"]->value().section(',', 2, 2).toInt()));
}

QPen CanvasLabel::getPenForShape()
{
    PenStyle style = SolidLine;
    switch (props["BorderStyle"]->value().toInt())
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
    return QPen(QColor(props["BorderColor"]->value().section(',', 0, 0).toInt(),
        props["BorderColor"]->value().section(',', 1, 1).toInt(),
        props["BorderColor"]->value().section(',', 2, 2).toInt()),
        props["BorderWidth"]->value().toInt(), style);
}

QBrush CanvasLabel::getBrush()
{
    return QBrush(QColor(props["BackgroundColor"]->value().section(',', 0, 0).toInt(),
        props["BackgroundColor"]->value().section(',', 1, 1).toInt(),
        props["BackgroundColor"]->value().section(',', 2, 2).toInt()));
}

void CanvasLabel::draw(QPainter &painter)
{
    //update dimensions
    if (!section()) return;

    setX(props["X"]->value().toInt() + section()->x());
    setY(props["Y"]->value().toInt() + section()->y());
    setSize(props["Width"]->value().toInt(), props["Height"]->value().toInt());

    //draw border and background
    painter.setPen(getPenForShape());
    painter.setBrush(getBrush());
    painter.drawRect(rect());

    //draw text inside
    painter.setFont(getFont());
    painter.setPen(getPenForText());
    painter.drawText((int)x(), (int)y(), width(), height(),
        getTextAlignment() | getTextWrap(),
        props["Text"]->value());

    //draw resizable region
    if (isSelected())
        drawHolders(painter);
//    CanvasReportItem::draw(painter);
}

QString CanvasLabel::getXml()
{
    return "\t\t<Label" + CanvasReportItem::getXml() + " />\n";
}
