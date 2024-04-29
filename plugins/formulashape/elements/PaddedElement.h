/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PADDEDELEMENT_H
#define PADDEDELEMENT_H

#include "RowElement.h"
#include "koformula_export.h"

/**
 * @short Implementation of the MathML mpadded element
 *
 * The mpadded element adjusts the space around a sequence of elements.
 * Therefore it just implements the layout() method.
 */
class KOFORMULA_EXPORT PaddedElement : public RowElement
{
public:
    /// The standard constructor
    explicit PaddedElement(BasicElement *parent = nullptr);

    /**
     * Calculate the size of the element and the positions of its children
     * @param am The AttributeManager providing information about attributes values
     */
    void layout(const AttributeManager *am) override;

    /// @return The element's ElementType
    ElementType elementType() const override;
};

#endif // PADDEDELEMENT_H
