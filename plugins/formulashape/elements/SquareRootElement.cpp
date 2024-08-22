/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Martin Pfeiffer <hubipete@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "SquareRootElement.h"

#include "AttributeManager.h"
#include "FormulaDebug.h"

#include <QPainter>
#include <QPen>

SquareRootElement::SquareRootElement(BasicElement *parent)
    : RowElement(parent)
{
}

SquareRootElement::~SquareRootElement() = default;

void SquareRootElement::paint(QPainter &painter, AttributeManager *am)
{
    Q_UNUSED(am)
    //     BasicElement::paint(painter, am);
    QPen pen;
    pen.setWidth(m_lineThickness);
    painter.setPen(pen);
    painter.drawPath(m_rootSymbol);
}

void SquareRootElement::layout(const AttributeManager *am)
{
    RowElement::layout(am);

    qreal thinSpace = am->layoutSpacing(this);
    qreal symbolHeight = baseLine();
    if (height() > symbolHeight * 1.3)
        symbolHeight = height();
    symbolHeight += thinSpace;
    qreal tickWidth = symbolHeight / 3.0;

    m_lineThickness = am->lineThickness(this);

    // Set the sqrt dimensions
    QPointF childOffset(tickWidth + thinSpace, thinSpace + m_lineThickness);

    setWidth(width() + childOffset.x());
    setHeight(height() + childOffset.y());
    setBaseLine(baseLine() + childOffset.y());

    // Adapt the children's positions to the new offset
    foreach (BasicElement *element, childElements())
        element->setOrigin(element->origin() + childOffset);

    QRectF rect = childrenBoundingRect();
    rect.translate(childOffset);
    setChildrenBoundingRect(rect);

    // Draw the sqrt symbol into a QPainterPath as buffer
    m_rootSymbol = QPainterPath();
    m_rootSymbol.moveTo(m_lineThickness, 2.0 * symbolHeight / 3.0);
    m_rootSymbol.lineTo(0 + tickWidth / 2.0, symbolHeight - m_lineThickness / 2);
    m_rootSymbol.lineTo(0 + tickWidth, m_lineThickness / 2);
    m_rootSymbol.lineTo(width() - m_lineThickness / 2, m_lineThickness / 2);
}

ElementType SquareRootElement::elementType() const
{
    return SquareRoot;
}
