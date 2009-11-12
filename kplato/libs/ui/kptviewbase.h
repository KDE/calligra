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

#include "ui_kptprintingheaderfooter.h"


#include <KoView.h>
#include <KoPrintingDialog.h>

#include <QMap>
#include <QTreeView>
#include <QSplitter>
#include <QList>
#include <QPointer>

class KAction;

class QWidget;
class QMetaEnum;
class QAbstractItemModel;

class KoDocument;
class KoPrintJob;

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

class ViewBase;
class TreeViewBase;
class DoubleTreeViewBase;


//------------------
class KPLATOUI_EXPORT PrintingOptions
{
public:
    PrintingOptions() {
        headerOptions.group = true;
        headerOptions.project = Qt::Checked;
        headerOptions.date = Qt::Checked;
        headerOptions.manager = Qt::Checked;
        headerOptions.page = Qt::Checked;
        
        footerOptions.group = false;
        footerOptions.project = Qt::Checked;
        footerOptions.date = Qt::Checked;
        footerOptions.manager = Qt::Checked;
        footerOptions.page = Qt::Checked;
    }
    ~PrintingOptions() {}

    struct Data {
        bool group;
        Qt::CheckState project;
        Qt::CheckState date;
        Qt::CheckState manager;
        Qt::CheckState page;
    };
    struct Data headerOptions;
    struct Data footerOptions;
};

//------------------
class KPLATOUI_EXPORT PrintingHeaderFooter : public QWidget, public Ui::PrintingHeaderFooter
{
    Q_OBJECT
public:
    explicit PrintingHeaderFooter( const PrintingOptions &opt, QWidget *parent = 0 );
    ~PrintingHeaderFooter();
    
    void setOptions( const PrintingOptions &options );
    PrintingOptions options() const;

private:
    PrintingOptions m_options;
};

//------------------
class KPLATOUI_EXPORT PrintingDialog : public KoPrintingDialog
{
    Q_OBJECT
public:
    PrintingDialog(ViewBase *view);
    ~PrintingDialog() {}

    virtual QList<QWidget*> createOptionWidgets() const;
    virtual QList<KoShape*> shapesOnPage(int);

    QRect headerRect() const;
    QRect footerRect() const;
    void paintHeaderFooter( QPainter &p, const PrintingOptions &options, int pageNumber, const Project &project );
    
    PrintingOptions printingOptions() const;
    void setPrintingOptions( const PrintingOptions &opt);
    
signals:
    void changed ( const PrintingOptions &opt );
    
public slots:
    virtual void startPrinting(RemovePolicy removePolicy = DoNotDelete);

protected:
    virtual void paint( QPainter &p, const PrintingOptions::Data &options, const QRect &rect,  int pageNumber, const Project &project );
    int headerFooterHeight( const PrintingOptions::Data &options ) const;
    void drawBottomRect( QPainter &p, const QRect &r );

protected:
    ViewBase *m_view;
    QPointer<PrintingHeaderFooter> m_widget;
};

class KPLATOUI_EXPORT ViewActionLists
{
public:
    ViewActionLists() : actionOptions( 0 ) {}
    virtual ~ViewActionLists() {}

    /// Returns the list of action lists that shall be plugged/unplugged
    virtual QStringList actionListNames() const { return m_actionListMap.keys(); }
    /// Returns the list of actions associated with the action list name
    virtual QList<QAction*> actionList( const QString name ) const { return m_actionListMap[name]; }
    /// Add an action to the specified action list
    void addAction( const QString list, QAction *action ) { m_actionListMap[list].append( action ); }

    virtual QList<QAction*> viewlistActionList() const { return m_viewlistActionList; }
    void addViewlistAction( QAction *action ) { m_viewlistActionList.append( action ); }
    
    QList<QAction*> contextActionList() const { return m_contextActionList; }
    void addContextAction( QAction *action ) { m_contextActionList.append( action ); }

protected:
    /// List of all menu/toolbar actions (used for plug/unplug)
    QMap<QString, QList<QAction*> > m_actionListMap;

    /// List of actions that will be shown in the viewlist context menu
    QList<QAction*> m_viewlistActionList;
    
    /// List of actions that will be shown in the views header context menu
    QList<QAction*> m_contextActionList;
    
    // View options context menu
    KAction *actionOptions;
};

/** 
 ViewBase is the baseclass of all sub-views to View.

*/
class KPLATOUI_EXPORT ViewBase : public KoView, public ViewActionLists
{
    Q_OBJECT
public:
    /// Contructor
    ViewBase(KoDocument *doc, QWidget *parent);
    /// Destructor
    virtual ~ViewBase();
    /// Return the part (document) this view handles
    KoDocument *part() const;
    
    /// Return the type of view this is (class name)
    QString viewType() const { return metaObject()->className(); }
    
    /// Returns true if this view or any child widget has focus
    bool isActive() const;
    
    /// Set the project this view shall handle.
    virtual void setProject( Project * /*project*/ ) {}
    /// Return the project
    virtual Project *project() const { return 0; }
    /// Draw data from current part / project
    virtual void draw() {}
    /// Draw data from project.
    virtual void draw(Project &/*project*/) {}
    /// Draw changed data from project.
    virtual void drawChanges(Project &project) { draw(project); }
    /// Set readWrite mode
    virtual void updateReadWrite( bool );
    bool isReadWrite() const { return m_readWrite; }

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
    /// Reimplement if your view handles zoom
    virtual KoZoomController *zoomController() const { return 0; }

    /// Loads context info into this view. Reimplement.
    virtual bool loadContext( const KoXmlElement &/*context*/ ) { return false; }
    /// Save context info from this view. Reimplement.
    virtual void saveContext( QDomElement &/*context*/ ) const {}
    
    virtual ViewBase *hitView( const QPoint &pos );

    virtual KoPrintJob *createPrintJob();
    PrintingOptions printingOptions() const { return m_printingOptions; }
    void setPrintingOptions( const PrintingOptions &opt ) { m_printingOptions = opt; }
    
    void addAction( const QString list, QAction *action ) { ViewActionLists::addAction( list, action );  }

public slots:
    /// Activate/deactivate the gui
    virtual void setGuiActive( bool activate );

    void slotUpdateReadWrite( bool );
    virtual void slotHeaderContextMenuRequested( const QPoint &pos );

    virtual void slotEditCopy() {}
    virtual void slotEditCut() {}
    virtual void slotEditPaste() {}
    virtual void slotRefreshView() {}
    
signals:
    /// Emitted when the gui has been activated or deactivated
    void guiActivated( ViewBase*, bool );
    /// Request for a context menu popup
    void requestPopupMenu( const QString&, const QPoint & );
    
protected slots:
    virtual void slotOptions() {}

protected:
    void createOptionAction();
    
    bool m_readWrite;
    PrintingOptions m_printingOptions;
};

//------------------
class KPLATOUI_EXPORT TreeViewPrintingDialog : public PrintingDialog
{
    Q_OBJECT
public:
    TreeViewPrintingDialog( ViewBase *view, TreeViewBase *treeview, Project *project = 0 );
    ~TreeViewPrintingDialog() {}

    virtual int documentFirstPage() const { return 1; }
    virtual int documentLastPage() const;

protected:
    virtual void printPage( int pageNumber, QPainter &painter );

    int firstRow( int page ) const;

private:
    TreeViewBase *m_tree;
    Project *m_project;
    int m_firstRow;
};

//-----------------
class KPLATOUI_EXPORT TreeViewBase : public QTreeView
{
    Q_OBJECT
public:
    explicit TreeViewBase( QWidget *parent = 0 );

    void setReadWrite( bool rw );
    virtual void createItemDelegates( ItemModelBase *model );
    void setArrowKeyNavigation( bool on ) { m_arrowKeyNavigation = on; }
    bool arrowKeyNavigation() const { return m_arrowKeyNavigation; }

    /// Move move to first visual
    QModelIndex firstColumn( int row, const QModelIndex &parent );
    /// Move move to last visual
    QModelIndex lastColumn( int row, const QModelIndex &parent );
    /// Move from @p current to next item
    QModelIndex nextColumn( const QModelIndex &current );
    /// Move from @p current to next item
    QModelIndex previousColumn( const QModelIndex &current );
    /// Move to first editable index in @p row with @p parent
    QModelIndex firstEditable( int row, const QModelIndex &parent );
    /// Move to last editable index in @p row with @p parent
    QModelIndex lastEditable( int row, const QModelIndex &parent );

    void setAcceptDropsOnView( bool mode ) { m_acceptDropsOnView = mode; }

    virtual void setModel( QAbstractItemModel *model );

    virtual void setSelectionModel( QItemSelectionModel *model );

    void setStretchLastSection( bool );

    void mapToSection( int column, int section );
    int section( int col ) const;

    void setColumnsHidden( const QList<int> &list );

    /// Loads context info into this view. Reimplement.
    virtual bool loadContext( const QMetaEnum &map, const KoXmlElement &element );
    /// Save context info from this view. Reimplement.
    virtual void saveContext( const QMetaEnum &map, QDomElement &context ) const;

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
    
    void setDefaultColumns( const QList<int> lst ) { m_defaultColumns = lst; }
    QList<int> defaultColumns() const { return m_defaultColumns; }
    
    KoPrintJob *createPrintJob( ViewBase *parent );
    
    QModelIndex firstVisibleIndex( const QModelIndex &idx ) const;

signals:
    /// Context menu requested from viewport at global position @p pos
    void contextMenuRequested( QModelIndex, const QPoint &pos );
    /// Context menu requested from header at global position @p pos
    void headerContextMenuRequested( const QPoint &pos );

    void moveAfterLastColumn( const QModelIndex & );
    void moveBeforeFirstColumn( const QModelIndex & );
    void editAfterLastColumn( const QModelIndex & );
    void editBeforeFirstColumn( const QModelIndex & );

    void dropAllowed( const QModelIndex &index, int dropIndicatorPosition, QDragMoveEvent *event );

protected:
    void keyPressEvent(QKeyEvent *event);
    /**
      Reimplemented from QTreeView to make tab/backtab in editor work reasonably well.
      Move the cursor in the way described by \a cursorAction, *not* using the
      information provided by the button \a modifiers.
    */
    QModelIndex moveCursor( CursorAction cursorAction, Qt::KeyboardModifiers modifiers );
    /// Move cursor from @p index in direction @p cursorAction. @p modifiers is not used.
    QModelIndex moveCursor(  const QModelIndex &index, CursorAction cursorAction, Qt::KeyboardModifiers = Qt::NoModifier );
    /// Move from @p index to next editable item, in direction @p cursorAction.
    QModelIndex moveToEditable( const QModelIndex &index, CursorAction cursorAction );
    
    void contextMenuEvent ( QContextMenuEvent * event );

    void dragMoveEvent(QDragMoveEvent *event);

    void updateSelection( const QModelIndex &oldidx, const QModelIndex &newidx, QKeyEvent *event );

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
    QList<int> m_defaultColumns;
};

//------------------
class KPLATOUI_EXPORT DoubleTreeViewPrintingDialog : public PrintingDialog
{
    Q_OBJECT
public:
    DoubleTreeViewPrintingDialog( ViewBase *view, DoubleTreeViewBase *treeview, Project *project );
    ~DoubleTreeViewPrintingDialog() {}

    virtual int documentFirstPage() const { return 1; }
    virtual int documentLastPage() const;

protected:
    virtual void printPage( int pageNumber, QPainter &painter );
    
    int firstRow( int page ) const;
    
private:
    DoubleTreeViewBase *m_tree;
    Project *m_project;
    int m_firstRow;
};

class KPLATOUI_EXPORT DoubleTreeViewBase : public QSplitter
{
    Q_OBJECT
public:
    explicit DoubleTreeViewBase( QWidget *parent );
    DoubleTreeViewBase( bool mode, QWidget *parent );
    ~DoubleTreeViewBase();

    void setReadWrite( bool rw );

    void setModel( QAbstractItemModel *model );
    QAbstractItemModel *model() const;

    void setArrowKeyNavigation( bool on ) { m_arrowKeyNavigation = on; }
    bool arrowKeyNavigation() const { return m_arrowKeyNavigation; }

    QItemSelectionModel *selectionModel() const { return m_selectionmodel; }
    void setSelectionModel( QItemSelectionModel *model );
    void setSelectionMode( QAbstractItemView::SelectionMode mode );
    void setSelectionBehavior( QAbstractItemView::SelectionBehavior mode );
    virtual void createItemDelegates( ItemModelBase *model );
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
    virtual bool loadContext( const QMetaEnum &map, const KoXmlElement &element );
    /// Save context info from this view. Reimplement.
    virtual void saveContext( const QMetaEnum &map, QDomElement &context ) const;
    
    void setViewSplitMode( bool split );
    bool isViewSplit() const { return m_mode; }
    KAction *actionSplitView() const { return m_actionSplitView; }
    
    void setRootIsDecorated ( bool show );

    KoPrintJob *createPrintJob( ViewBase *parent );

    void setStretchFactors();
    
    QModelIndex indexAt( const QPoint &pos ) const;

    void setParentsExpanded( const QModelIndex &idx, bool expanded );

    void setSortingEnabled( bool on ) {
        m_leftview->setSortingEnabled( on );
        m_rightview->setSortingEnabled( on );
    }
    void sortByColumn( int col, Qt::SortOrder order = Qt::AscendingOrder ) {
        if ( ! m_leftview->isColumnHidden( col ) || 
             ! m_rightview->isVisible() ||
             m_rightview->isColumnHidden( col ) )
        {
            m_leftview->sortByColumn( col, order );
        } else {
            m_rightview->sortByColumn( col, order );
        }
    }
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

    void dropAllowed( const QModelIndex &index, int dropIndicatorPosition, QDragMoveEvent *event );

public slots:
    void edit( const QModelIndex &index );

protected slots:
    void slotSelectionChanged( const QItemSelection &sel, const QItemSelection & );
    void slotToRightView( const QModelIndex &index );
    void slotToLeftView( const QModelIndex &index );
    void slotEditToRightView( const QModelIndex &index );
    void slotEditToLeftView( const QModelIndex &index );

    void slotRightHeaderContextMenuRequested( const QPoint &pos );
    void slotLeftHeaderContextMenuRequested( const QPoint &pos );

    void slotLeftSortIndicatorChanged( int logicalIndex, Qt::SortOrder order );
    void slotRightSortIndicatorChanged( int logicalIndex, Qt::SortOrder order );

protected:
    void init();
    QList<int> expandColumnList( const QList<int> lst ) const;
    
protected:
    TreeViewBase *m_leftview;
    TreeViewBase *m_rightview;
    QItemSelectionModel *m_selectionmodel;
    bool m_arrowKeyNavigation;
    bool m_readWrite;
    bool m_mode;
    
    KAction *m_actionSplitView;

};

} // namespace KPlato

#endif
