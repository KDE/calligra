/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ENCLOSEELEMENT_H
#define ENCLOSEELEMENT_H

#include "RowElement.h"
#include "koformula_export.h"
#include <QPainterPath>

/**
 * @short Implementation of the MathML menclose element
 */
class KOFORMULA_EXPORT EncloseElement : public RowElement
{
public:
    /// The standard constructor
    explicit EncloseElement(BasicElement *parent = nullptr);

    /**
     * Render the element to the given QPainter
     * @param painter The QPainter to paint the element to
     * @param am AttributeManager containing style info
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
    /// The QPainterPath holding what the enclose element has to paint
    QPainterPath m_enclosePath;
};

#endif // ENCLOSEELEMENT_H
