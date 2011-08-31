/* This file is part of the KDE project
 * Copyright (C) 2011 Paul Mendez <paulestebanms@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or ( at your option ) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KPrSlidesSorterItemDelegate.h"

//Qt headers
#include <QPainter>

KPrSlidesSorterItemDelegate::KPrSlidesSorterItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

KPrSlidesSorterItemDelegate::~KPrSlidesSorterItemDelegate()
{
}

void KPrSlidesSorterItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    QFont font = option.font;
    QFontMetrics fm(font);
    QString slideNumber = QString("%1").arg(index.row() + 1);
    QRect slideNumberRect = option.rect;
    slideNumberRect.setLeft(option.rect.left() + 5);
    int offSet = option.rect.height() > 0 ? 600 / option.rect.height(): 0;
    slideNumberRect.setTop(option.rect.top() + 5 - offSet);
    slideNumberRect.setBottom(slideNumberRect.top() + fm.height());
    slideNumberRect.setRight(option.rect.left() + 30);
    painter->setFont(font);
    painter->drawText(slideNumberRect, slideNumber);
    painter->restore();
}
