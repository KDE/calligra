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
#include "krtextdata.h"
#include <koproperty/Property.h>
#include <koproperty/Set.h>
#include <KoGlobal.h>
#include <kdebug.h>
#include <klocalizedstring.h>
#include <kglobalsettings.h>

KRTextData::KRTextData(QDomNode & element) : m_bottomPadding(0.0)
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
                /*if ( n == "query" )
                {
                 _query->setValue ( node.firstChild().nodeValue() );
                }
                else */if (n == "controlsource") {
                    m_controlSource->setValue(node.firstChild().nodeValue());
                } else {
                    kDebug() << "while parsing field data encountered and unknown element: " << n;
                }
            }
	} else if (n == "name") {
            m_name->setValue(node.firstChild().nodeValue());
        } else if (n == "zvalue") {
            Z = node.firstChild().nodeValue().toDouble();
        } else if (n == "bottompad") {
            m_bottomPadding = node.firstChild().nodeValue().toDouble() / 100.0;
        } else if (n == "left") {
            m_horizontalAlignment->setValue("Left");
        } else if (n == "hcenter") {
            m_horizontalAlignment->setValue("Center");
        } else if (n == "right") {
            m_horizontalAlignment->setValue("Right");
        } else if (n == "top") {
            m_verticalAlignment->setValue("Top");
        } else if (n == "vcenter") {
            m_verticalAlignment->setValue("Center");
        } else if (n == "bottom") {
            m_verticalAlignment->setValue("Bottom");
        } else if (n == "rect") {
            QRectF r;
            parseReportRect(node.toElement(), r);
            m_pos.setPointPos(r.topLeft());
            m_size.setPointSize(r.size());
        } else if (n == "textstyle") {

            ORTextStyleData ts;
            if (parseReportTextStyleData(node.toElement(), ts)) {
                m_backgroundColor->setValue(ts.backgroundColor);
                m_foregroundColor->setValue(ts.foregroundColor);
                m_backgroundOpacity->setValue(ts.backgroundOpacity);
                m_font->setValue(ts.font);
            }
        } else if (n == "linestyle") {
            ORLineStyleData ls;
            if (parseReportLineStyleData(node.toElement(), ls)) {
                m_lineWeight->setValue(ls.weight);
                m_lineColor->setValue(ls.lineColor);
                m_lineStyle->setValue(ls.style);
            }
        } else {
            kDebug() << "while parsing text element encountered unknow element: " << n;
        }
    }
}

KRTextData::~KRTextData()
{
}

Qt::Alignment KRTextData::textFlags() const
{
    Qt::Alignment align;
    QString t;
    t = m_horizontalAlignment->value().toString();
    if (t == "Center")
        align = Qt::AlignHCenter;
    else if (t == "Right")
        align = Qt::AlignRight;
    else
        align = Qt::AlignLeft;

    t = m_verticalAlignment->value().toString();
    if (t == "Center")
        align |= Qt::AlignVCenter;
    else if (t == "Bottom")
        align |= Qt::AlignBottom;
    else
        align |= Qt::AlignTop;

    return align;
}

void KRTextData::createProperties()
{
    m_set = new KoProperty::Set(0, "Text");

    //connect ( set, SIGNAL ( propertyChanged ( KoProperty::Set &, KoProperty::Property & ) ), this, SLOT ( propertyChanged ( KoProperty::Set &, KoProperty::Property & ) ) );

    QStringList keys, strings;

    //_query = new KoProperty::Property ( "Query", QStringList(), QStringList(), "Data Source", "Query" );
    m_controlSource = new KoProperty::Property("ControlSource", QStringList(), QStringList(), "", "Control Source");

    keys << "Left" << "Center" << "Right";
    strings << i18n("Left") << i18n("Center") << i18n("Right");
    m_horizontalAlignment = new KoProperty::Property("HAlign", keys, strings, "Left", "Horizontal Alignment");

    keys.clear();
    strings.clear();
    keys << "Top" << "Center" << "Bottom";
    strings << i18n("Top") << i18n("Center") << i18n("Bottom");
    m_verticalAlignment = new KoProperty::Property("VAlign", keys, strings, "Center", "Vertical Alignment");

    m_font = new KoProperty::Property("Font", KGlobalSettings::generalFont(), "Font", "Field Font");

    m_backgroundColor = new KoProperty::Property("BackgroundColor", Qt::white, "Background Color", "Background Color");
    m_foregroundColor = new KoProperty::Property("ForegroundColor", Qt::black, "Foreground Color", "Foreground Color");

    m_lineWeight = new KoProperty::Property("Weight", 1, "Line Weight", "Line Weight");
    m_lineColor = new KoProperty::Property("LineColor", Qt::black, "Line Color", "Line Color");
    m_lineStyle = new KoProperty::Property("LineStyle", Qt::NoPen, "Line Style", "Line Style", KoProperty::LineStyle);
    m_backgroundOpacity = new KoProperty::Property("Opacity", 255, "Opacity", "Opacity");
    m_backgroundOpacity->setOption("max", 255);
    m_backgroundOpacity->setOption("min", 0);

    //_set->addProperty ( _query );
    m_set->addProperty(m_name);
    m_set->addProperty(m_controlSource);
    m_set->addProperty(m_horizontalAlignment);
    m_set->addProperty(m_verticalAlignment);
    m_set->addProperty(m_pos.property());
    m_set->addProperty(m_size.property());
    m_set->addProperty(m_font);
    m_set->addProperty(m_backgroundColor);
    m_set->addProperty(m_foregroundColor);
    m_set->addProperty(m_backgroundOpacity);
    m_set->addProperty(m_lineWeight);
    m_set->addProperty(m_lineColor);
    m_set->addProperty(m_lineStyle);

}

void KRTextData::setTextFlags(Qt::Alignment f)
{

}

QString KRTextData::column() const
{
    return m_controlSource->value().toString();
}

qreal KRTextData::bottomPadding() const
{
    return m_bottomPadding;
}

void KRTextData::setBottomPadding(qreal bp)
{
    if (m_bottomPadding != bp) {
        m_bottomPadding = bp;
    }
}

ORTextStyleData KRTextData::textStyle()
{
    ORTextStyleData d;
    d.backgroundColor = m_backgroundColor->value().value<QColor>();
    d.foregroundColor = m_foregroundColor->value().value<QColor>();
    d.font = m_font->value().value<QFont>();
    d.backgroundOpacity = m_backgroundOpacity->value().toInt();
    return d;
}

ORLineStyleData KRTextData::lineStyle()
{
    ORLineStyleData ls;
    ls.weight = m_lineWeight->value().toInt();
    ls.lineColor = m_lineColor->value().value<QColor>();
    ls.style = (Qt::PenStyle)m_lineStyle->value().toInt();
    return ls;
}

// RTTI
int KRTextData::type() const
{
    return RTTI;
}
int KRTextData::RTTI = KRObjectData::EntityText;
KRTextData* KRTextData::toText()
{
    return this;
}
