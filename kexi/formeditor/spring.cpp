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

#include <qsizepolicy.h>
#include <qpainter.h>
#include <qdom.h>
#include <qvariant.h>
//Added by qt3to4:
#include <Q3PointArray>
#include <Q3CString>
#include <QPaintEvent>

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
    m_edit = true;
    m_orient = Qt::Horizontal;
    setSizeType((SizeType)QSizePolicy::Expanding);
}

Spring::~Spring()
{
}

void
Spring::setOrientation(Qt::Orientation orient)
{
    SizeType type = sizeType();
    m_orient = orient;
    setSizeType(type);
    repaint();
}

Spring::SizeType
Spring::sizeType() const
{
    if (m_orient == Qt::Vertical)
        return (SizeType)sizePolicy().verData();
    else
        return (SizeType)sizePolicy().horData();
}

void
Spring::setSizeType(SizeType size)
{
    if (m_orient == Qt::Vertical)
        setSizePolicy(QSizePolicy::Minimum, (QSizePolicy::SizeType)size);
    else
        setSizePolicy((QSizePolicy::SizeType)size, QSizePolicy::Minimum);
}

void
Spring::paintEvent(QPaintEvent *ev)
{
    if (!m_edit)
        return;

    QPainter p(this);
    if (!ev->erased())
        p.eraseRect(0, 0, width(), height());
    p.setPen(QPen(Qt::white, 1));
    p.setCompositionMode(QPainter::CompositionMode_Xor);
    Q3PointArray pa(4);
    if (m_orient == Qt::Vertical) {
        uint part = (height() + 16) / 16;
        if (part < 3)
            part = 3;
        uint w = width() - 1;
        uint offset = 0;
        for (uint i = 0; i < 4; i++, offset += (part * 4)) {
            pa.putPoints(0, 4,
                         w / 2, offset, w, offset + part, w, offset + part, w / 2, offset + part*2);
            p.drawCubicBezier(pa, 0);
            pa.putPoints(0, 4,
                         w / 2, offset + part*2, 0, offset + part*3, 0, offset + part*3, w / 2, offset + part*4);
            p.drawCubicBezier(pa, 0);
        }
    } else {
        uint part = (width() + 16) / 16;
        if (part < 3)
            part = 3;
        uint h = height() - 1;
        uint offset = 0;
        for (uint i = 0; i < 4; i++, offset += (part * 4)) {
            pa.putPoints(0, 4,
                         offset, h / 2, offset + part, 0, offset + part, 0, offset + part*2, h / 2);
            p.drawCubicBezier(pa, 0);
            pa.putPoints(0, 4,
                         offset + part*2, h / 2, offset + part*3, h, offset + part*3, h, offset + part*4, h / 2);
            p.drawCubicBezier(pa, 0);
        }
    }
}

bool
Spring::isPropertyVisible(const Q3CString &name)
{
    if ((name == "name") || (name == "sizeType") || (name == "orientation") || (name == "geometry"))
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

    KFormDesigner::FormIO::savePropertyValue(tclass, domDoc, "name", item->widget()->property("name"), item->widget());

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

