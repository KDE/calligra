/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Andrea Rizzi <rizzi@kde.org>
                  Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
         2006 Martin Pfeiffer <hubipete@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "TextElement.h"
#include "AttributeManager.h"
#include "FormulaDebug.h"

#include <QFontMetricsF>

TextElement::TextElement(BasicElement *parent)
    : TokenElement(parent)
{
}

QRectF TextElement::renderToPath(const QString &raw, QPainterPath &path) const
{
    AttributeManager manager;

    QFont font = manager.font(this);
    path.addText(path.currentPosition(), font, raw);
    QFontMetricsF fm(font);
    QRectF box = fm.boundingRect(QRect(), Qt::TextIncludeTrailingSpaces, raw).adjusted(0, -fm.ascent(), 0, -fm.ascent());
    return box;
}

ElementType TextElement::elementType() const
{
    return Text;
}
