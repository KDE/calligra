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
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef PURE_QT
#include <klocale.h>
#include <klineeditdlg.h>
#else
#include "qlocale.h"
#include <qinputdialog.h>
#endif

#include "cfield.h"
#include "property.h"

CanvasField::CanvasField(int x, int y, int width, int height, QCanvas * canvas,bool reg):
    CanvasLabel(x, y, width, height, canvas)
{
    std::map<QString, QString> m;

    props["Field"] = *(new PropPtr(new Property(FieldName, "Field", i18n("Field to display"), i18n("Field Name"))));

    m["String"] = "0";
    m["Integer"] = "1";
    m["Float"] = "2";
    m["Date"] = "3";
    m["Currency"] = "4";
    props["DataType"] = *(new PropPtr(new Property("DataType", m, i18n("Data type"), "0")));
    m.clear();

    m["m/d/y"] = "0";
    m["m-d-y"] = "1";
    m["mm/dd/y"] = "2";
    m["mm-dd-y"] = "3";
    m["m/d/yyyy"] = "4";
    m["m-d-yyyy"] = "5";
    m["mm/dd/yyyy"] = "6";
    m["mm-dd-yyyy"] = "7";
    m["yyyy/m/d"] = "8";
    m["yyyy-m-d"] = "9";
    m["dd.mm.yy"] = "10";
    m["dd.mm.yyyy"] = "11";
    //TODO: make date format not hard-coded, use locale settings
    props["DateFormat"] = *(new PropPtr(new Property("DateFormat", m, i18n("Date format"), "11")));
    m.clear();

    props["Precision"] = *(new PropPtr(new Property(IntegerValue, "Precision", i18n("Number of digits after comma"), "2")));

    //TODO: make currency locale-aware
    props["Currency"] = *(new PropPtr(new Property(Symbol, "Currency", i18n("Currency symbol"), "32")));

    props["NegValueColor"] = *(new PropPtr(new Property(Color, "NegValueColor", i18n("Negative value color"), "0,0,0")));

    props["CommaSeparator"] = *(new PropPtr(new Property(Symbol, "CommaSeparator", i18n("Comma separator"), "44")));

    if (reg) registerAs(KuDesignerRttiCanvasField);
}

void CanvasField::draw(QPainter &painter)
{
    props["Text"]->setValue("[" + props["Field"]->value() + "]");
    CanvasLabel::draw(painter);
}

QString CanvasField::getXml()
{
    return "\t\t<Field" + CanvasReportItem::getXml() + " />\n";
}

void CanvasField::fastProperty()
{
    bool accepted;
    QString sValue = props["Field"]->value();

#ifndef PURE_QT
    QString sText = KLineEditDlg::getText( i18n( "Change Field" ),
            "Enter field name:", sValue , &accepted );
#else
    QString sText = QInputDialog::getText( i18n( "Change Field" ),
            "Enter field name:", QLineEdit::Normal, sValue , &accepted );
#endif

    if ( accepted )
        props["Field"]->setValue( sText );

}

