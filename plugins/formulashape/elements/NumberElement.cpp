/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>
                 2007      Martin Pfeiffer <hubipete@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <QFontMetricsF>

#include "AttributeManager.h"
#include "NumberElement.h"

NumberElement::NumberElement(BasicElement *parent)
    : TokenElement(parent)
{
}

QRectF NumberElement::renderToPath(const QString &rawString, QPainterPath &path) const
{
    AttributeManager manager;
    QFont font = manager.font(this);
    path.addText(path.currentPosition(), font, rawString);
    QFontMetricsF fm(font);
    return fm.boundingRect(rawString);
}

ElementType NumberElement::elementType() const
{
    return Number;
}
