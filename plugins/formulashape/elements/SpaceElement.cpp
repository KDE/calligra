/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Andrea Rizzi <rizzi@kde.org>
                  Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
   SPDX-FileCopyrightText: 2006 Martin Pfeiffer <hubipete@gmx.net>
   SPDX-FileCopyrightText: 2006-2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "SpaceElement.h"
#include "AttributeManager.h"
#include <QBrush>
#include <QPainter>

SpaceElement::SpaceElement(BasicElement *parent)
    : BasicElement(parent)
{
}

void SpaceElement::paint(QPainter &painter, AttributeManager *)
{
    painter.setBrush(QBrush(Qt::lightGray, Qt::DiagCrossPattern));
    painter.drawRect(QRectF(0.0, 0.0, width(), height()));
}

void SpaceElement::layout(const AttributeManager *am)
{
    qreal height = am->doubleOf("height", this);
    setHeight(height + am->doubleOf("depth", this));
    setWidth(am->doubleOf("width", this));
    setBaseLine(height);
}

QString SpaceElement::attributesDefaultValue(const QString &attribute) const
{
    if (attribute == "width" || attribute == "height" || attribute == "depth")
        return "0.0";
    else
        return "auto";
}

ElementType SpaceElement::elementType() const
{
    return Space;
}
