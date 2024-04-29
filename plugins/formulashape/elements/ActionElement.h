/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ACTIONELEMENT_H
#define ACTIONELEMENT_H

#include "RowElement.h"
#include "koformula_export.h"

/**
 * @short Implementation of the MathML maction element
 *
 * Support for action elements in MathML. According to MathML spec
 * (Section 3.6.1.1), a MathML conformant application is not required to
 * recognize any single actiontype.
 */
class KOFORMULA_EXPORT ActionElement : public RowElement
{
public:
    /// The standard constructor
    explicit ActionElement(BasicElement *parent = nullptr);

    /// @return The element's ElementType
    ElementType elementType() const override;
};

#endif // ACTIONELEMENT_H
