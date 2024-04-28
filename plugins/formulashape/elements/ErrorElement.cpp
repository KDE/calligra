/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ErrorElement.h"
#include <QPainter>

ErrorElement::ErrorElement(BasicElement *parent)
    : RowElement(parent)
{
}

void ErrorElement::paint(QPainter &painter, AttributeManager *am)
{
    Q_UNUSED(am)
    // just paint a red rectangle
    painter.save();
    QRectF tmp(0.0, 0.0, width(), height());
    painter.fillRect(tmp, QColor("red"));
    painter.restore();
}

ElementType ErrorElement::elementType() const
{
    return Error;
}
