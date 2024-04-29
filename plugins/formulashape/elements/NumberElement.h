/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef NUMBERELEMENT_H
#define NUMBERELEMENT_H

#include "TokenElement.h"
#include "koformula_export.h"

/**
 * @short Implementation of the MathML mn element
 *
 * The mn element represents a number and is defined in the section 3.2.4 of the
 * MathML spec. The number element holds the number to be painted as a sequence
 * of TextElement and/or GlyphElements.
 */
class KOFORMULA_EXPORT NumberElement : public TokenElement
{
public:
    /// Standard constructor
    explicit NumberElement(BasicElement *parent = nullptr);

    /// @return The element's ElementType
    ElementType elementType() const override;

    /// Process @p raw and render it to @p path
    QRectF renderToPath(const QString &raw, QPainterPath &path) const override;
};

#endif // NUMBERELEMENT_H
