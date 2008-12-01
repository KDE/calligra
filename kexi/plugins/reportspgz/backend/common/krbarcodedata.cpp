/*
 * Kexi Report Plugin
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact info@openmfg.com with any questions on this license.
 */

#include "krbarcodedata.h"
#include <koproperty/Property.h>
#include <koproperty/Set.h>
#include <KoGlobal.h>
#include <kdebug.h>
#include <klocalizedstring.h>
#include <kglobalsettings.h>

KRBarcodeData::KRBarcodeData(QDomNode & element)
{
    createProperties();
    QDomNodeList nl = element.childNodes();
    QString n;
    QDomNode node;
    for (int i = 0; i < nl.count(); i++) {
        node = nl.item(i);
        n = node.nodeName();
        if (n == "data") {
            // see "string" just below for comments on String vs. Data
            QDomNodeList dnl = node.childNodes();
            for (int di = 0; di < dnl.count(); di++) {
                node = dnl.item(di);
                n = node.nodeName();
                if (n == "controlsource") {
                    _controlSource->setValue(node.firstChild().nodeValue());
                } else {
                    kDebug() << "while parsing field data encountered and unknown element: " << n;
                }
            }
        } else if (n == "name") {
            _name->setValue(node.firstChild().nodeValue());
        } else if (n == "string") {
            // ok -- this entity wasn't really part of the initial spec for work
            // and from what i understand the data should be puilled from the database
            // however this string field as part of the xml def i received implies that it
            // is static.
        } else if (n == "zvalue") {
            Z = node.firstChild().nodeValue().toDouble();
        } else if (n == "format") {
            _format->setValue(node.firstChild().nodeValue());
        } else if (n == "rect") {
            QRectF r;
            parseReportRect(node.toElement(), r);
            _pos.setPointPos(r.topLeft());
            _size.setPointSize(r.size());
        } else if (n == "maxlength") {
            // this is the maximum length of a barcode value so that we can determine reasonably
            // what the minimum height of the barcode will be
            int i = node.firstChild().nodeValue().toInt();
            if (i < 1) i = 5;
            setMaxLength(i);
        } else if (n == "left") {
            _hAlignment->setValue("Left");
        } else if (n == "center") {
            _hAlignment->setValue("Center");
        } else if (n == "right") {
            _hAlignment->setValue("Right");
        } else {
            kDebug() << "while parsing barcode encountered unknow element: " << n;
        }
    }
}

void KRBarcodeData::setMaxLength(int i)
{
    if (i > 0) {
        if (_maxLength->value().toInt() != i) {
            _maxLength->setValue(i);
        }
        if (_format->value().toString() == "3of9") {
            int C = i; // number of characters
            int N = 2; // narrow mult for wide line
            int X = 1; // narrow line width
            int I = 1; // interchange line width
            min_width_data = (((C + 2) * ((3 * N) + 6) * X) + ((C + 1) * I)) / 100.0;
            min_height = min_width_data * 0.15;
            /*if(min_height < 0.25)*/ min_height = 0.25;
            min_width_total = min_width_data + 0.22; // added a little buffer to make sure we don't loose any
            // of our required quiet zone in conversions
        } else if (_format->value().toString() == "3of9+") {
            int C = i * 2; // number of characters
            int N = 2; // narrow mult for wide line
            int X = 1; // 1px narrow line
            int I = 1; // 1px narrow line interchange
            min_width_data = (((C + 2) * ((3 * N) + 6) * X) + ((C + 1) * I)) / 100.0;
            min_height = min_width_data * 0.15;
            /*if(min_height < 0.25)*/ min_height = 0.25;
            min_width_total = min_width_data + 0.22; // added a little buffer to make sure we don't loose any
            // of our required quiet zone in conversions
        } else if (_format->value().toString() == "128") {
            int C = i; // assuming 1:1 ratio of data passed in to data actually used in encoding
            int X = 1; // 1px wide
            min_width_data = (((11 * C) + 35) * X) / 100.0;       // assuming CODE A or CODE B
            min_height = min_width_data * 0.15;
            /*if(min_height < 0.25)*/ min_height = 0.25;
            min_width_total = min_width_data + 0.22; // added a little bugger to make sure we don't loose any
            // of our required quiet zone in conversions
        } else if (_format->value().toString() == "upc-a") {
            min_width_data = 0.95;
            min_width_total = 1.15;
            min_height = 0.25;
        } else if (_format->value().toString() == "upc-e") {
            min_width_data = 0.52;
            min_width_total = 0.70;
            min_height = 0.25;
        } else if (_format->value().toString() == "ean13") {
            min_width_data = 0.95;
            min_width_total = 1.15;
            min_height = 0.25;
        } else if (_format->value().toString() == "ean8") {
            min_width_data = 0.67;
            min_width_total = 0.90;
            min_height = 0.25;
        } else {
            kDebug() << "Unknown format encountered: " << _format->value().toString();
        }
    }
}

void KRBarcodeData::createProperties()
{
    _set = new KoProperty::Set(0, "Barcode");

    QStringList keys, strings;

    //_query = new KoProperty::Property ( "Query", QString(), "Query", "Query" );
    _controlSource = new KoProperty::Property("ControlSource", QStringList(), QStringList(), "Control Source");

    keys << "Left" << "Center" << "Right";
    strings << i18n("Left") << i18n("Center") << i18n("Right");
    _hAlignment = new KoProperty::Property("HAlign", keys, strings, "Left", "Horizontal Alignment");

    keys.clear();
    strings.clear();
    keys << "3of9" << "3of9+" << "128" << "upc-a" << "upc-e" << "ean13" << "ean8";
    strings << "3of9" << "3of9+" << "128" << "upc-a" << "upc-e" << "ean13" << "ean8";
    _format = new KoProperty::Property("Format", keys, strings, "3of9", "Barcode Format");

    _maxLength = new KoProperty::Property("Max Length", 5, "Max Length", "Maximum Barode Length");

    _set->addProperty(_name);
    _set->addProperty(_controlSource);
    _set->addProperty(_format);
    _set->addProperty(_hAlignment);
    _set->addProperty(_pos.property());
    _set->addProperty(_size.property());
    _set->addProperty(_maxLength);
}

KRBarcodeData::~KRBarcodeData()
{
}

int KRBarcodeData::alignment()
{
    QString a = _hAlignment->value().toString();

    if (a == "Left")
        return 0;
    else if (a == "Center")
        return 1;
    else if (a == "Right")
        return 2;
    else
        return 0;
}

QString KRBarcodeData::column()
{
    return _controlSource->value().toString();
}

QString KRBarcodeData::format()
{
    return _format->value().toString();
}

int KRBarcodeData::maxLength()
{
    return _maxLength->value().toInt();
}

void KRBarcodeData::setFormat(const QString& f)
{
    _format->setValue(f);
}

void KRBarcodeData::setAlignment(int)
{
    //TODO Barcode alignment
}

//RTTI
KRBarcodeData * KRBarcodeData::toBarcode()
{
    return this;
}
int KRBarcodeData::type() const
{
    return RTTI;
}
int KRBarcodeData::RTTI = KRObjectData::EntityBarcode;
