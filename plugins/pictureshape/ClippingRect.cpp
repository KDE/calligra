/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2011 Silvio Heinrich <plassy@web.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ClippingRect.h"

ClippingRect::ClippingRect()
    : top(0)
    , right(1)
    , bottom(1)
    , left(0)
    , uniform(true)
    , inverted(false)
{
}

ClippingRect::ClippingRect(const ClippingRect &rect)

    = default;

ClippingRect &ClippingRect::operator=(const ClippingRect &rect) = default;

ClippingRect::ClippingRect(const QRectF &rect, bool isUniform)
{
    setRect(rect, isUniform);
}

void ClippingRect::scale(const QSizeF &size, bool isUniform)
{
    top *= size.height();
    right *= size.width();
    bottom *= size.height();
    left *= size.width();
    uniform = isUniform;
}

void ClippingRect::normalize(const QSizeF &size)
{
    if (!uniform) {
        scale(QSizeF(1.0 / size.width(), 1.0 / size.height()), true);
    }

    if (inverted) {
        right = 1.0 - right;
        bottom = 1.0 - bottom;
        inverted = false;
    }
}

void ClippingRect::setRect(const QRectF &rect, bool isUniform)
{
    top = rect.top();
    right = rect.right();
    bottom = rect.bottom();
    left = rect.left();
    uniform = isUniform;
    inverted = false;
}

qreal ClippingRect::width() const
{
    return right - left;
}

qreal ClippingRect::height() const
{
    return bottom - top;
}

QRectF ClippingRect::toRect() const
{
    return QRectF(left, top, width(), height());
}
