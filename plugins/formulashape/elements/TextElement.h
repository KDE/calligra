/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Andrea Rizzi <rizzi@kde.org>
                      Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
   SPDX-FileCopyrightText: 2006 Martin Pfeiffer <hubipete@gmx.net>
   SPDX-FileCopyrightText: 2006 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TEXTELEMENT_H
#define TEXTELEMENT_H

#include "TokenElement.h"
#include "koformula_export.h"

/**
 * @short Implementation of the MathML
 * An element that represents one char.
 */
class KOFORMULA_EXPORT TextElement : public TokenElement
{
public:
    /// The standard constructor
    explicit TextElement(BasicElement *parent = nullptr);

    /// @return The element's ElementType
    ElementType elementType() const override;

    QRectF renderToPath(const QString &raw, QPainterPath &path) const override;
};

#endif // TEXTELEMENT_H
