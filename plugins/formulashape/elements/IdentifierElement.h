/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef IDENTIFIERELEMENT_H
#define IDENTIFIERELEMENT_H

#include "TokenElement.h"
#include "koformula_export.h"

/**
 * @short Implementation of the MathML mi element
 *
 * The mi element represents an identifier and is defined in the section 3.2.3
 * of the MathMl spec.
 */
class KOFORMULA_EXPORT IdentifierElement : public TokenElement
{
public:
    /// The standard constructor
    explicit IdentifierElement(BasicElement *parent = nullptr);

    /// @return The element's ElementType
    ElementType elementType() const override;

    /// Process @p raw and render it to @p path
    QRectF renderToPath(const QString &raw, QPainterPath &path) const override;
};

#endif // IDENTIFIERELEMENT_H
