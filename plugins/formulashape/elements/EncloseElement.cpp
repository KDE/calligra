/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "EncloseElement.h"

#include "AttributeManager.h"
#include "FormulaDebug.h"

#include <QPainter>

EncloseElement::EncloseElement(BasicElement *parent)
    : RowElement(parent)
{
}

void EncloseElement::paint(QPainter &painter, AttributeManager *)
{
    painter.save();
    QPen pen;
    pen.setWidth(1);
    painter.setPen(pen);
    painter.drawPath(m_enclosePath);
    painter.restore();
}

void EncloseElement::layout(const AttributeManager *am)
{
    // TODO: actuarial (how does it look?) - radical - circle (how to determine extends )
    m_enclosePath = QPainterPath();
    QString tmpstring = am->stringOf("notation", this);
    QList<QString> tmp = tmpstring.split(' ');
    RowElement::layout(am);
    QRectF tmpRect = boundingRect();
    /*    if( tmp.contains( "longdiv" ) ) {
            m_enclosePath.moveTo( 0, 0 );
            m_enclosePath.lineTo();
        }*/
    if (tmp.contains("left")) {
        m_enclosePath.moveTo(0, 0);
        m_enclosePath.lineTo(0, tmpRect.height());
    }
    if (tmp.contains("right")) {
        m_enclosePath.moveTo(tmpRect.width(), 0);
        m_enclosePath.lineTo(tmpRect.width(), tmpRect.height());
    }
    if (tmp.contains("top")) {
        m_enclosePath.moveTo(0, 0);
        m_enclosePath.lineTo(tmpRect.width(), 0);
    }
    if (tmp.contains("bottom")) {
        m_enclosePath.moveTo(0, tmpRect.height());
        m_enclosePath.lineTo(tmpRect.width(), tmpRect.height());
    }
    if (tmp.contains("box")) { // TODO spacing is missing - might look odd
        m_enclosePath.addRect(0, 0, tmpRect.width(), tmpRect.height());
    }
    if (tmp.contains("roundedbox")) { // TODO spacing is missing - might look odd
        m_enclosePath.addRoundedRect(0, 0, tmpRect.width(), tmpRect.height(), 25, Qt::RelativeSize);
    }
    if (tmp.contains("updiagonalstrike")) {
        m_enclosePath.moveTo(0, tmpRect.height());
        m_enclosePath.lineTo(tmpRect.width(), 0);
    }
    if (tmp.contains("downdiagonalstrike")) {
        m_enclosePath.moveTo(0, 0);
        m_enclosePath.lineTo(tmpRect.width(), tmpRect.height());
    }
    if (tmp.contains("verticalstrike")) {
        m_enclosePath.moveTo(tmpRect.width() / 2, 0);
        m_enclosePath.lineTo(tmpRect.width() / 2, tmpRect.height());
    }
    if (tmp.contains("horizontalstrike")) {
        m_enclosePath.moveTo(0, tmpRect.height() / 2);
        m_enclosePath.lineTo(tmpRect.width(), tmpRect.height() / 2);
    }
    setWidth(tmpRect.width());
    setHeight(tmpRect.height());
}

ElementType EncloseElement::elementType() const
{
    return Enclose;
}
