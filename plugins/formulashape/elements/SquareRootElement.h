/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Martin Pfeiffer <hubipete@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef SQUAREROOTELEMENT_H
#define SQUAREROOTELEMENT_H

#include "RowElement.h"
#include "koformula_export.h"

#include <QPainterPath>

/**
 * @short Implementation of the MathML msqrt element
 *
 * The msqrt element is not implemented along with mroot element in a single class
 * as it is an inferred row element. That means that it takes any number of child
 * elements whereas mroot only accepts exactly 2 child elements. From a painting
 * and layouting perspective both are very similar except for the lack of a exponent
 * in square root.
 */
class KOFORMULA_EXPORT SquareRootElement : public RowElement
{
public:
    /// The standard constructor
    explicit SquareRootElement(BasicElement *parent = nullptr);

    /// The standard destructor
    ~SquareRootElement() override;

    /**
     * Render the element to the given QPainter
     * @param painter The QPainter to paint the element to
     * @param am The AttributeManager providing information about attributes values
     */
    void paint(QPainter &painter, AttributeManager *am) override;

    /**
     * Calculate the size of the element and the positions of its children
     * @param am The AttributeManager providing information about attributes values
     */
    void layout(const AttributeManager *am) override;

    /// @return The element's ElementType
    ElementType elementType() const override;

private:
    /// The point the artwork relates to.
    QPointF m_rootOffset;

    /// The QPainterPath that holds the lines for the root sign
    QPainterPath m_rootSymbol;

    /// Line thickness, in pixels
    qreal m_lineThickness;
};

#endif // SQUAREROOTELEMENT_H
