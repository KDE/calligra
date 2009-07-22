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
#include "parsexmlutils.h"

KRLineData::KRLineData(QDomNode & element)
{
    createProperties();
    // parse the xml entity we have been passed
    QDomNodeList nl = element.childNodes();
    QDomNode node;
    QString n;
    qreal sx = 0.0, sy = 0.0, ex = 0.0, ey = 0.0;
    ORLineStyleData ls;
    for (int i = 0; i < nl.count(); i++) {
        node = nl.item(i);
        n = node.nodeName();

        if (n == "name") {
            m_name->setValue(node.firstChild().nodeValue());
        } else if (n == "linestyle") {
            if (!parseReportLineStyleData(node.toElement(), ls)) {
                ls.lnColor = Qt::black;
                ls.weight = 1;
                ls.style = Qt::SolidLine;
            }
        } else if (n == "xstart") {
            sx = node.firstChild().nodeValue().toDouble() ;
            sx = ((sx - (int) sx) < 0.5 ? (int) sx : (int) sx + 1);
        } else if (n == "ystart") {
            sy = node.firstChild().nodeValue().toDouble();
            sy = ((sy - (int) sy) < 0.5 ? (int) sy : (int) sy + 1);
        } else if (n == "xend") {
            ex = node.firstChild().nodeValue().toDouble();
            ex = ((ex - (int) ex) < 0.5 ? (int) ex : (int) ex + 1);
        } else if (n == "yend") {
            ey = node.firstChild().nodeValue().toDouble();
            ey = ((ey - (int) ey) < 0.5 ? (int) ey : (int) ey + 1);
        } else if (n == "zvalue") {
            Z = node.firstChild().nodeValue().toDouble();
        } else {
            kDebug() << "While parsing line encountered unknown element: " << n;
        }
    }
    m_start.setPointPos(QPointF(sx, sy));
    m_end.setPointPos(QPointF(ex, ey));
    m_lineWeight->setValue(ls.weight);
    m_lineColor->setValue(ls.lnColor);
    m_lineStyle->setValue(ls.style);

}


void KRLineData::createProperties()
{
    m_set = new KoProperty::Set(0, "Line");

    m_lineWeight = new KoProperty::Property("Weight", 1, "Line Weight", "Line Weight");
    m_lineColor = new KoProperty::Property("LineColor", Qt::black, "Line Color", "Line Color");
    m_lineStyle = new KoProperty::Property("LineStyle", Qt::SolidLine, "Line Style", "Line Style", KoProperty::LineStyle);
    m_start.setName("Start");
    m_end.setName("End");

    m_set->addProperty(m_name);
    m_set->addProperty(m_start.property());
    m_set->addProperty(m_end.property());
    m_set->addProperty(m_lineWeight);
    m_set->addProperty(m_lineColor);
    m_set->addProperty(m_lineStyle);
}

ORLineStyleData KRLineData::lineStyle()
{
    ORLineStyleData ls;
    ls.weight = m_lineWeight->value().toInt();
    ls.lnColor = m_lineColor->value().value<QColor>();
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

