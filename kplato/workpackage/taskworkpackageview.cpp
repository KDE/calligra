/* This file is part of the KDE project
  Copyright (C) 2007 Dag Andersen <danders@get2net.dk>

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

#include <KoDocument.h>

#include <QDragMoveEvent>
#include <QMenu>
#include <QModelIndex>
#include <QVBoxLayout>
#include <QWidget>
#include <QTextBrowser>
#include <QTextCursor>
#include <QTextTableFormat>
#include <QTextLength>
#include <QTextTable>
#include <QTextFrame>
#include <QTextFrameFormat>
#include <QTextCharFormat>
#include <QTextTableCell>
#include <QLineEdit>
#include <QItemSelection>

#include <kicon.h>
#include <kaction.h>
#include <kglobal.h>
#include <klocale.h>
#include <ktoggleaction.h>
#include <kactionmenu.h>
#include <kstandardaction.h>
#include <kstandardshortcut.h>
#include <kaccelgen.h>
#include <kactioncollection.h>


using namespace KPlato;

namespace KPlatoWork
{


TaskWorkPackageTreeView::TaskWorkPackageTreeView( Part *part, QWidget *parent )
    : DoubleTreeViewBase( parent )
{
    setContextMenuPolicy( Qt::CustomContextMenu );
    TaskWorkPackageModel *m = new TaskWorkPackageModel( part, this );
    setModel( m );
    //setSelectionBehavior( QAbstractItemView::SelectItems );
    setSelectionMode( QAbstractItemView::ExtendedSelection );
    setStretchLastSection( false );
    
    createItemDelegates( m );
    
    QList<int> lst1; lst1 << 1 << -1; // only display column 0 (NodeName) in left view
    masterView()->setDefaultColumns( QList<int>() << 0 );
    QList<int> show;
    show << TaskWorkPackageModel::NodeCompleted
            << TaskWorkPackageModel::NodeActualEffort
            << TaskWorkPackageModel::NodeRemainingEffort
            << TaskWorkPackageModel::NodePlannedEffort
            << TaskWorkPackageModel::NodeActualStart
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
}

TaskWorkPackageModel *TaskWorkPackageTreeView::model() const
{
    return static_cast<TaskWorkPackageModel*>( DoubleTreeViewBase::model() );
}

Project *TaskWorkPackageTreeView::project() const
{
    return model()->project();
}

QList<Node*> TaskWorkPackageTreeView::selectedNodes() const
{
    QList<Node*> lst;
    foreach( const QModelIndex &idx, selectionModel()->selectedIndexes() ) {
        Node *n = model()->nodeForIndex( idx );
        if ( n && ! lst.contains( n ) ) {
            lst << n;
        }
    }
    return lst;
}

void TaskWorkPackageTreeView::setProject( Project *project )
{
    model()->setProject( project );
}

void TaskWorkPackageTreeView::slotActivated( const QModelIndex index )
{
    kDebug()<<index.column();
}

void TaskWorkPackageTreeView::dragMoveEvent(QDragMoveEvent *event)
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
    kDebug()<<"-------------------- creating TaskWorkPackageView -------------------";
    QVBoxLayout * l = new QVBoxLayout( this );
    l->setMargin( 0 );
    m_view = new TaskWorkPackageTreeView( part, this );
    l->addWidget( m_view );
    setupGui();

    connect( model(), SIGNAL( executeCommand( QUndoCommand* ) ), part, SLOT( addCommand( QUndoCommand* ) ) );

    connect( m_view, SIGNAL( contextMenuRequested( const QModelIndex&, const QPoint& ) ), SLOT( slotContextMenuRequested( const QModelIndex&, const QPoint& ) ) );
    
    connect( m_view, SIGNAL( headerContextMenuRequested( const QPoint& ) ), SLOT( slotHeaderContextMenuRequested( const QPoint& ) ) );

    connect( m_view, SIGNAL( selectionChanged( const QModelIndexList ) ), SLOT( slotSelectionChanged( const QModelIndexList ) ) );
}

void TaskWorkPackageView::updateReadWrite( bool rw )
{
    m_view->setReadWrite( rw );
}

void TaskWorkPackageView::slotSelectionChanged( const QModelIndexList lst )
{
    emit selectionChanged();
}

QList<Node*> TaskWorkPackageView::selectedNodes() const
{
    return m_view->selectedNodes();
}

Node *TaskWorkPackageView::currentNode() const 
{
    return m_view->model()->nodeForIndex( m_view->selectionModel()->currentIndex() );
}

Document *TaskWorkPackageView::currentDocument() const 
{
    return m_view->model()->documentForIndex( m_view->selectionModel()->currentIndex() );
}


void TaskWorkPackageView::slotHeaderContextMenuRequested( const QPoint &pos )
{
    kDebug();
    QList<QAction*> lst = contextActionList();
    if ( ! lst.isEmpty() ) {
        QMenu::exec( lst, pos, lst.first() );
    }
}

void TaskWorkPackageView::slotContextMenuRequested( const QModelIndex &index, const QPoint& pos )
{
    kDebug()<<index<<pos;
    if ( ! index.isValid() ) {
        slotHeaderContextMenuRequested( pos );
        return;
    }
    Node *node = m_view->model()->nodeForIndex( index );
    if ( node ) {
        return slotContextMenuRequested( node, pos );
    }
    Document *doc = m_view->model()->documentForIndex( index );
    if ( doc ) {
        return slotContextMenuRequested( doc, pos );
    }
    return slotHeaderContextMenuRequested( pos );
}

void TaskWorkPackageView::slotContextMenuRequested( Node *node, const QPoint& pos )
{
    kDebug()<<node->name()<<" :"<<pos;
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
    kDebug()<<name;
    if ( name.isEmpty() ) {
        slotHeaderContextMenuRequested( pos );
        return;
    }
    emit requestPopupMenu( name, pos );
}

void TaskWorkPackageView::slotContextMenuRequested( Document *doc, const QPoint& pos )
{
    kDebug()<<doc->url()<<" :"<<pos;
    QString name;
    switch ( doc->type() ) {
        case Document::Type_Product:
            name = "editdocument_popup";
            break;
        default:
            name = "viewdocument_popup";
            break;
    }
    kDebug()<<name;
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
    
    actionOptions = new KAction(KIcon("configure"), i18n("Configure View..."), this);
    connect(actionOptions, SIGNAL(triggered(bool) ), SLOT(slotOptions()));
    addContextAction( actionOptions );
}

void TaskWorkPackageView::slotSplitView()
{
    kDebug();
    m_view->setViewSplitMode( ! m_view->isViewSplit() );
}


void TaskWorkPackageView::slotOptions()
{
    kDebug();
    SplitItemViewSettupDialog *dlg = new SplitItemViewSettupDialog( m_view, this );
    dlg->exec();
    delete dlg;
}

bool TaskWorkPackageView::loadContext( const KoXmlElement &context )
{
    kDebug();
//     m_view->setPeriod( context.attribute( "period", QString("%1").arg( m_view->defaultPeriod() ) ).toInt() );
//     
//     m_view->setPeriodType( context.attribute( "periodtype", QString("%1").arg( m_view->defaultPeriodType() ) ).toInt() );
//     
//     m_view->setWeekday( context.attribute( "weekday", QString("%1").arg( m_view->defaultWeekday() ) ).toInt() );
    return m_view->loadContext( model()->columnMap(), context );
}

void TaskWorkPackageView::saveContext( QDomElement &context ) const
{
    m_view->saveContext( model()->columnMap(), context );
}

KoPrintJob *TaskWorkPackageView::createPrintJob()
{
//    return m_view->createPrintJob( this );
    return 0;
}


} // namespace KPlatoWork

#include "taskworkpackageview.moc"
