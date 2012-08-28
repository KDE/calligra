/* This file is part of the KDE project
  Copyright (C) 2007 - 2009, 2012 Dag Andersen <danders@get2net.dk>

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

#include "taskworkpackageview.h"
#include "taskworkpackagemodel.h"

#include "part.h"
#include "kptglobal.h"
#include "kptcommand.h"
#include "kptproject.h"
#include "kptschedule.h"
#include "kpteffortcostmap.h"
#include "kptitemviewsettup.h"
#include "planworksettings.h"

#include <KoIcon.h>

#include <QDragMoveEvent>
#include <QMenu>
#include <QModelIndex>
#include <QWidget>
#include <QSortFilterProxyModel>
#include <QHeaderView>
#include <QPointer>

#include <kaction.h>
#include <kglobal.h>
#include <klocale.h>
#include <ktoggleaction.h>
#include <kactionmenu.h>
#include <kstandardaction.h>
#include <kstandardshortcut.h>
#include <kaccelgen.h>
#include <kactioncollection.h>

#include "debugarea.h"

using namespace KPlato;

namespace KPlatoWork
{


TaskWorkPackageTreeView::TaskWorkPackageTreeView( Part *part, QWidget *parent )
    : DoubleTreeViewBase( parent )
{
    setContextMenuPolicy( Qt::CustomContextMenu );
    masterView()->header()->setSortIndicatorShown( true );
    masterView()->header()->setClickable( true );
    slaveView()->header()->setSortIndicatorShown( true );
    slaveView()->header()->setClickable( true );

    QSortFilterProxyModel *sf = new QSortFilterProxyModel( this );
    TaskWorkPackageModel *m = new TaskWorkPackageModel( part, sf );
    sf->setSourceModel( m );
    setModel( sf );
    //setSelectionBehavior( QAbstractItemView::SelectItems );
    setSelectionMode( QAbstractItemView::SingleSelection );
    setStretchLastSection( false );

    createItemDelegates( m );

    QList<int> lst1; lst1 << 2 << -1; // display column 0 and 1 (NodeName and NodeType ) in left view
    masterView()->setDefaultColumns( QList<int>() << 0 << 1 );
    QList<int> show;
    show << TaskWorkPackageModel::NodeCompleted
            << TaskWorkPackageModel::NodeActualEffort
            << TaskWorkPackageModel::NodeRemainingEffort
            << TaskWorkPackageModel::NodePlannedEffort
            << TaskWorkPackageModel::NodeStartTime
            << TaskWorkPackageModel::NodeActualStart
            << TaskWorkPackageModel::NodeEndTime
            << TaskWorkPackageModel::NodeActualFinish
            << TaskWorkPackageModel::ProjectName
            << TaskWorkPackageModel::ProjectManager;

    QList<int> lst2;
    for ( int i = 0; i < m->columnCount(); ++i ) {
        if ( ! show.contains( i ) ) {
            lst2 << i;
        }
    }
    hideColumns( lst1, lst2 );
    slaveView()->setDefaultColumns( show );
    masterView()->setFocus();

    kDebug(planworkDbg())<<PlanWorkSettings::self()->taskWorkPackageView();
    masterView()->header()->setClickable( true );
    slaveView()->header()->setSortIndicatorShown( true );

    connect(masterView()->header(), SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)), SLOT(setSortOrder(int, Qt::SortOrder)));
    connect(slaveView()->header(), SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)), SLOT(setSortOrder(int, Qt::SortOrder)));

    masterView()->header()->setSortIndicator( TaskWorkPackageModel::NodeType, Qt::AscendingOrder );

    connect(masterView()->header(), SIGNAL(sectionMoved(int, int, int)), SIGNAL(sectionsMoved()));
    connect(slaveView()->header(), SIGNAL(sectionMoved(int, int, int)), SIGNAL(sectionsMoved()));
}

void TaskWorkPackageTreeView::setSortOrder( int col, Qt::SortOrder order )
{
    model()->sort( col, order );
}

TaskWorkPackageModel *TaskWorkPackageTreeView::itemModel() const
{
    return static_cast<TaskWorkPackageModel*>( static_cast<QSortFilterProxyModel*>( model() )->sourceModel() );
}

Project *TaskWorkPackageTreeView::project() const
{
    return itemModel()->project();
}

Document *TaskWorkPackageTreeView::currentDocument() const
{
    QSortFilterProxyModel *sf = qobject_cast<QSortFilterProxyModel*>( model() );
    Q_ASSERT( sf );
    if ( sf == 0 ) {
        return 0;
    }
    return itemModel()->documentForIndex( sf->mapToSource(  selectionModel()->currentIndex() ) );
}

Node *TaskWorkPackageTreeView::currentNode() const
{
    QSortFilterProxyModel *sf = qobject_cast<QSortFilterProxyModel*>( model() );
    Q_ASSERT( sf );
    if ( sf == 0 ) {
        return 0;
    }
    return itemModel()->nodeForIndex( sf->mapToSource(  selectionModel()->currentIndex() ) );
}

QList<Node*> TaskWorkPackageTreeView::selectedNodes() const
{
    QList<Node*> lst;
    QSortFilterProxyModel *sf = qobject_cast<QSortFilterProxyModel*>( model() );
    Q_ASSERT( sf );
    if ( sf == 0 ) {
        return lst;
    }
    foreach( const QModelIndex &idx, selectionModel()->selectedIndexes() ) {
        QModelIndex i = sf->mapToSource( idx );
        Q_ASSERT( i.isValid() && i.model() == itemModel() );
        Node *n = itemModel()->nodeForIndex( i );
        if ( n && ! lst.contains( n ) ) {
            lst << n;
        }
    }
    return lst;
}

void TaskWorkPackageTreeView::setProject( Project *project )
{
    itemModel()->setProject( project );
}

void TaskWorkPackageTreeView::slotActivated( const QModelIndex index )
{
    kDebug(planworkDbg())<<index.column();
}

void TaskWorkPackageTreeView::dragMoveEvent(QDragMoveEvent */*event*/)
{
/*    if (dragDropMode() == InternalMove
        && (event->source() != this || !(event->possibleActions() & Qt::MoveAction)))
        return;

    TreeViewBase::dragMoveEvent( event );
    if ( ! event->isAccepted() ) {
        return;
    }
    //QTreeView thinks it's ok to drop
    event->ignore();
    QModelIndex index = indexAt( event->pos() );
    if ( ! index.isValid() ) {
        event->accept();
        return; // always ok to drop on main project
    }
    Node *dn = model()->node( index );
    if ( dn == 0 ) {
        kError()<<"no node to drop on!"
        return; // hmmm
    }
    switch ( dropIndicatorPosition() ) {
        case AboveItem:
        case BelowItem:
            //dn == sibling
            if ( model()->dropAllowed( dn->parentNode(), event->mimeData() ) ) {
                event->accept();
            }
            break;
        case OnItem:
            //dn == new parent
            if ( model()->dropAllowed( dn, event->mimeData() ) ) {
                event->accept();
            }
            break;
        default:
            break;
    }*/
}


//-----------------------------------
TaskWorkPackageView::TaskWorkPackageView( Part *part, QWidget *parent )
    : QWidget( parent )
{
    kDebug(planworkDbg())<<"-------------------- creating TaskWorkPackageView -------------------";
    QVBoxLayout * l = new QVBoxLayout( this );
    l->setMargin( 0 );
    m_view = new TaskWorkPackageTreeView( part, this );
    l->addWidget( m_view );
    setupGui();

    connect( itemModel(), SIGNAL( executeCommand( KUndo2Command* ) ), part, SLOT( addCommand( KUndo2Command* ) ) );

    connect( m_view, SIGNAL( contextMenuRequested( const QModelIndex&, const QPoint& ) ), SLOT( slotContextMenuRequested( const QModelIndex&, const QPoint& ) ) );

    connect( m_view, SIGNAL( headerContextMenuRequested( const QPoint& ) ), SLOT( slotHeaderContextMenuRequested( const QPoint& ) ) );

    connect( m_view, SIGNAL( selectionChanged( const QModelIndexList ) ), SLOT( slotSelectionChanged( const QModelIndexList ) ) );

    loadContext();

    connect(m_view, SIGNAL(sectionsMoved()), SLOT(sectionsMoved()));
}

void TaskWorkPackageView::updateReadWrite( bool rw )
{
    m_view->setReadWrite( rw );
}

void TaskWorkPackageView::slotSelectionChanged( const QModelIndexList /*lst*/ )
{
    emit selectionChanged();
}

QList<Node*> TaskWorkPackageView::selectedNodes() const
{
    return m_view->selectedNodes();
}

Node *TaskWorkPackageView::currentNode() const
{
    return m_view->currentNode();
}

Document *TaskWorkPackageView::currentDocument() const
{
    return m_view->currentDocument();
}


void TaskWorkPackageView::slotHeaderContextMenuRequested( const QPoint &pos )
{
    kDebug(planworkDbg());
    QList<QAction*> lst = contextActionList();
    if ( ! lst.isEmpty() ) {
        QMenu::exec( lst, pos, lst.first() );
    }
}

void TaskWorkPackageView::slotContextMenuRequested( const QModelIndex &index, const QPoint& pos )
{
    kDebug(planworkDbg())<<index<<pos;
    if ( ! index.isValid() ) {
        slotHeaderContextMenuRequested( pos );
        return;
    }
    QSortFilterProxyModel *sf = qobject_cast<QSortFilterProxyModel*>( m_view->model() );
    Q_ASSERT( sf );
    if ( sf == 0 ) {
        return;
    }
    QModelIndex idx = sf->mapToSource( index );
    if ( ! idx.isValid() ) {
        slotHeaderContextMenuRequested( pos );
        return;
    }

    Node *node = itemModel()->nodeForIndex( idx );
    if ( node ) {
        return slotContextMenuRequested( node, pos );
    }
    Document *doc = itemModel()->documentForIndex( idx );
    if ( doc ) {
        return slotContextMenuRequested( doc, pos );
    }
    return slotHeaderContextMenuRequested( pos );
}

void TaskWorkPackageView::slotContextMenuRequested( Node *node, const QPoint& pos )
{
    kDebug(planworkDbg())<<node->name()<<" :"<<pos;
    QString name;
    switch ( node->type() ) {
        case Node::Type_Task:
            name = "taskstatus_popup";
            break;
        case Node::Type_Milestone:
            name = "taskview_milestone_popup";
            break;
        case Node::Type_Summarytask:
            name = "taskview_summary_popup";
            break;
        default:
            break;
    }
    kDebug(planworkDbg())<<name;
    if ( name.isEmpty() ) {
        slotHeaderContextMenuRequested( pos );
        return;
    }
    emit requestPopupMenu( name, pos );
}

void TaskWorkPackageView::slotContextMenuRequested( Document *doc, const QPoint& pos )
{
    kDebug(planworkDbg())<<doc->url()<<" :"<<pos;
    QString name;
    switch ( doc->type() ) {
        case Document::Type_Product:
            name = "editdocument_popup";
            break;
        default:
            name = "viewdocument_popup";
            break;
    }
    kDebug(planworkDbg())<<name;
    if ( name.isEmpty() ) {
        slotHeaderContextMenuRequested( pos );
        return;
    }
    emit requestPopupMenu( name, pos );
}

void TaskWorkPackageView::setupGui()
{
    // Add the context menu actions for the view options
    connect(m_view->actionSplitView(), SIGNAL(triggered(bool) ), SLOT(slotSplitView()));
    addContextAction( m_view->actionSplitView() );

    actionOptions = new KAction(koIcon("configure"), i18n("Configure View..."), this);
    connect(actionOptions, SIGNAL(triggered(bool) ), SLOT(slotOptions()));
    addContextAction( actionOptions );
}

void TaskWorkPackageView::slotSplitView()
{
    kDebug(planworkDbg());
    m_view->setViewSplitMode( ! m_view->isViewSplit() );
    saveContext();
}


void TaskWorkPackageView::slotOptions()
{
    kDebug(planworkDbg());
    QPointer<SplitItemViewSettupDialog> dlg = new SplitItemViewSettupDialog( 0, m_view, this );
    dlg->exec();
    delete dlg;
    saveContext();
}

void TaskWorkPackageView::sectionsMoved()
{
    saveContext();
}

bool TaskWorkPackageView::loadContext()
{
    KoXmlDocument doc;
    doc.setContent( PlanWorkSettings::self()->taskWorkPackageView() );
    KoXmlElement context = doc.namedItem( "TaskWorkPackageViewSettings" ).toElement();
    if ( context.isNull() ) {
        kDebug(planworkDbg())<<"No settings";
        return false;
    }
    return m_view->loadContext( itemModel()->columnMap(), context );
}

void TaskWorkPackageView::saveContext()
{
    QDomDocument doc ( "TaskWorkPackageView" );
    QDomElement context = doc.createElement( "TaskWorkPackageViewSettings" );
    doc.appendChild( context );
    m_view->saveContext( itemModel()->columnMap(), context );
    PlanWorkSettings::self()->setTaskWorkPackageView( doc.toString() );
    PlanWorkSettings::self()->writeConfig();
    kDebug(planworkDbg())<<endl<<doc.toString();
}

KoPrintJob *TaskWorkPackageView::createPrintJob()
{
//    return m_view->createPrintJob( this );
    return 0;
}


} // namespace KPlatoWork

#include "taskworkpackageview.moc"
