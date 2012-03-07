/*
    Part of Calligra project.
    Copyright (C) 2010-2012 Yue Liu <yue.liu@mail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#ifndef STENCILBOXTREEWIDGET_H
#define STENCILBOXTREEWIDGET_H

//#include "CollectionItemModel.h"

#include <QTreeWidget>
#include <QStyledItemDelegate>
#include <QListView>

class QIcon;
class QHash;
class StencilListView;
class StencilListModel;

/// A delegate from qt designer, used to paint stencil header.
class SheetDelegate: public QStyledItemDelegate
{
    Q_OBJECT
public:
    SheetDelegate(QTreeView *view, QWidget *parent);

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem &opt, const QModelIndex &index) const;

private:
    QTreeView *m_view;
};

class StencilBoxTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    explicit StencilBoxTreeWidget(QWidget *parent);
    ~StencilBoxTreeWidget();
    bool addStencil(const QStringList& parameters);

protected:
    void contextMenuEvent(QContextMenuEvent *e);
    void resizeEvent(QResizeEvent *e);

private:
    void adjustSubListSize(QTreeWidgetItem *cat_item);
    void saveLastStencils();
    void loadLastStencils();

    QHash<QString, StencilListModel*>* m_stencilHash;

private slots:
    void handleMousePress(QTreeWidgetItem *item);
};

#endif // STENCILBOXTREEWIDGET_H
