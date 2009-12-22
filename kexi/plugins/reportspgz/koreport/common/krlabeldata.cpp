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
#include "krlabeldata.h"
#include <koproperty/Property.h>
#include <koproperty/Set.h>
#include <KoGlobal.h>
#include <kdebug.h>
#include <klocalizedstring.h>
#include <kglobalsettings.h>

KRLabelData::KRLabelData(QDomNode & element)
{
    createProperties();
    QDomNodeList nl = element.childNodes();
    QString n;
    QDomNode node;

    m_name->setValue(element.toElement().attribute("report:name"));
    m_text->setValue(element.toElement().attribute("report:caption"));
    Z = element.toElement().attribute("report:z-index").toDouble();
    m_horizontalAlignment->setValue(element.toElement().attribute("report:horizontal-align"));
    m_verticalAlignment->setValue(element.toElement().attribute("report:vertical-align"));

    for (int i = 0; i < nl.count(); i++) {
        node = nl.item(i);
        n = node.nodeName();

        if (n == "report:rect") {
            parseReportRect(node.toElement(), &m_pos, &m_size);
        } else if (n == "report:text-style") {
            KRTextStyleData ts;
            if (parseReportTextStyleData(node.toElement(), ts)) {
                m_backgroundColor->setValue(ts.backgroundColor);
                m_foregroundColor->setValue(ts.foregroundColor);
                m_backgroundOpacity->setValue(ts.backgroundOpacity);
                m_font->setValue(ts.font);

            }
        } else if (n == "report:line-style") {
            KRLineStyleData ls;
            if (parseReportLineStyleData(node.toElement(), ls)) {
                m_lineWeight->setValue(ls.weight);
                m_lineColor->setValue(ls.lineColor);
                m_lineStyle->setValue(ls.style);
            }
        } else {
            kDebug() << "while parsing label element encountered unknow element: " << n;
        }
    }
}

QString KRLabelData::text() const
{
    return m_text->value().toString();
}

void KRLabelData::setText(const QString& t)
{
    m_text->setValue(t);
}

void KRLabelData::createProperties()
{
    m_set = new KoProperty::Set(0, "Label");

    m_text = new KoProperty::Property("caption", "Label", "Caption", "Label Caption");
    QStringList keys, strings;

    keys << "left" << "center" << "right";
    strings << i18n("Left") << i18n("Center") << i18n("Right");
    m_horizontalAlignment = new KoProperty::Property("horizontal-align", keys, strings, "left", "Horizontal Alignment");

    keys.clear();
    strings.clear();
    keys << "top" << "center" << "bottom";
    strings << i18n("Top") << i18n("Center") << i18n("Bottom");
    m_verticalAlignment = new KoProperty::Property("vertical-align", keys, strings, "center", "Vertical Alignment");

    m_font = new KoProperty::Property("Font", KGlobalSettings::generalFont(), "Font", "Label Font");

    m_backgroundColor = new KoProperty::Property("background-color", Qt::white, "Background Color", "Background Color");
    m_foregroundColor = new KoProperty::Property("foreground-color", Qt::black, "Foreground Color", "Foreground Color");
    m_backgroundOpacity = new KoProperty::Property("background-opacity", 100, "Opacity", "Opacity");
    m_backgroundOpacity->setOption("max", 100);
    m_backgroundOpacity->setOption("min", 0);
    m_backgroundOpacity->setOption("unit", "%");

    m_lineWeight = new KoProperty::Property("line-weight", 1, "Line Weight", "Line Weight");
    m_lineColor = new KoProperty::Property("line-color", Qt::black, "Line Color", "Line Color");
    m_lineStyle = new KoProperty::Property("line-style", Qt::NoPen, "Line Style", "Line Style", KoProperty::LineStyle);

    m_set->addProperty(m_name);
    m_set->addProperty(m_text);
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

QRectF KRLabelData::_rect()
{
    QRectF r;
    r.setRect(m_pos.toScene().x(), m_pos.toScene().y(), m_size.toScene().width(), m_size.toScene().height());
    return r;
}

Qt::Alignment KRLabelData::textFlags() const
{
    Qt::Alignment align;
    QString t;
    t = m_horizontalAlignment->value().toString();
    if (t == "center")
        align = Qt::AlignHCenter;
    else if (t == "right")
        align = Qt::AlignRight;
    else
        align = Qt::AlignLeft;

    t = m_verticalAlignment->value().toString();
    if (t == "center")
        align |= Qt::AlignVCenter;
    else if (t == "bottom")
        align |= Qt::AlignBottom;
    else
        align |= Qt::AlignTop;

    return align;
}

KRTextStyleData KRLabelData::textStyle()
{
    KRTextStyleData d;
    d.backgroundColor = m_backgroundColor->value().value<QColor>();
    d.foregroundColor = m_foregroundColor->value().value<QColor>();
    d.font = m_font->value().value<QFont>();
    d.backgroundOpacity = m_backgroundOpacity->value().toInt();
    return d;
}

KRLineStyleData KRLabelData::lineStyle()
{
    KRLineStyleData ls;
    ls.weight = m_lineWeight->value().toInt();
    ls.lineColor = m_lineColor->value().value<QColor>();
    ls.style = (Qt::PenStyle)m_lineStyle->value().toInt();
    return ls;
}

// RTTI
int KRLabelData::type() const
{
    return RTTI;
}
int KRLabelData::RTTI = KRObjectData::EntityLabel;
KRLabelData * KRLabelData::toLabel()
{
    return this;
}

