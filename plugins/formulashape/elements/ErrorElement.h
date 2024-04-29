/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ERRORELEMENT_H
#define ERRORELEMENT_H

#include "RowElement.h"

/**
 * @short Implementation of the MathML merror element
 */
class KOFORMULA_EXPORT ErrorElement : public RowElement
{
public:
    /// The standard constructor
    explicit ErrorElement(BasicElement *parent = nullptr);

    /**
     * Render the element to the given QPainter
     * @param painter The QPainter to paint the element to
     * @param am AttributeManager containing style info
     */
    void paint(QPainter &painter, AttributeManager *am) override;

    /// @return The element's ElementType
    ElementType elementType() const override;
};

#endif // ERRORELEMENT_H
