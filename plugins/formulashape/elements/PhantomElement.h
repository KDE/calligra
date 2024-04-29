/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PHANTOMELEMENT_H
#define PHANTOMELEMENT_H

#include "RowElement.h"
#include "koformula_export.h"

/**
 * @short Implementation of the MathML mphantom element
 */
class KOFORMULA_EXPORT PhantomElement : public RowElement
{
public:
    /// The standard constructor
    explicit PhantomElement(BasicElement *parent = nullptr);

    /// @return The element's ElementType
    ElementType elementType() const override;
};

#endif // PHANTOMELEMENT_H
