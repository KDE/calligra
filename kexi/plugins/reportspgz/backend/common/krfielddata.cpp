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
#include "krfielddata.h"
#include <KoGlobal.h>
#include <kdebug.h>
#include <klocalizedstring.h>
#include <kglobalsettings.h>

KRFieldData::~KRFieldData()
{
}

KRFieldData::KRFieldData(QDomNode & element)
{
    createProperties();
    QDomNodeList nl = element.childNodes();

    QString n;
    QDomNode node;

    for (int i = 0; i < nl.count(); i++) {
        node = nl.item(i);
        n = node.nodeName();
        if (n == "data") {
            QDomNodeList dnl = node.childNodes();
            for (int di = 0; di < dnl.count(); di++) {
                node = dnl.item(di);
                n = node.nodeName();
                if (n == "query") {
//     _query->setValue ( node.firstChild().nodeValue() );
                } else if (n == "controlsource") {
                    _controlSource->setValue(node.firstChild().nodeValue());
                } else {
                    kDebug() << "while parsing field data encountered and unknown element: " << n;
                }
            }
            //txt = node.firstChild().nodeValue();
        } else if (n == "name") {
            _name->setValue(node.firstChild().nodeValue());
        } else if (n == "zvalue") {
            Z = node.firstChild().nodeValue().toDouble();
        } else if (n == "left") {
            _hAlignment->setValue("Left");
        } else if (n == "hcenter") {
            _hAlignment->setValue("Center");
        } else if (n == "right") {
            _hAlignment->setValue("Right");
        } else if (n == "top") {
            _vAlignment->setValue("Top");
        } else if (n == "vcenter") {
            _vAlignment->setValue("Center");
        } else if (n == "bottom") {
            _vAlignment->setValue("Bottom");
        } else if (n == "rect") {
            QRectF r;
            parseReportRect(node.toElement(), r);
            _pos.setPointPos(r.topLeft());
            _size.setPointSize(r.size());
        } else if (n == "textstyle") {
            ORTextStyleData ts;
            if (parseReportTextStyleData(node.toElement(), ts)) {
                _bgColor->setValue(ts.bgColor);
                _fgColor->setValue(ts.fgColor);
                _bgOpacity->setValue(ts.bgOpacity);
                _font->setValue(ts.font);
            }
        } else if (n == "linestyle") {
            ORLineStyleData ls;
            if (parseReportLineStyleData(node.toElement(), ls)) {
                _lnWeight->setValue(ls.weight);
                _lnColor->setValue(ls.lnColor);
                _lnStyle->setValue(ls.style);
            }
        }
        /*
        else if ( n == "tracktotal" )
        {
         _trackBuiltinFormat->setValue ( node.toElement().attribute ( "builtin" ) =="true"?true:false );
         _useSubTotal->setValue ( node.toElement().attribute ( "subtotal" ) =="true"?true:false );
         _trackTotalFormat->setValue ( node.firstChild().nodeValue() );
         if ( _trackTotalFormat->value().toString().length() > 0 ) _trackTotal->setValue ( true );
        }*/
        else {
            kDebug() << "while parsing field element encountered unknow element: " << n;
        }
    }
}

void KRFieldData::createProperties()
{
    _set = new KoProperty::Set(0, "Field");

    QStringList keys, strings;

    _controlSource = new KoProperty::Property("ControlSource", QStringList(), QStringList(), "", "Control Source");

    _controlSource->setOption("extraValueAllowed", "true");

    keys << "Left" << "Center" << "Right";
    strings << i18n("Left") << i18n("Center") << i18n("Right");
    _hAlignment = new KoProperty::Property("HAlign", keys, strings, "Left", "Horizontal Alignment");

    keys.clear();
    strings.clear();
    keys << "Top" << "Center" << "Bottom";
    strings << i18n("Top") << i18n("Center") << i18n("Bottom");
    _vAlignment = new KoProperty::Property("VAlign", keys, strings, "Center", "Vertical Alignment");

    _font = new KoProperty::Property("Font", KGlobalSettings::generalFont(), "Font", "Field Font");

    _bgColor = new KoProperty::Property("BackgroundColor", Qt::white, "Background Color", "Background Color");
    _fgColor = new KoProperty::Property("ForegroundColor", Qt::black, "Foreground Color", "Foreground Color");

    _bgOpacity = new KoProperty::Property("Opacity", 255, "Opacity", "Opacity");
    _bgOpacity->setOption("max", 255);
    _bgOpacity->setOption("min", 0);

    _lnWeight = new KoProperty::Property("Weight", 1, "Line Weight", "Line Weight");
    _lnColor = new KoProperty::Property("LineColor", Qt::black, "Line Color", "Line Color");
    _lnStyle = new KoProperty::Property("LineStyle", Qt::NoPen, "Line Style", "Line Style", KoProperty::LineStyle);

    //TODO I do not think we need these
    _trackTotal = new KoProperty::Property("TrackTotal", QVariant(false, 0), "Track Total", "Track Total");
    _trackBuiltinFormat = new KoProperty::Property("TrackBuiltinFormat", QVariant(false, 0), "Track Builtin Format", "Track Builtin Format");
    _useSubTotal = new KoProperty::Property("UseSubTotal", QVariant(false, 0), "Use Sub Total", "Use Sub Total");
    _trackTotalFormat = new KoProperty::Property("TrackTotalFormat", QString(), "Track Total Format", "Track Total Format");

    _set->addProperty(_name);
    _set->addProperty(_controlSource);
    _set->addProperty(_hAlignment);
    _set->addProperty(_vAlignment);
    _set->addProperty(_pos.property());
    _set->addProperty(_size.property());
    _set->addProperty(_font);
    _set->addProperty(_bgColor);
    _set->addProperty(_fgColor);
    _set->addProperty(_bgOpacity);
    _set->addProperty(_lnWeight);
    _set->addProperty(_lnColor);
    _set->addProperty(_lnStyle);
    //_set->addProperty ( _trackTotal );
    //_set->addProperty ( _trackBuiltinFormat );
    //_set->addProperty ( _useSubTotal );
    //_set->addProperty ( _trackTotalFormat );
}

Qt::Alignment KRFieldData::textFlags() const
{
    Qt::Alignment align;
    QString t;
    t = _hAlignment->value().toString();
    if (t == "Center")
        align = Qt::AlignHCenter;
    else if (t == "Right")
        align = Qt::AlignRight;
    else
        align = Qt::AlignLeft;

    t = _vAlignment->value().toString();
    if (t == "Center")
        align |= Qt::AlignVCenter;
    else if (t == "Bottom")
        align |= Qt::AlignBottom;
    else
        align |= Qt::AlignTop;

    return align;
}

ORTextStyleData KRFieldData::textStyle()
{
    ORTextStyleData d;
    d.bgColor = _bgColor->value().value<QColor>();
    d.fgColor = _fgColor->value().value<QColor>();
    d.font = _font->value().value<QFont>();
    d.bgOpacity = _bgOpacity->value().toInt();

    return d;
}

void KRFieldData::setTextFlags(Qt::Alignment f)
{
}

QString KRFieldData::column() const
{
    return _controlSource->value().toString();
}

void KRFieldData::setColumn(const QString& t)
{
    if (_controlSource->value() != t) {
        _controlSource->setValue(t);
    }
}

//bool    KRFieldData::trackTotal()       { return _trackTotal->value().toBool(); }
//bool    KRFieldData::trackBuiltinFormat() { return _trackBuiltinFormat->value().toBool(); }
//bool    KRFieldData::useSubTotal() { return _useSubTotal->value().toBool(); }
//QString KRFieldData::trackTotalFormat() { return _trackTotalFormat->value().toString(); }

void KRFieldData::setTrackTotal(bool yes)
{
    if (_trackTotal->value() != yes) {
        _trackTotal->setValue(yes);
    }
}
void KRFieldData::setTrackTotalFormat(const QString & str, bool builtin)
{
    if (_trackBuiltinFormat->value() != builtin || _trackTotalFormat->value() != str) {
        _trackBuiltinFormat->setValue(builtin);
        _trackTotalFormat->setValue(str);
    }
}
void KRFieldData::setUseSubTotal(bool yes)
{
    if (_useSubTotal->value() != yes) {
        _useSubTotal->setValue(yes);
    }
}

ORLineStyleData KRFieldData::lineStyle()
{
    ORLineStyleData ls;
    ls.weight = _lnWeight->value().toInt();
    ls.lnColor = _lnColor->value().value<QColor>();
    ls.style = (Qt::PenStyle)_lnStyle->value().toInt();
    return ls;
}
// RTTI
int KRFieldData::type() const
{
    return RTTI;
}
int KRFieldData::RTTI = KRObjectData::EntityField;
KRFieldData * KRFieldData::toField()
{
    return this;
}
