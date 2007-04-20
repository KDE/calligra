/* This file is part of the KDE project
  Copyright (C) 2006 - 2007 Dag Andersen <kplato@kde.org>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301, USA.
*/

#ifndef KPTITEMMODELBASE_H
#define KPTITEMMODELBASE_H

#include "kptglobal.h"

#include <QAbstractItemModel>
#include <QItemDelegate>
#include <QSplitter>
#include <QScrollBar>
#include <QTreeView>

namespace KPlato
{

namespace Delegate
{
    enum EditorType { EnumEditor, TimeEditor };
}

class Part;
class Project;

class SelectorDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    SelectorDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class EnumDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    EnumDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class DurationDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    DurationDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class DurationSpinBoxDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    DurationSpinBoxDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class SpinBoxDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    SpinBoxDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class DoubleSpinBoxDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    DoubleSpinBoxDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class MoneyDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    MoneyDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class TimeDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    TimeDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class ItemModelBase : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit ItemModelBase( Part *part, QObject *parent = 0 );
    ~ItemModelBase();

    Project *project() const { return m_project; }
    virtual void setProject( Project *project );
    virtual void setReadWrite( bool rw ) { m_readWrite = rw; }
    bool isReadWrite() { return m_readWrite; }

    /**
     * Re-implement to check if the @p data is allowed to be dropped on @p index,
     * @p dropIndicatorPosition indicates position relative @p index.
     */
    virtual bool dropAllowed( const QModelIndex &index, int dropIndicatorPosition, const QMimeData *data );
    
protected slots:
    virtual void slotLayoutToBeChanged();
    virtual void slotLayoutChanged();

protected:
    Part *m_part;
    Project *m_project;
    bool m_readWrite;
};

class TreeViewBase : public QTreeView
{
    Q_OBJECT
public:
    // Copy from protected enum in QAbstractItemView
    enum DropIndicatorPosition {
        OnItem = QAbstractItemView::OnItem,  /// The item will be dropped on the index.
        AboveItem = QAbstractItemView::AboveItem, /// The item will be dropped above the index.
        BelowItem = QAbstractItemView::BelowItem,  /// The item will be dropped below the index.
        OnViewport = QAbstractItemView::OnViewport /// The item will be dropped onto a region of the viewport with no items if acceptDropsOnView is set.
    };
    
    explicit TreeViewBase( QWidget *parent );

    void setArrowKeyNavigation( bool on ) { m_arrowKeyNavigation = on; }
    bool arrowKeyNavigation() const { return m_arrowKeyNavigation; }

    QModelIndex nextColumn( const QModelIndex &current ) const;
    QModelIndex previousColumn( const QModelIndex &current ) const;
    
    void setAcceptDropsOnView( bool mode ) { m_acceptDropsOnView = mode; }

    ItemModelBase *itemModel() const;
    
signals:
    void contextMenuRequested( QModelIndex, const QPoint& );
    void moveAfterLastColumn( const QModelIndex & );
    void moveBeforeFirstColumn( const QModelIndex & );
    
protected:
    void keyPressEvent(QKeyEvent *event);
    QItemSelectionModel::SelectionFlags selectionCommand(const QModelIndex &index, const QEvent *event) const;
    
    void contextMenuEvent ( QContextMenuEvent * event );
    
    void dragMoveEvent(QDragMoveEvent *event);
    
protected slots:
    virtual void currentChanged ( const QModelIndex & current, const QModelIndex & previous );
    
protected:
    bool m_arrowKeyNavigation;
    bool m_acceptDropsOnView;
};


class DoubleTreeViewBase : public QSplitter
{
    Q_OBJECT
public:
    explicit DoubleTreeViewBase( QWidget *parent );
    ~DoubleTreeViewBase();
    
    void setModel( ItemModelBase *model );
    ItemModelBase *model() const;
    
    void setArrowKeyNavigation( bool on ) { m_arrowKeyNavigation = on; }
    bool arrowKeyNavigation() const { return m_arrowKeyNavigation; }

    QItemSelectionModel *selectionModel() const { return m_selectionmodel; }
    
    void setSelectionMode( QAbstractItemView::SelectionMode mode );
    void setItemDelegateForColumn( int col, QAbstractItemDelegate * delegate );
    void setEditTriggers ( QAbstractItemView::EditTriggers );
    QAbstractItemView::EditTriggers editTriggers() const;
    
    void setAcceptDrops( bool );
    void setAcceptDropsOnView( bool );
    void setDropIndicatorShown( bool );
    void setDragDropMode( QAbstractItemView::DragDropMode mode );
    void setDragEnabled ( bool mode );

    void setStretchLastSection( bool );
    
signals:
    void contextMenuRequested( QModelIndex, const QPoint& );
    void currentChanged ( const QModelIndex & current, const QModelIndex & previous );
    void selectionChanged( const QModelIndexList );
    
public slots:
    void setExpanded( const QModelIndex & );
    void setCollapsed( const QModelIndex & );
    void edit( const QModelIndex &index );

protected slots:
    void slotSelectionChanged( const QItemSelection &sel, const QItemSelection & );
    void slotToRightView( const QModelIndex &index );
    void slotToLeftView( const QModelIndex &index );
    

protected:
    TreeViewBase *m_leftview;
    TreeViewBase *m_rightview;
    ItemModelBase *m_model;
    QItemSelectionModel *m_selectionmodel;
    bool m_arrowKeyNavigation;
};


} // namespace KPlato

#endif
