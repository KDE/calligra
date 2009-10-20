/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2005 Jarosław Staniek <staniek@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <QSizePolicy>
#include <QPainter>
#include <QDomDocument>
#include <QDomElement>
#include <QPolygon>
#include <QVariant>

#include <kdebug.h>

#include "objecttree.h"
#include "container.h"
#include "form.h"
#include "formIO.h"
#include "widgetlibrary.h"

#include "spring.h"

Spring::Spring(QWidget *parent)
        : QWidget(parent)
{
    m_orient = Qt::Horizontal;
    setSizeType(QSizePolicy::Expanding);
}

Spring::~Spring()
{
}

void
Spring::setOrientation(Qt::Orientation orient)
{
    QSizePolicy::Policy policy = sizeType();
    m_orient = orient;
    setSizeType(policy);
    repaint();
}

QSizePolicy::Policy
Spring::sizeType() const
{
    if (m_orient == Qt::Vertical)
        return sizePolicy().verticalPolicy();
    else
        return sizePolicy().horizontalPolicy();
}

void
Spring::setSizeType(QSizePolicy::Policy size)
{
    if (m_orient == Qt::Vertical)
        setSizePolicy(QSizePolicy::Minimum, size);
    else
        setSizePolicy(size, QSizePolicy::Minimum);
}

void
Spring::paintEvent(QPaintEvent *ev)
{
    if (!designMode())
        return;

    QPainter p(this);
    if (!ev->erased())
        p.eraseRect(0, 0, width(), height());
//todo?    p.setPen(QPen(Qt::white, 1));
//todo?    p.setCompositionMode(QPainter::CompositionMode_Xor);
    if (m_orient == Qt::Vertical) {
        uint part = (height() + 16) / 16;
        if (part < 3)
            part = 3;
        uint w = width() - 1;
        uint offset = 0;
        for (uint i = 0; i < 4; i++, offset += (part * 4)) {
            QPolygon poly1(4);
            poly1.putPoints(0, 4,
                         w / 2, offset, w, offset + part, w, offset + part, w / 2, offset + part*2);
            QPainterPath ppath1;
            ppath1.addPolygon(poly1);
            p.strokePath(ppath1, p.pen());
            QPolygon poly2(4);
            poly2.putPoints(0, 4,
                         w / 2, offset + part*2, 0, offset + part*3, 0, offset + part*3, w / 2, offset + part*4);
            QPainterPath ppath2;
            ppath2.addPolygon(poly2);
            p.strokePath(ppath2, p.pen());
        }
    } else {
        uint part = (width() + 16) / 16;
        if (part < 3)
            part = 3;
        uint h = height() - 1;
        uint offset = 0;
        for (uint i = 0; i < 4; i++, offset += (part * 4)) {
            QPolygon poly1(4);
            poly1.putPoints(0, 4,
                         offset, h / 2, offset + part, 0, offset + part, 0, offset + part*2, h / 2);
            QPainterPath ppath1;
            ppath1.addPolygon(poly1);
            p.strokePath(ppath1, p.pen());
            QPolygon poly2(4);
            poly2.putPoints(0, 4,
                         offset + part*2, h / 2, offset + part*3, h, offset + part*3, h, offset + part*4, h / 2);
            QPainterPath ppath2;
            ppath2.addPolygon(poly2);
            p.strokePath(ppath2, p.pen());
        }
    }
}

bool
Spring::isPropertyVisible(const QByteArray &name)
{
    if ((name == "objectName") || (name == "sizeType") || (name == "orientation") || (name == "geometry"))
        return true;

    return false;
}


void
Spring::saveSpring(KFormDesigner::ObjectTreeItem *item, QDomElement &parentNode, QDomDocument &domDoc, bool insideGridLayout)
{
    QDomElement tclass = domDoc.createElement("spacer");
    parentNode.appendChild(tclass);

    if (insideGridLayout) {
        tclass.setAttribute("row", item->gridRow());
        tclass.setAttribute("column", item->gridCol());
        if (item->spanMultipleCells()) {
            tclass.setAttribute("rowspan", item->gridRowSpan());
            tclass.setAttribute("colspan", item->gridColSpan());
        }
    }

    KFormDesigner::FormIO::savePropertyValue(tclass, domDoc, "objectName", item->widget()->property("objectName"), item->widget());

    if (parentNode.tagName() == "widget")
        KFormDesigner::FormIO::savePropertyValue(tclass, domDoc, "geometry", item->widget()->property("geometry"), item->widget());

    if (!item->widget()->sizeHint().isValid())
        KFormDesigner::FormIO::savePropertyValue(tclass, domDoc, "sizeHint", item->widget()->property("size"), item->widget());
    else
        KFormDesigner::FormIO::savePropertyValue(tclass, domDoc, "sizeHint", item->widget()->property("sizeHint"), item->widget());

    KFormDesigner::FormIO::savePropertyValue(tclass, domDoc, "orientation", item->widget()->property("orientation"), item->widget());
    KFormDesigner::FormIO::savePropertyValue(tclass, domDoc, "sizeType", item->widget()->property("sizeType"), item->widget());
}


#include "spring.moc"

