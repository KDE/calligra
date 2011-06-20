/*
    Part of Calligra project.
    Copyright (C) 2010 Yue Liu <opuspace@gmail.com>

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

#ifndef STENCILBOXVIEW_H
#define STENCILBOXVIEW_H

//#include "CollectionItemModel.h"

#include <QTreeWidget>
#include <QItemDelegate>
#include <QListView>

class QIcon;
class QSortFilterProxyModel;
class ShapeListView;
class CollectionItemModel;

///A delegate from qt designer, used to paint category item in the treewidget.
class SheetDelegate: public QItemDelegate
{
    Q_OBJECT
public:
    SheetDelegate(QTreeView *view, QWidget *parent);

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem &opt, const QModelIndex &index) const;

private:
    QTreeView *m_view;
};

class ShapeListView : public QListView
{
    Q_OBJECT
    
public:
    using QListView::contentsSize;

    explicit ShapeListView(QWidget *parent = 0);

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

class CollectionTreeWidget : public QTreeWidget
{
    Q_OBJECT
    
    public:
        explicit CollectionTreeWidget(QWidget *parent);
        ~CollectionTreeWidget();
        bool loadContents(const QString &contents);
        bool save();
        void setFamilyMap(QMap<QString, CollectionItemModel*> map);
        void regenerateFilteredMap();
        void setFilter(QRegExp regExp);

    protected:
	void contextMenuEvent(QContextMenuEvent *e);
	void resizeEvent(QResizeEvent *e);
	
    private:
        QListView::ViewMode m_viewMode;
        QMap<QString, CollectionItemModel*> m_familyMap;
        QMap<QString, QSortFilterProxyModel*> m_filteredMap;
        ShapeListView* categoryViewAt(int idx) const;
        void addCategoryView(QTreeWidgetItem *parent, bool iconMode, QSortFilterProxyModel *model);
	void adjustSubListSize(QTreeWidgetItem *cat_item);
	void updateViewMode();
	void saveViewMode();
	void restoreViewMode();
	void addWidgetItem(QString itemPath);
	
    private slots:
	void handleMousePress(QTreeWidgetItem *item);
	void slotListMode();
        void slotIconMode();
};

#endif // STENCILBOXVIEW_H
