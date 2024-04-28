/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Andrea Rizzi <rizzi@kde.org>
                      Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
                 2006 Martin Pfeiffer <hubipete@gmx.net>
   SPDX-FileCopyrightText: 2006-2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "FencedElement.h"
#include "AttributeManager.h"
#include "OperatorElement.h"
#include <QPainter>

FencedElement::FencedElement(BasicElement *parent)
    : RowElement(parent)
{
}

void FencedElement::paint(QPainter &painter, AttributeManager *am)
{
    Q_UNUSED(am)

    QPen pen(painter.pen());
    pen.setWidth(1);
    painter.setPen(pen);
    painter.drawPath(m_fence);
}

void FencedElement::layout(const AttributeManager *am)
{
    m_fence = QPainterPath(); // empty path buffer
    OperatorElement op;
    m_fence.addPath(op.renderForFence(am->stringOf("open", this), Prefix));

    const QString separators = am->stringOf("separators", this);
    int count = 0;
    foreach (const BasicElement *tmp, childElements()) {
        m_fence.moveTo(m_fence.currentPosition() + QPointF(tmp->width(), 0.0));
        if (tmp != childElements().last())
            m_fence.addPath(op.renderForFence(separators.at(count), Infix));
        if (count < separators.size() - 1) {
            count++;
        }
    }

    m_fence.addPath(op.renderForFence(am->stringOf("close", this), Postfix));

    setWidth(m_fence.boundingRect().width());
    setHeight(m_fence.boundingRect().height());
}

QString FencedElement::attributesDefaultValue(const QString &attribute) const
{
    if (attribute == "open")
        return "(";
    else if (attribute == "close")
        return ")";
    else if (attribute == "separators")
        return ",";
    else
        return QString();
}

ElementType FencedElement::elementType() const
{
    return Fenced;
}
