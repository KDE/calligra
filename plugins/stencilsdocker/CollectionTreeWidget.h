/* This file is part of the Calligra project
 * Copyright (C) 2010-2014 Yue Liu <yue.liu@mail.com>
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

#ifndef COLLECTIONTREEWIDGET_H
#define COLLECTIONTREEWIDGET_H

#include <QItemDelegate>
#include <QTreeWidget>
#include <QString>
#include <QMap>
#include <QListView>

class QSortFilterProxyModel;
class CollectionItemModel;
class StencilListView;

class SheetDelegate: public QItemDelegate
{
    Q_OBJECT
public:
    SheetDelegate(QTreeView* view, QWidget* parent);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& opt, const QModelIndex& index) const override;

private:
    QTreeView* m_view;
};

class CollectionTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    explicit CollectionTreeWidget(QWidget* parent);
    ~CollectionTreeWidget() override;
    void setFamilyMap(QMap<QString, CollectionItemModel*> map);
    void regenerateFilteredMap();
    void setFilter(QRegExp regExp);

protected:
    void contextMenuEvent(QContextMenuEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;

private:
    QListView::ViewMode m_viewMode;
    QMap<QString, CollectionItemModel*> m_familyMap;
    QMap<QString, QSortFilterProxyModel*> m_filteredMap;
    StencilListView* stencilListViewAt(int idx) const;
    void addStencilListView(QTreeWidgetItem* parent, QListView::ViewMode viewMode, QSortFilterProxyModel* model);
    void adjustStencilListSize(QTreeWidgetItem* cat_item);
    void updateViewMode();
    void saveOptions();
    void loadOptions();

private Q_SLOTS:
    void handleMousePress(QTreeWidgetItem* item);
    void slotListMode();
    void slotIconMode();
};

#endif // COLLECTIONTREEWIDGET_H
