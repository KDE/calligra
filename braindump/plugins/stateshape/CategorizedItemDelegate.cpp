/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2.1 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "CategorizedItemDelegate.h"

#include <QPainter>

#include <KCategoryDrawer>
#include <KCategorizedSortFilterProxyModel>

struct CategorizedItemDelegate::Private {
    QAbstractItemDelegate* fallback;
    KCategoryDrawer* categoryDrawer;
    bool isFirstOfCategory(const QModelIndex& index);
};

bool CategorizedItemDelegate::Private::isFirstOfCategory(const QModelIndex& index)
{
    if(index.row() == 0) return true;
    QModelIndex idx = index.model()->index(index.row() - 1, index.column(), index.parent());
    const QString category1 = index.model()->data(index, KCategorizedSortFilterProxyModel::CategorySortRole).toString();
    const QString category2 = index.model()->data(idx, KCategorizedSortFilterProxyModel::CategorySortRole).toString();
    return category1 != category2;
}

CategorizedItemDelegate::CategorizedItemDelegate(QAbstractItemDelegate* _fallback, QObject* parent) : QAbstractItemDelegate(parent), d(new Private)
{
    _fallback->setParent(this);
    d->fallback = _fallback;
    // QT5TODO: Pass correct param to KCategoryDrawer
    d->categoryDrawer = new KCategoryDrawer(0);
}
CategorizedItemDelegate::~CategorizedItemDelegate()
{
    delete d;
}

QWidget * CategorizedItemDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    return d->fallback->createEditor(parent, option, index);
}

bool CategorizedItemDelegate::editorEvent(QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index)
{
    return d->fallback->editorEvent(event, model, option, index);
}

void CategorizedItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & _option, const QModelIndex & index) const
{
    // We will need to edit the option to make sure the header isn't drawned as selected
    QStyleOptionViewItem* option = 0;
    if(const QStyleOptionViewItem *v4 = qstyleoption_cast<const QStyleOptionViewItem*>(&_option)) {
        option = new QStyleOptionViewItem(*v4);
    } else if(const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3*>(&_option)) {
        option = new QStyleOptionViewItemV3(*v3);
    } else if(const QStyleOptionViewItemV2 *v2 = qstyleoption_cast<const QStyleOptionViewItemV2*>(&_option)) {
        option = new QStyleOptionViewItemV2(*v2);
    } else {
        option = new QStyleOptionViewItem(_option);
    }
    Q_ASSERT(option);
    // If it's a first category then we need to draw it
    if(d->isFirstOfCategory(index)) {
        // Prepare the rectangle for drawing the category
        int h = d->categoryDrawer->categoryHeight(index, *option);
        QRect rect = option->rect;

        // Make sure the category isn't drown as selected
        option->state &= (~QStyle::State_Selected);
        Q_ASSERT(!(option->state & QStyle::State_Selected));
        option->state &= (~QStyle::State_HasFocus);
        Q_ASSERT(!(option->state & QStyle::State_HasFocus));
        option->state &= (~QStyle::State_MouseOver);
        Q_ASSERT(!(option->state & QStyle::State_MouseOver));
        option->rect.setHeight(h);

        // draw the category
        d->categoryDrawer->drawCategory(index, 0, *option, painter);

        // Prepare the rectangle for the item
        option->rect = rect;
        option->rect.setY(rect.y() + h);
        option->rect.setHeight(rect.height() - h);
        option->state = _option.state;
    }
    d->fallback->paint(painter, *option, index);
    delete option;
}

void CategorizedItemDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
    d->fallback->setEditorData(editor, index);
}

void CategorizedItemDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
{
    d->fallback->setModelData(editor, model, index);
}

QSize CategorizedItemDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    QSize size = d->fallback->sizeHint(option, index);
    // If is first of a category, then add the space needed to paint the category
    if(d->isFirstOfCategory(index)) {
        size.setHeight(d->categoryDrawer->categoryHeight(index, option) + size.height());
    }
    return size;
}

void CategorizedItemDelegate::updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    d->fallback->updateEditorGeometry(editor, option, index);

    // If it's the first category, then the editor need to be moved
    if(d->isFirstOfCategory(index)) {
        int h = d->categoryDrawer->categoryHeight(index, option);
        editor->move(editor->x(), editor->y() + h);
        editor->resize(editor->width(), editor->height() - h);
    }
}
