/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "IdentifierElement.h"

#include "AttributeManager.h"
#include "Dictionary.h"
#include "FormulaDebug.h"

#include <QFontMetricsF>

IdentifierElement::IdentifierElement(BasicElement *parent)
    : TokenElement(parent)
{
}

QRectF IdentifierElement::renderToPath(const QString &raw, QPainterPath &path) const
{
    AttributeManager manager;
    QFont font = manager.font(this);
    path.addText(path.currentPosition(), font, raw);
    QFontMetricsF fm(font);
    return fm.boundingRect(raw);
}

ElementType IdentifierElement::elementType() const
{
    return Identifier;
}
