/* This file is part of the Calligra project
 * SPDX-FileCopyrightText: 2010-2014 Yue Liu <yue.liu@mail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef COLLECTIONTREEWIDGET_H
#define COLLECTIONTREEWIDGET_H

#include <QItemDelegate>
#include <QListView>
#include <QMap>
#include <QString>
#include <QTreeWidget>

class QSortFilterProxyModel;
class CollectionItemModel;
class StencilListView;

class SheetDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    SheetDelegate(QTreeView *view, QWidget *parent);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &opt, const QModelIndex &index) const override;

private:
    QTreeView *m_view;
};

class CollectionTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    explicit CollectionTreeWidget(QWidget *parent);
    ~CollectionTreeWidget() override;
    void setFamilyMap(QMap<QString, CollectionItemModel *> map);
    void regenerateFilteredMap();
    void setFilter(QRegularExpression regExp);

protected:
    void contextMenuEvent(QContextMenuEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;

private:
    QListView::ViewMode m_viewMode;
    QMap<QString, CollectionItemModel *> m_familyMap;
    QMap<QString, QSortFilterProxyModel *> m_filteredMap;
    StencilListView *stencilListViewAt(int idx) const;
    void addStencilListView(QTreeWidgetItem *parent, QListView::ViewMode viewMode, QSortFilterProxyModel *model);
    void adjustStencilListSize(QTreeWidgetItem *cat_item);
    void updateViewMode();
    void saveOptions();
    void loadOptions();

private Q_SLOTS:
    void handleMousePress(QTreeWidgetItem *item);
    void slotListMode();
    void slotIconMode();
};

#endif // COLLECTIONTREEWIDGET_H
