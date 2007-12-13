/* This file is part of the KDE project
  Copyright (C) 2006 Dag Andersen <danders@get2net.dk>

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
  Boston, MA 02110-1301, USA.
*/

#ifndef KPLATO_VIEWBASE
#define KPLATO_VIEWBASE

#include "kplatoui_export.h"
#include "kptitemmodelbase.h"

#include <KoView.h>

#include <QMap>
#include <QTreeView>
#include <QSplitter>

class KAction;

class QWidget;

class KoDocument;

/// The main namespace
namespace KPlato
{

class Project;
class Node;
class Resource;
class ResourceGroup;
class Relation;
class Calendar;
class Context;

class ItemModelBase;

/** 
 ViewBase is the baseclass of all sub-views to View.

*/
class KPLATOUI_EXPORT ViewBase : public KoView
{
    Q_OBJECT
public:
    /// Contructor
    ViewBase(KoDocument *doc, QWidget *parent);
    /// Destructor
    virtual ~ViewBase() {}
    /// Return the part (document) this view handles
    KoDocument *part() const;
    
    /// Return the type of view this is (class name)
    QString viewType() const { return metaObject()->className(); }
    
    virtual void setZoom(double /*zoom*/) {}
    /// Set the project this view shall handle.
    virtual void setProject( Project * /*project*/ ) {}
    /// Draw data from current part / project
    virtual void draw() {}
    /// Draw data from project.
    virtual void draw(Project &/*project*/) {}
    /// Draw changed data from project.
    virtual void drawChanges(Project &project) { draw(project); }
    /// Set readWrite mode
    virtual void updateReadWrite( bool );

    /// Reimplement if your view handles nodes
    virtual Node* currentNode() const { return 0; }
    /// Reimplement if your view handles resources
    virtual Resource* currentResource() const { return 0; }
    /// Reimplement if your view handles resource groups
    virtual ResourceGroup* currentResourceGroup() const { return 0; }
    /// Reimplement if your view handles calendars
    virtual Calendar* currentCalendar() const { return 0; }
    /// Reimplement if your view handles relations
    virtual Relation *currentRelation() const { return 0; }
    
    /// Returns the list of action lists that shall be plugged/unplugged
    QStringList actionListNames() const { return m_actionListMap.keys(); }
    /// Returns the list of actions associated with the action list name
    QList<QAction*> actionList( const QString name ) const { return m_actionListMap[name]; }
    /// Add an action to the specified action list
    void addAction( const QString list, QAction *action ) { m_actionListMap[list].append( action ); }
    
    /// Loads context info into this view. Reimplement.
    virtual bool loadContext( const KoXmlElement &/*context*/ ) { return false; }
    /// Save context info from this view. Reimplement.
    virtual void saveContext( QDomElement &/*context*/ ) const {}
    
    virtual QList<QAction*> viewlistActionList() const { return m_viewlistActionList; }
    void addViewlistAction( QAction *action ) { m_viewlistActionList.append( action ); }
    
    virtual QList<QAction*> contextActionList() const { return m_contextActionList; }
    void addContextAction( QAction *action ) { m_contextActionList.append( action ); }
    
    virtual ViewBase *hitView( const QPoint &pos );

public slots:
    /// Activate/deactivate the gui
    virtual void setGuiActive( bool activate );

signals:
    /// Emitted when the gui has been activated or deactivated
    void guiActivated( ViewBase*, bool );
    
protected:
    /// List of all menu/toolbar actions (used for plug/unplug)
    QMap<QString, QList<QAction*> > m_actionListMap;

    /// List of actions that will be shown in the viewlist context menu
    QList<QAction*> m_viewlistActionList;
    
    /// List of actions that will be shown in the views header context menu
    QList<QAction*> m_contextActionList;
};

class KPLATOUI_EXPORT TreeViewBase : public QTreeView
{
    Q_OBJECT
public:
    explicit TreeViewBase( QWidget *parent );

    void setReadWrite( bool rw );
    virtual void createItemDelegates();
    void setArrowKeyNavigation( bool on ) { m_arrowKeyNavigation = on; }
    bool arrowKeyNavigation() const { return m_arrowKeyNavigation; }

    QModelIndex nextColumn( const QModelIndex &current ) const;
    QModelIndex previousColumn( const QModelIndex &current ) const;

    void setAcceptDropsOnView( bool mode ) { m_acceptDropsOnView = mode; }

    ItemModelBase *model() const;
    virtual void setModel( QAbstractItemModel *model );

    virtual void setSelectionModel( QItemSelectionModel *model );

    void setStretchLastSection( bool );

    void mapToSection( int column, int section );
    int section( int col ) const;

    void setColumnsHidden( const QList<int> &list );

    /// Loads context info into this view. Reimplement.
    virtual bool loadContext( const KoXmlElement &/*context*/ );
    /// Save context info from this view. Reimplement.
    virtual void saveContext( QDomElement &/*context*/ ) const;

    /**
      Reimplemented to fix qt bug 160083: Doesn't scroll horisontally.
    
      Scroll the contents of the tree view until the given model item
      \a index is visible. The \a hint parameter specifies more
      precisely where the item should be located after the
      operation.
      If any of the parents of the model item are collapsed, they will
      be expanded to ensure that the model item is visible.
    */
    void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible);
    
signals:
    /// Context menu requested from viewport at global position @p pos
    void contextMenuRequested( QModelIndex, const QPoint &pos );
    /// Context menu requested from header at global position @p pos
    void headerContextMenuRequested( const QPoint &pos );

    void moveAfterLastColumn( const QModelIndex & );
    void moveBeforeFirstColumn( const QModelIndex & );

protected:
    void keyPressEvent(QKeyEvent *event);
    /**
      Reimplemented from QTreeView to make tab/backtab in editor work reasonably well.
      Move the cursor in the way described by \a cursorAction, *not* using the
      information provided by the button \a modifiers.
    */
    QModelIndex moveCursor( CursorAction cursorAction, Qt::KeyboardModifiers modifiers );

    void contextMenuEvent ( QContextMenuEvent * event );

    void dragMoveEvent(QDragMoveEvent *event);

protected slots:
    /// Close the @p editor, using sender()->endEditHint().
    /// Use @p hint if sender is not of type ItemDelegate.
    virtual void closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint);
    
    virtual void slotCurrentChanged ( const QModelIndex & current, const QModelIndex & previous );
    void slotHeaderContextMenuRequested( const QPoint& );

    //Copied from QAbstractItemView
    inline QItemSelectionModel::SelectionFlags selectionBehaviorFlags() const
    {
        switch (selectionBehavior()) {
            case QAbstractItemView::SelectRows: return QItemSelectionModel::Rows;
            case QAbstractItemView::SelectColumns: return QItemSelectionModel::Columns;
            case QAbstractItemView::SelectItems: default: return QItemSelectionModel::NoUpdate;
        }
    }

protected:
    virtual void focusInEvent(QFocusEvent *event);
    
    bool m_arrowKeyNavigation;
    bool m_acceptDropsOnView;
    QList<int> m_hideList;
    bool m_readWrite;
};


class KPLATOUI_EXPORT DoubleTreeViewBase : public QSplitter
{
    Q_OBJECT
public:
    explicit DoubleTreeViewBase( QWidget *parent );
    DoubleTreeViewBase( bool mode, QWidget *parent );
    ~DoubleTreeViewBase();

    void setReadWrite( bool rw );

    void setModel( ItemModelBase *model );
    ItemModelBase *model() const;

    void setArrowKeyNavigation( bool on ) { m_arrowKeyNavigation = on; }
    bool arrowKeyNavigation() const { return m_arrowKeyNavigation; }

    QItemSelectionModel *selectionModel() const { return m_selectionmodel; }
    void setSelectionModel( QItemSelectionModel *model );
    void setSelectionMode( QAbstractItemView::SelectionMode mode );
    void setSelectionBehavior( QAbstractItemView::SelectionBehavior mode );
    virtual void createItemDelegates();
    void setItemDelegateForColumn( int col, QAbstractItemDelegate * delegate );
    void setEditTriggers ( QAbstractItemView::EditTriggers );
    QAbstractItemView::EditTriggers editTriggers() const;

    void setAcceptDrops( bool );
    void setAcceptDropsOnView( bool );
    void setDropIndicatorShown( bool );
    void setDragDropMode( QAbstractItemView::DragDropMode mode );
    void setDragEnabled ( bool mode );

    void setStretchLastSection( bool );
    
    /// Hide columns in the @p hideList, show all other columns.
    /// If the hideList.last() == -1, the rest of the columns are hidden.
    void hideColumns( TreeViewBase *view, const QList<int> &hideList );
    void hideColumns( const QList<int> &masterList, const QList<int> List = QList<int>() );
    void hideColumn( int col ) {
        m_leftview->hideColumn( col ); 
        if ( m_rightview ) m_rightview->hideColumn( col );
    }
    void showColumn( int col ) { 
        if ( col == 0 || m_rightview == 0 ) m_leftview->showColumn( col );
        else m_rightview->showColumn( col );
    }
    bool isColumnHidden( int col ) const {
        return m_rightview ? m_rightview->isColumnHidden( col ) : m_leftview->isColumnHidden( col );
    }

    TreeViewBase *masterView() const { return m_leftview; }
    TreeViewBase *slaveView() const { return m_rightview; }

    /// Loads context info into this view. Reimplement.
    virtual bool loadContext( const KoXmlElement &/*context*/ );
    /// Save context info from this view. Reimplement.
    virtual void saveContext( QDomElement &/*context*/ ) const;
    
    void setViewSplitMode( bool split );
    bool isViewSplit() const { return m_mode; }
    KAction *actionSplitView() const { return m_actionSplitView; }
    
    void setRootIsDecorated ( bool show );

signals:
    /// Context menu requested from the viewport, pointer over @p index at global position @p pos
    void contextMenuRequested( QModelIndex index, const QPoint& pos );
    /// Context menu requested from master- or slave header at global position @p pos
    void headerContextMenuRequested( const QPoint &pos );
    /// Context menu requested from master header at global position @p pos
    void masterHeaderContextMenuRequested( const QPoint &pos );
    /// Context menu requested from slave header at global position @p pos
    void slaveHeaderContextMenuRequested( const QPoint &pos );

    void currentChanged ( const QModelIndex & current, const QModelIndex & previous );
    void selectionChanged( const QModelIndexList );

public slots:
    void edit( const QModelIndex &index );

protected slots:
    void slotSelectionChanged( const QItemSelection &sel, const QItemSelection & );
    void slotToRightView( const QModelIndex &index );
    void slotToLeftView( const QModelIndex &index );

    void slotRightHeaderContextMenuRequested( const QPoint &pos );
    void slotLeftHeaderContextMenuRequested( const QPoint &pos );

protected:
    void init();
    QList<int> expandColumnList( const QList<int> lst ) const;
    
protected:
    TreeViewBase *m_leftview;
    TreeViewBase *m_rightview;
    ItemModelBase *m_model;
    QItemSelectionModel *m_selectionmodel;
    bool m_arrowKeyNavigation;
    bool m_readWrite;
    bool m_mode;
    
    KAction *m_actionSplitView;

};

} // namespace KPlato

#endif
