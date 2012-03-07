/*
 * Copyright (C) 2012  Yue Liu <yue.liu@mail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#ifndef STENCILLISTVIEW_H
#define STENCILLISTVIEW_H

#include <QListView>
#include <QStyledItemDelegate>

class ShapeItemDelegate: public QStyledItemDelegate
{
    Q_OBJECT
public:
    ShapeItemDelegate(QWidget *parent);
    ~ShapeItemDelegate();
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem &opt, const QModelIndex &index) const;
};

class StencilListView : public QListView
{
    Q_OBJECT
public:
    explicit StencilListView(QObject *parent = 0);
    using QListView::contentsSize;

signals:
    void pressed(const QString &name, const QString &xml, const QPoint &globalPos);
    //void itemRemoved();
    //void lastItemRemoved();

protected:
    //void mousePressEvent(QMouseEvent *event);
    //void mouseMoveEvent(QMouseEvent *event);

//private slots:
    //void removeCurrentItem();
    //void editCurrentItem();

private:
    //void performDrag();
    //QPoint m_startPos;
    //int mapRowToSource(int filterRow) const;
    //CollectionItemModel *m_model;
    
};

#endif // STENCILLISTVIEW_H
