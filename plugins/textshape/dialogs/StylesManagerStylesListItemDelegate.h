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

#ifndef STYLESMANAGERSTYLESLISTITEMDELEGATE_H
#define STYLESMANAGERSTYLESLISTITEMDELEGATE_H

#include <QStyledItemDelegate>

class StylesManagerStylesListItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit StylesManagerStylesListItemDelegate(QObject *parent = 0);

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option,                              const QModelIndex &index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

//    virtual bool editorEvent(QEvent *event, QAbstractItemModel *model,                             const QStyleOptionViewItem &option, const QModelIndex &index);
//    void setEditButtonEnable(bool enable);

signals:
//    void styleManagerButtonClicked(QModelIndex index);
//    void deleteStyleButtonClicked(QModelIndex index);
//    void needsUpdate(QModelIndex index);
//    void clickedInItem(QModelIndex index);

private:
//    bool m_editButtonPressed;
//    bool m_deleteButtonPressed;
//    bool m_enableEditButton;

//    int m_buttonSize;
//    int m_buttonDistance;
};

#endif // STYLESMANAGERSTYLESLISTITEMDELEGATE_H
