/* This file is part of the KDE project
  Copyright (C) 2006-2007 Dag Andersen kplato@kde.org>

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

#include "kptscheduleeditor.h"

#include "kptcommand.h"
#include "kptcalendar.h"
#include "kptduration.h"
#include "kptnode.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptschedule.h"
#include "kptdatetime.h"
#include "kptpertresult.h"
#include "kptitemviewsettup.h"
#include "kptrecalculatedialog.h"

#include <KoDocument.h>

#include <QMenu>
#include <QList>
#include <QObject>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QApplication>
#include <QClipboard>

#include <kaction.h>
#include <kicon.h>
#include <kglobal.h>
#include <klocale.h>
#include <kxmlguifactory.h>
#include <kactioncollection.h>
#include <QKeyEvent>

#include <kdebug.h>

namespace KPlato
{

ScheduleTreeView::ScheduleTreeView( QWidget *parent )
    : TreeViewBase( parent )
{
    header()->setStretchLastSection ( false );
    
    ScheduleItemModel *m = new ScheduleItemModel( this );
    setModel( m );
    //setSelectionBehavior( QAbstractItemView::SelectItems );
    setSelectionMode( QAbstractItemView::ExtendedSelection );
    setSelectionBehavior( QAbstractItemView::SelectRows );
    
    createItemDelegates( m );
}

void ScheduleTreeView::selectionChanged( const QItemSelection &sel, const QItemSelection &desel )
{
    //kDebug()<<sel.indexes().count();
    foreach( const QModelIndex &i, selectionModel()->selectedIndexes() ) {
        //kDebug()<<i.row()<<","<<i.column();
    }
    QTreeView::selectionChanged( sel, desel );
    emit selectionChanged( selectionModel()->selectedIndexes() );
}

void ScheduleTreeView::currentChanged( const QModelIndex & current, const QModelIndex & previous )
{
    //kDebug()<<current.row()<<","<<current.column();
    QTreeView::currentChanged( current, previous );
    emit currentChanged( current );
    selectionModel()->select( current, QItemSelectionModel::Rows | QItemSelectionModel::ClearAndSelect );
}

ScheduleManager *ScheduleTreeView::currentManager() const
{
    return model()->manager( currentIndex() );
}

//-----------------------------------
ScheduleEditor::ScheduleEditor( KoDocument *part, QWidget *parent )
    : ViewBase( part, parent )
{
    setupGui();
    slotEnableActions( 0 );
    
    QVBoxLayout * l = new QVBoxLayout( this );
    l->setMargin( 0 );
    m_view = new ScheduleTreeView( this );
    l->addWidget( m_view );
    m_view->setEditTriggers( m_view->editTriggers() | QAbstractItemView::EditKeyPressed );

    QList<int> show;
    show << ScheduleModel::ScheduleName
        << ScheduleModel::ScheduleState
        << ScheduleModel::ScheduleDirection
        << ScheduleModel::ScheduleOverbooking
        << ScheduleModel::ScheduleDistribution
        << ScheduleModel::SchedulePlannedStart
        << ScheduleModel::SchedulePlannedFinish
        //<< ScheduleModel::ScheduleScheduler
        ;

    QList<int> lst;
    for ( int c = 0; c < model()->columnCount(); ++c ) {
        if ( ! show.contains( c ) ) {
            lst << c;
        }
    }
    m_view->setColumnsHidden( lst );
    m_view->setDefaultColumns( show );
    

    connect( model(), SIGNAL( executeCommand( QUndoCommand* ) ), part, SLOT( addCommand( QUndoCommand* ) ) );

    connect( m_view, SIGNAL( currentChanged( QModelIndex ) ), this, SLOT( slotCurrentChanged( QModelIndex ) ) );

    connect( m_view, SIGNAL( selectionChanged( const QModelIndexList ) ), this, SLOT( slotSelectionChanged( const QModelIndexList ) ) );
    
    connect( model(), SIGNAL( dataChanged( const QModelIndex&, const QModelIndex& ) ), this, SLOT( updateActionsEnabled( const QModelIndex& ) ) );

    connect( m_view, SIGNAL( contextMenuRequested( QModelIndex, const QPoint& ) ), this, SLOT( slotContextMenuRequested( QModelIndex, const QPoint& ) ) );
    
    connect( m_view, SIGNAL( headerContextMenuRequested( const QPoint& ) ), SLOT( slotHeaderContextMenuRequested( const QPoint& ) ) );
}

void ScheduleEditor::draw( Project &project )
{
    m_view->setProject( &project );
}

void ScheduleEditor::draw()
{
}

void ScheduleEditor::setGuiActive( bool activate )
{
    //kDebug()<<activate;
    ViewBase::setGuiActive( activate );
    if ( activate && !m_view->currentIndex().isValid() ) {
        m_view->selectionModel()->setCurrentIndex(m_view->model()->index( 0, 0 ), QItemSelectionModel::NoUpdate);
    }
}

void ScheduleEditor::slotContextMenuRequested( QModelIndex index, const QPoint& pos )
{
    kDebug()<<index.row()<<","<<index.column()<<":"<<pos;
    QString name;
    if ( name.isEmpty() ) {
        slotHeaderContextMenuRequested( pos );
        return;
    }
    kDebug()<<name;
    emit requestPopupMenu( name, pos );
}

void ScheduleEditor::slotCurrentChanged(  const QModelIndex & )
{
    //kDebug()<<curr.row()<<","<<curr.column();
}

void ScheduleEditor::slotSelectionChanged( const QModelIndexList list)
{
    //kDebug()<<list.count();
    // The list has one entry per column, and we only select one row at a time, so...
    ScheduleManager *sm = 0;
    if ( ! list.isEmpty() ) {
        sm = m_view->model()->manager( list.first() );
        emit scheduleSelectionChanged( sm );
    } else {
        emit scheduleSelectionChanged( 0 );
    }
    slotEnableActions( sm );
    
}

void ScheduleEditor::updateActionsEnabled( const QModelIndex &index )
{
    kDebug()<<index;
    slotEnableActions( m_view->currentManager() );
}

void ScheduleEditor::slotEnableActions( const ScheduleManager *sm )
{
    actionAddSchedule->setEnabled( isReadWrite() );

    bool on = isReadWrite() && sm != 0;
    actionAddSubSchedule->setEnabled( on );
    
    actionBaselineSchedule->setIcon( KIcon( ( sm && sm->isBaselined() ? "view-time-schedule-baselined-remove" : "view-time-schedule-baselined-add" ) ) );
    if ( on && ( sm->isBaselined() || sm->isChildBaselined() ) ) {
        actionBaselineSchedule->setEnabled( sm->isBaselined() );
        actionCalculateSchedule->setEnabled( false );
        actionDeleteSelection->setEnabled( false );
    } else {
        actionBaselineSchedule->setEnabled( on && sm->isScheduled() && ! m_view->project()->isBaselined() );
        actionCalculateSchedule->setEnabled( on );
        actionDeleteSelection->setEnabled( on );
    }
}

void ScheduleEditor::setupGui()
{
    QString name = "scheduleeditor_edit_list";
    
    actionAddSchedule  = new KAction(KIcon( "view-time-schedule-insert" ), i18n("Add Schedule"), this);
    actionAddSchedule->setShortcut( KShortcut( Qt::CTRL + Qt::Key_I ) );
    actionCollection()->addAction("add_schedule", actionAddSchedule );
    connect( actionAddSchedule, SIGNAL( triggered( bool ) ), SLOT( slotAddSchedule() ) );
    addAction( name, actionAddSchedule );
    
    actionAddSubSchedule  = new KAction(KIcon( "view-time-schedule-child-insert" ), i18n("Add Sub-schedule"), this);
    actionAddSubSchedule->setShortcut( KShortcut( Qt::CTRL + Qt::SHIFT + Qt::Key_I ) );
    actionCollection()->addAction("add_subschedule", actionAddSubSchedule );
    connect( actionAddSubSchedule, SIGNAL( triggered( bool ) ), SLOT( slotAddSubSchedule() ) );
    addAction( name, actionAddSubSchedule );
    
    actionDeleteSelection  = new KAction(KIcon( "edit-delete" ), i18n("Delete Selection"), this);
    actionDeleteSelection->setShortcut( KShortcut( Qt::Key_Delete ) );
    actionCollection()->addAction("schedule_delete_selection", actionDeleteSelection );
    connect( actionDeleteSelection, SIGNAL( triggered( bool ) ), SLOT( slotDeleteSelection() ) );
    addAction( name, actionDeleteSelection );

    actionCalculateSchedule  = new KAction(KIcon( "view-time-schedule-calculus" ), i18n("Calculate"), this);
//    actionCalculateSchedule->setShortcut( KShortcut( Qt::CTRL + Qt::Key_C ) );
    actionCollection()->addAction("calculate_schedule", actionCalculateSchedule );
    connect( actionCalculateSchedule, SIGNAL( triggered( bool ) ), SLOT( slotCalculateSchedule() ) );
    addAction( name, actionCalculateSchedule );
    
    actionBaselineSchedule  = new KAction(KIcon( "view-time-schedule-baselined-add" ), i18n("Baseline"), this);
//    actionBaselineSchedule->setShortcut( KShortcut( Qt::CTRL + Qt::Key_B ) );
    actionCollection()->addAction("schedule_baseline", actionBaselineSchedule );
    connect( actionBaselineSchedule, SIGNAL( triggered( bool ) ), SLOT( slotBaselineSchedule() ) );
    addAction( name, actionBaselineSchedule );


    // Add the context menu actions for the view options
    createOptionAction();
}

void ScheduleEditor::updateReadWrite( bool readwrite )
{
    kDebug()<<readwrite;
    ViewBase::updateReadWrite( readwrite );
    m_view->setReadWrite( readwrite );
    slotEnableActions( m_view->currentManager() );
}

void ScheduleEditor::slotOptions()
{
    kDebug();
    ItemViewSettupDialog *dlg = new ItemViewSettupDialog( m_view, true, this );
    dlg->exec();
    delete dlg;

}

void ScheduleEditor::slotCalculateSchedule()
{
    //kDebug();
    ScheduleManager *sm = m_view->currentManager();
    if ( sm == 0 ) {
        return;
    }
    if ( sm->parentManager() ) {
        RecalculateDialog dlg;
        if ( dlg.exec() == QDialog::Rejected ) {
            return;
        }
        sm->setRecalculate( true );
        sm->setRecalculateFrom( DateTime( KDateTime( dlg.dateTime() ) ) );
    }
    emit calculateSchedule( m_view->project(), sm );
}

void ScheduleEditor::slotAddSchedule()
{
    //kDebug();
    ScheduleManager *sm = m_view->currentManager();
    if ( sm && sm->parentManager() ) {
        sm = sm->parentManager();
        ScheduleManager *m = m_view->project()->createScheduleManager( sm->name() + QString(".%1").arg( sm->children().count() + 1 ) );
        part()->addCommand( new AddScheduleManagerCmd( sm, m, i18n( "Create sub-schedule" ) ) );
        m_view->expand( model()->index( sm ) );
        QModelIndex idx = model()->index( m );
        if ( idx.isValid() ) {
            m_view->selectionModel()->select( idx, QItemSelectionModel::Rows | QItemSelectionModel::ClearAndSelect );
            m_view->selectionModel()->setCurrentIndex( idx, QItemSelectionModel::NoUpdate );
        }
    } else {
        Project *p = m_view->project();
        ScheduleManager *m = p->createScheduleManager();
        AddScheduleManagerCmd *cmd =  new AddScheduleManagerCmd( *p, m, i18n( "Add schedule %1", m->name() ) );
        part() ->addCommand( cmd );
        m_view->expand( model()->index( m ) );
        QModelIndex idx = model()->index( m );
        if ( idx.isValid() ) {
            m_view->selectionModel()->select( idx, QItemSelectionModel::Rows | QItemSelectionModel::ClearAndSelect );
            m_view->selectionModel()->setCurrentIndex( idx, QItemSelectionModel::NoUpdate );
        }
    }
}

void ScheduleEditor::slotAddSubSchedule()
{
    //kDebug();
    ScheduleManager *sm = m_view->currentManager();
    if ( sm ) {
        ScheduleManager *m = m_view->project()->createScheduleManager( sm->name() + QString(".%1").arg( sm->children().count() + 1 ) );
        part()->addCommand( new AddScheduleManagerCmd( sm, m, i18n( "Create sub-schedule" ) ) );
        m_view->expand( model()->index( sm ) );
        QModelIndex idx = model()->index( m );
        if ( idx.isValid() ) {
            m_view->selectionModel()->select( idx, QItemSelectionModel::Rows | QItemSelectionModel::ClearAndSelect );
            m_view->selectionModel()->setCurrentIndex( idx, QItemSelectionModel::NoUpdate );
        }
    } else {
        slotAddSchedule();
    }
}

void ScheduleEditor::slotBaselineSchedule()
{
    //kDebug();
    ScheduleManager *sm = m_view->currentManager();
    if ( sm ) {
        emit baselineSchedule( m_view->project(), sm );
    }
}

void ScheduleEditor::slotDeleteSelection()
{
    //kDebug();
    ScheduleManager *sm = m_view->currentManager();
    if ( sm ) {
        emit deleteScheduleManager( m_view->project(), sm );
    }
}

bool ScheduleEditor::loadContext( const KoXmlElement &context )
{
    kDebug();
    return m_view->loadContext( model()->columnMap(), context );
}

void ScheduleEditor::saveContext( QDomElement &context ) const
{
    m_view->saveContext( model()->columnMap(), context );
}

KoPrintJob *ScheduleEditor::createPrintJob()
{
    return m_view->createPrintJob( this );
}

//-----------------------------------------
ScheduleLogTreeView::ScheduleLogTreeView( QWidget *parent )
    : QTreeView( parent )
{
    header()->setStretchLastSection ( true );
    header()->setContextMenuPolicy( Qt::CustomContextMenu );
    
    m_model = new QSortFilterProxyModel( this );
    m_model->setFilterRole( Qt::UserRole+1 );
    m_model->setFilterKeyColumn ( 2 ); // severity
    m_model->setFilterWildcard( "[^0]" ); // Filter out Debug

    m_model->setSourceModel( new ScheduleLogItemModel( this ) );
    setModel( m_model );
    
    setRootIsDecorated( false );
    setSelectionMode( QAbstractItemView::ExtendedSelection );
    setSelectionBehavior( QAbstractItemView::SelectRows );
    
    connect( header(), SIGNAL( customContextMenuRequested ( const QPoint& ) ), this, SLOT( headerContextMenuRequested( const QPoint& ) ) );
}

void ScheduleLogTreeView::setFilterWildcard( const QString &filter )
{
    m_model->setFilterWildcard( filter );
}

QRegExp ScheduleLogTreeView::filterRegExp() const
{
    return m_model->filterRegExp();
}

void ScheduleLogTreeView::headerContextMenuRequested( const QPoint &pos )
{
    kDebug()<<header()->logicalIndexAt(pos)<<" at"<<pos;
}

void ScheduleLogTreeView::selectionChanged( const QItemSelection &sel, const QItemSelection &desel )
{
    //kDebug()<<sel.indexes().count();
    foreach( const QModelIndex &i, selectionModel()->selectedIndexes() ) {
        //kDebug()<<i.row()<<","<<i.column();
    }
    QTreeView::selectionChanged( sel, desel );
    emit selectionChanged( selectionModel()->selectedIndexes() );
}

void ScheduleLogTreeView::currentChanged( const QModelIndex & current, const QModelIndex & previous )
{
    //kDebug()<<current.row()<<","<<current.column();
    QTreeView::currentChanged( current, previous );
    emit currentChanged( current );
//    selectionModel()->select( current, QItemSelectionModel::Rows | QItemSelectionModel::ClearAndSelect );
}

void ScheduleLogTreeView::slotEditCopy()
{
    QStringList lst;
    int row = 0;
    QString s;
    QHeaderView *h = header();
    foreach( const QModelIndex &i, selectionModel()->selectedRows() ) {
        QString s;
        for ( int section = 0; section < h->count(); ++section ) {
            QModelIndex idx = model()->index( i.row(), h->logicalIndex( section ) );
            if ( ! idx.isValid() || isColumnHidden( idx.column() ) ) {
                continue;
            }
            if ( ! s.isEmpty() ) {
                s += " ";
            }
            s = QString( "%1%2" ).arg( s ).arg( idx.data().toString(), -10 );
        }
        if ( ! s.isEmpty() ) {
            lst << s;
        }
    }
    if ( ! lst.isEmpty() ) {
        QApplication::clipboard()->setText( lst.join( "\n" ) );
    }
}

//-----------------------------------
ScheduleLogView::ScheduleLogView( KoDocument *part, QWidget *parent )
    : ViewBase( part, parent )
{
    setupGui();
    slotEnableActions( 0 );
    
    QVBoxLayout * l = new QVBoxLayout( this );
    l->setMargin( 0 );
    m_view = new ScheduleLogTreeView( this );
    l->addWidget( m_view );
//    m_view->setEditTriggers( m_view->editTriggers() | QAbstractItemView::EditKeyPressed );


    connect( m_view, SIGNAL( currentChanged( QModelIndex ) ), this, SLOT( slotCurrentChanged( QModelIndex ) ) );

    connect( m_view, SIGNAL( selectionChanged( const QModelIndexList ) ), this, SLOT( slotSelectionChanged( const QModelIndexList ) ) );
    
    connect( baseModel(), SIGNAL( dataChanged( const QModelIndex&, const QModelIndex& ) ), this, SLOT( updateActionsEnabled( const QModelIndex& ) ) );

    connect( m_view, SIGNAL( contextMenuRequested( QModelIndex, const QPoint& ) ), this, SLOT( slotContextMenuRequested( QModelIndex, const QPoint& ) ) );
    
}

void ScheduleLogView::setProject( Project *project )
{
    m_view->setProject( project );
}

void ScheduleLogView::draw( Project &project )
{
    setProject( &project );
}

void ScheduleLogView::setGuiActive( bool activate )
{
    //kDebug()<<activate;
    ViewBase::setGuiActive( activate );
/*    if ( activate && !m_view->currentIndex().isValid() ) {
        m_view->selectionModel()->setCurrentIndex(m_view->model()->index( 0, 0 ), QItemSelectionModel::NoUpdate);
    }*/
}

void ScheduleLogView::slotContextMenuRequested( QModelIndex index, const QPoint& pos )
{
    kDebug()<<index.row()<<","<<index.column()<<":"<<pos;
    slotHeaderContextMenuRequested( pos );
}

void ScheduleLogView::slotScheduleSelectionChanged( ScheduleManager *sm )
{
    baseModel()->setManager( sm );
}

void ScheduleLogView::slotCurrentChanged(  const QModelIndex & )
{
    //kDebug()<<curr.row()<<","<<curr.column();
}

void ScheduleLogView::slotSelectionChanged( const QModelIndexList list)
{
    kDebug()<<list.count();
}

void ScheduleLogView::updateActionsEnabled( const QModelIndex &index )
{
    kDebug()<<index;
}

void ScheduleLogView::slotEnableActions( const ScheduleManager * )
{
}

void ScheduleLogView::setupGui()
{
    // Add the context menu actions for the view options
    createOptionAction();
}

void ScheduleLogView::updateReadWrite( bool readwrite )
{
    kDebug()<<readwrite;
    ViewBase::updateReadWrite( readwrite );
//    m_view->setReadWrite( readwrite );
    //slotEnableActions( m_view->currentManager() );
}

void ScheduleLogView::slotOptions()
{
    kDebug();
}

void ScheduleLogView::slotEditCopy()
{
    m_view->slotEditCopy();
}

bool ScheduleLogView::loadContext( const KoXmlElement &context )
{
    kDebug();
    return true;//m_view->loadContext( model()->columnMap(), context );
}

void ScheduleLogView::saveContext( QDomElement &context ) const
{
    //m_view->saveContext( model()->columnMap(), context );
}


//---------------------------

ScheduleHandlerView::ScheduleHandlerView( KoDocument *part, QWidget *parent )
    : SplitterView( part, parent )
{
    kDebug()<<"---------------- Create ScheduleHandlerView ------------------";
    m_scheduleEditor = new ScheduleEditor( part, this );
    m_scheduleEditor->setObjectName( "ScheduleEditor" );
    addView( m_scheduleEditor );

    QTabWidget *tab = addTabWidget();
    
    PertResult *p = new PertResult( part, tab );
    p->setObjectName( "PertResult" );
    addView( p, tab, i18n( "Result" ) );
    connect( m_scheduleEditor, SIGNAL( scheduleSelectionChanged( ScheduleManager* ) ), p, SLOT( slotScheduleSelectionChanged( ScheduleManager* ) ) );
    
    PertCpmView *c = new PertCpmView( part, tab );
    c->setObjectName( "PertCpmView" );
    addView( c, tab, i18n( "Critical Path" ) );
    connect( m_scheduleEditor, SIGNAL( scheduleSelectionChanged( ScheduleManager* ) ), c, SLOT( slotScheduleSelectionChanged( ScheduleManager* ) ) );
    
    ScheduleLogView *v = new ScheduleLogView( part, tab );
    v->setObjectName( "ScheduleLogView" );
    addView( v, tab, i18n( "Scheduling Log" ) );
    connect( m_scheduleEditor, SIGNAL( scheduleSelectionChanged( ScheduleManager* ) ), v, SLOT( slotScheduleSelectionChanged( ScheduleManager* ) ) );
}

ViewBase *ScheduleHandlerView::hitView( const QPoint &glpos )
{
    //kDebug()<<this<<glpos<<"->"<<mapFromGlobal( glpos )<<"in"<<frameGeometry();
    return this;
}


void ScheduleHandlerView::setGuiActive( bool active ) // virtual slot
{
    foreach ( ViewBase *v, findChildren<ViewBase*>() ) {
        v->setGuiActive( active );
    }
    m_activeview = active ? this : 0;
    emit guiActivated( this, active );
}

void ScheduleHandlerView::slotGuiActivated( ViewBase *, bool )
{
}

QStringList ScheduleHandlerView::actionListNames() const
{
    QStringList lst;
    foreach ( ViewBase *v, findChildren<ViewBase*>() ) {
        lst += v->actionListNames();
    }
    return lst;
}
    
QList<QAction*> ScheduleHandlerView::actionList( const QString name ) const
{
    //kDebug()<<name;
    QList<QAction*> lst;
    foreach ( ViewBase *v, findChildren<ViewBase*>() ) {
        lst += v->actionList( name );
    }
    return lst;
}

} // namespace KPlato

#include "kptscheduleeditor.moc"
