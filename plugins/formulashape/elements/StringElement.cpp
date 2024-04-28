/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "StringElement.h"

StringElement::StringElement(BasicElement *parent)
    : TokenElement(parent)
{
}

QRectF StringElement::renderToPath(const QString &raw, QPainterPath &path) const
{
    Q_UNUSED(raw)
    Q_UNUSED(path)
    return QRectF();
}

ElementType StringElement::elementType() const
{
    return String;
}
