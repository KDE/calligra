/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef STYLEELEMENT_H
#define STYLEELEMENT_H

#include "RowElement.h"
#include "koformula_export.h"

/**
 * @short The implementation of the MathML mstyle element
 *
 * This class is a very thin wrapper around RowElement just implementing elementType.
 * Basically it holds children like a row but it is able to apply almost any style on
 * them.
 */
class KOFORMULA_EXPORT StyleElement : public RowElement
{
public:
    /// The standard constructor
    explicit StyleElement(BasicElement *parent = nullptr);

    /// @return The element's ElementType
    ElementType elementType() const override;
};

#endif // STYLEELEMENT_H
