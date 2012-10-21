/* This file is part of the KDE project
 * Copyright (C) 2012 Pierre Stirnweiss <pstirnweiss@googlemail.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
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

#include "StylesManagerStylesListItemDelegate.h"

#include "StylesManagerStylesListModel.h"

#include <QApplication>
#include <QPainter>
#include <QStyleOptionViewItemV4>

StylesManagerStylesListItemDelegate::StylesManagerStylesListItemDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

void StylesManagerStylesListItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &optionV1, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 option = optionV1;
    initStyleOption(&option, index);

    if (!index.data(StylesManagerStylesListModel::isTitleRole).toBool()) {
        QStyledItemDelegate::paint(painter, option, index);
    }
    else {
        const QString category = index.data().toString();
        const QRect optRect = option.rect;
        QFont font(QApplication::font());
        font.setBold(true);
        const QFontMetrics fontMetrics = QFontMetrics(font);
        QColor outlineColor = option.palette.text().color();
        outlineColor.setAlphaF(0.35);
        //BEGIN: top left corner
        {
            painter->save();
            painter->setPen(outlineColor);
            const QPointF topLeft(optRect.topLeft());
            QRectF arc(topLeft, QSizeF(4, 4));
            arc.translate(0.5, 0.5);
            painter->drawArc(arc, 1440, 1440);
            painter->restore();
        }
        //END: top left corner
        //BEGIN: left vertical line
        {
            QPoint start(optRect.topLeft());
            start.ry() += 3;
            QPoint verticalGradBottom(optRect.topLeft());
            verticalGradBottom.ry() += fontMetrics.height() + 5;
            QLinearGradient gradient(start, verticalGradBottom);
            gradient.setColorAt(0, outlineColor);
            gradient.setColorAt(1, Qt::transparent);
            painter->fillRect(QRect(start, QSize(1, fontMetrics.height() + 5)), gradient);
        }
        //END: left vertical line
        //BEGIN: horizontal line
        {
            QPoint start(optRect.topLeft());
            start.rx() += 3;
            QPoint horizontalGradTop(optRect.topLeft());
            horizontalGradTop.rx() += optRect.width() - 6;
            painter->fillRect(QRect(start, QSize(optRect.width() - 6, 1)), outlineColor);
        }
        //END: horizontal line
        //BEGIN: top right corner
        {
            painter->save();
            painter->setPen(outlineColor);
            QPointF topRight(optRect.topRight());
            topRight.rx() -= 4;
            QRectF arc(topRight, QSizeF(4, 4));
            arc.translate(0.5, 0.5);
            painter->drawArc(arc, 0, 1440);
            painter->restore();
        }
        //END: top right corner
        //BEGIN: right vertical line
        {
            QPoint start(optRect.topRight());
            start.ry() += 3;
            QPoint verticalGradBottom(optRect.topRight());
            verticalGradBottom.ry() += fontMetrics.height() + 5;
            QLinearGradient gradient(start, verticalGradBottom);
            gradient.setColorAt(0, outlineColor);
            gradient.setColorAt(1, Qt::transparent);
            painter->fillRect(QRect(start, QSize(1, fontMetrics.height() + 5)), gradient);
        }
        //END: right vertical line
        //BEGIN: text
        {
            QRect textRect(option.rect);
            textRect.setTop(textRect.top() + 7);
            textRect.setLeft(textRect.left() + 7);
            textRect.setHeight(fontMetrics.height());
            textRect.setRight(textRect.right() - 7);
            painter->save();
            painter->setFont(font);
            QColor penColor(option.palette.text().color());
            penColor.setAlphaF(0.6);
            painter->setPen(penColor);
            painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, category);
            painter->restore();
        }
        //END: text
    }
}

QSize StylesManagerStylesListItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    return index.data(Qt::SizeHintRole).toSize();
}
