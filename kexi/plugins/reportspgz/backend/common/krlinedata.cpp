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
#include <koproperty/property.h>
#include <koproperty/set.h>
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
            _name->setValue(node.firstChild().nodeValue());
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
    _start.setPointPos(QPointF(sx, sy));
    _end.setPointPos(QPointF(ex, ey));
    _lnWeight->setValue(ls.weight);
    _lnColor->setValue(ls.lnColor);
    _lnStyle->setValue(ls.style);

}


void KRLineData::createProperties()
{
    _set = new KoProperty::Set(0, "Line");

    _lnWeight = new KoProperty::Property("Weight", 1, "Line Weight", "Line Weight");
    _lnColor = new KoProperty::Property("LineColor", Qt::black, "Line Color", "Line Color");
    _lnStyle = new KoProperty::Property("LineStyle", Qt::SolidLine, "Line Style", "Line Style", KoProperty::LineStyle);
    _start.setName("Start");
    _end.setName("End");

    _set->addProperty(_name);
    _set->addProperty(_start.property());
    _set->addProperty(_end.property());
    _set->addProperty(_lnWeight);
    _set->addProperty(_lnColor);
    _set->addProperty(_lnStyle);
}

ORLineStyleData KRLineData::lineStyle()
{
    ORLineStyleData ls;
    ls.weight = _lnWeight->value().toInt();
    ls.lnColor = _lnColor->value().value<QColor>();
    ls.style = (Qt::PenStyle)_lnStyle->value().toInt();
    return ls;
}

unsigned int KRLineData::weight() const
{
    return _lnWeight->value().toInt();
}
void KRLineData::setWeight(int w)
{
    _lnWeight->setValue(w);
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

