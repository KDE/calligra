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
#include "krlinedata.h"
#include <koproperty/Property.h>
#include <koproperty/Set.h>
#include <KoGlobal.h>
#include <kdebug.h>
#include <klocalizedstring.h>
#include <kglobalsettings.h>

KRLineData::KRLineData(QDomNode & element)
{
    createProperties();
    QDomNodeList nl = element.childNodes();
    QString n;
    QDomNode node;
    QPointF _s, _e;

    m_name->setValue(element.toElement().attribute("report:name"));
    Z = element.toElement().attribute("report:z-index").toDouble();

    _s.setX(element.toElement().attribute("report:xstart").toDouble());
    _s.setY(element.toElement().attribute("report:ystart").toDouble());
    _e.setX(element.toElement().attribute("report:xend").toDouble());
    _e.setY(element.toElement().attribute("report:yend").toDouble());
    m_start.setPointPos(_s);
    m_end.setPointPos(_e);

    for (int i = 0; i < nl.count(); i++) {
        node = nl.item(i);
        n = node.nodeName();

        if (n == "report:line-style") {
            KRLineStyleData ls;
            if (parseReportLineStyleData(node.toElement(), ls)) {
                m_lineWeight->setValue(ls.weight);
                m_lineColor->setValue(ls.lineColor);
                m_lineStyle->setValue(ls.style);
            }
        } else {
            kDebug() << "while parsing line element encountered unknow element: " << n;
        }
    }
}


void KRLineData::createProperties()
{
    m_set = new KoProperty::Set(0, "Line");

    m_lineWeight = new KoProperty::Property("line-weight", 1, "Line Weight", "Line Weight");
    m_lineColor = new KoProperty::Property("line-color", Qt::black, "Line Color", "Line Color");
    m_lineStyle = new KoProperty::Property("line-style", Qt::SolidLine, "Line Style", "Line Style", KoProperty::LineStyle);
    m_start.setName("Start");
    m_end.setName("End");

    m_set->addProperty(m_name);
    m_set->addProperty(m_start.property());
    m_set->addProperty(m_end.property());
    m_set->addProperty(m_lineWeight);
    m_set->addProperty(m_lineColor);
    m_set->addProperty(m_lineStyle);
}

KRLineStyleData KRLineData::lineStyle()
{
    KRLineStyleData ls;
    ls.weight = m_lineWeight->value().toInt();
    ls.lineColor = m_lineColor->value().value<QColor>();
    ls.style = (Qt::PenStyle)m_lineStyle->value().toInt();
    return ls;
}

unsigned int KRLineData::weight() const
{
    return m_lineWeight->value().toInt();
}
void KRLineData::setWeight(int w)
{
    m_lineWeight->setValue(w);
}

int KRLineData::type() const
{
    return RTTI;
}
int KRLineData::RTTI = KRObjectData::EntityLine;
KRLineData * KRLineData::toLine()
{
    return this;
}

