/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef STRINGELEMENT_H
#define STRINGELEMENT_H

#include "TokenElement.h"
#include "koformula_export.h"

/**
 * @short Implementation of the MathML ms element
 */
class KOFORMULA_EXPORT StringElement : public TokenElement
{
public:
    /// The standard constructor
    explicit StringElement(BasicElement *parent = nullptr);

    /// @return The element's ElementType
    ElementType elementType() const override;

    QRectF renderToPath(const QString &raw, QPainterPath &path) const override;
};

#endif // STRINGELEMENT_H
