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

#include <KoDocument.h>

#include <QMenu>
#include <QList>
#include <QObject>
#include <QVBoxLayout>
#include <QHeaderView>

#include <kaction.h>
#include <kicon.h>
#include <kglobal.h>
#include <klocale.h>
#include <kxmlguifactory.h>
#include <kactioncollection.h>

#include <kdebug.h>

namespace KPlato
{

ScheduleTreeView::ScheduleTreeView( QWidget *parent )
    : TreeViewBase( parent )
{
    header()->setStretchLastSection ( false );
    header()->setContextMenuPolicy( Qt::CustomContextMenu );
    setModel( new ScheduleItemModel() );
    setSelectionModel( new QItemSelectionModel( model() ) );
    setSelectionMode( QAbstractItemView::SingleSelection );
    setSelectionBehavior( QAbstractItemView::SelectRows );
    
    createItemDelegates();

    hideColumn( 4 );
    
    connect( header(), SIGNAL( customContextMenuRequested ( const QPoint& ) ), this, SLOT( headerContextMenuRequested( const QPoint& ) ) );
    connect( this, SIGNAL( activated ( const QModelIndex ) ), this, SLOT( slotActivated( const QModelIndex ) ) );

}

void ScheduleTreeView::slotActivated( const QModelIndex index )
{
    kDebug()<<index.column();
}

void ScheduleTreeView::headerContextMenuRequested( const QPoint &pos )
{
    kDebug()<<header()->logicalIndexAt(pos)<<" at"<<pos;
}

void ScheduleTreeView::selectionChanged( const QItemSelection &sel, const QItemSelection &desel )
{
    //kDebug()<<sel.indexes().count();
    foreach( QModelIndex i, selectionModel()->selectedIndexes() ) {
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

    connect( model(), SIGNAL( executeCommand( QUndoCommand* ) ), part, SLOT( addCommand( QUndoCommand* ) ) );

    connect( m_view, SIGNAL( currentChanged( QModelIndex ) ), this, SLOT( slotCurrentChanged( QModelIndex ) ) );

    connect( m_view, SIGNAL( selectionChanged( const QModelIndexList ) ), this, SLOT( slotSelectionChanged( const QModelIndexList ) ) );
    
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

void ScheduleEditor::slotHeaderContextMenuRequested( const QPoint &pos )
{
    kDebug()<<endl;
    QList<QAction*> lst = contextActionList();
    if ( ! lst.isEmpty() ) {
        QMenu::exec( lst, pos,  lst.first() );
    }
}

void ScheduleEditor::slotContextMenuRequested( QModelIndex index, const QPoint& pos )
{
    kDebug()<<index.row()<<","<<index.column()<<":"<<pos;
/*    QString name;
    if ( index.isValid() ) {
        QObject *obj = m_view->model()->object( index );
        ResourceGroup *g = qobject_cast<ResourceGroup*>( obj );
        if ( g ) {
            name = "resourceeditor_group_popup";
        } else {
            Resource *r = qobject_cast<Resource*>( obj );
            if ( r ) {
                name = "resourceeditor_resource_popup";
            }
        }
    }
    kDebug()<<name;
    emit requestPopupMenu( name, pos );*/
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

void ScheduleEditor::slotEnableActions( const ScheduleManager *sm )
{
    bool on = isReadWrite() && sm != 0;
    actionDeleteSelection->setEnabled( on );
    actionCalculateSchedule->setEnabled( on );
    actionAddSubSchedule->setEnabled( on );
    
    actionAddSchedule->setEnabled( isReadWrite() );
}

void ScheduleEditor::setupGui()
{
    QString name = "scheduleeditor_edit_list";
    
    actionCalculateSchedule  = new KAction(KIcon( "project_calculate" ), i18n("Calculate Schedule..."), this);
    actionCollection()->addAction("calculate_schedule", actionCalculateSchedule );
    connect( actionCalculateSchedule, SIGNAL( triggered( bool ) ), SLOT( slotCalculateSchedule() ) );
    addAction( name, actionCalculateSchedule );
    
    actionAddSchedule  = new KAction(KIcon( "document-new" ), i18n("Add Schedule..."), this);
    actionCollection()->addAction("add_schedule", actionAddSchedule );
    connect( actionAddSchedule, SIGNAL( triggered( bool ) ), SLOT( slotAddSchedule() ) );
    addAction( name, actionAddSchedule );
    
    actionAddSubSchedule  = new KAction(KIcon( "document-new" ), i18n("Add Sub-schedule..."), this);
    actionCollection()->addAction("add_subschedule", actionAddSubSchedule );
    connect( actionAddSubSchedule, SIGNAL( triggered( bool ) ), SLOT( slotAddSubSchedule() ) );
    addAction( name, actionAddSubSchedule );
    
    actionDeleteSelection  = new KAction(KIcon( "edit-delete" ), i18n("Delete Selected Schedules"), this);
    actionCollection()->addAction("schedule_delete_selection", actionDeleteSelection );
    connect( actionDeleteSelection, SIGNAL( triggered( bool ) ), SLOT( slotDeleteSelection() ) );
    addAction( name, actionDeleteSelection );

    // Add the context menu actions for the view options
    actionOptions = new KAction(KIcon("configure"), i18n("Configure..."), this);
    connect(actionOptions, SIGNAL(triggered(bool) ), SLOT(slotOptions()));
    addContextAction( actionOptions );
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
    kDebug()<<endl;
    ItemViewSettupDialog dlg( m_view );
    dlg.exec();
}

void ScheduleEditor::slotCalculateSchedule()
{
    //kDebug()<<endl;
    ScheduleManager *sm = m_view->currentManager();
    if ( sm == 0 ) {
        return;
    }
    sm->setRecalculate( sm->parentManager() );
    emit calculateSchedule( m_view->project(), sm );
}

void ScheduleEditor::slotAddSchedule()
{
    //kDebug()<<endl;
    ScheduleManager *sm = m_view->currentManager();
    if ( sm && sm->parentManager() ) {
        sm = sm->parentManager();
        ScheduleManager *m = m_view->project()->createScheduleManager( sm->name() + QString(".%1").arg( sm->children().count() + 1 ) );
        part()->addCommand( new AddScheduleManagerCmd( sm, m, i18n( "Create sub-schedule" ) ) );
    } else {
        emit addScheduleManager( m_view->project() );
    }
}

void ScheduleEditor::slotAddSubSchedule()
{
    //kDebug()<<endl;
    ScheduleManager *sm = m_view->currentManager();
    if ( sm ) {
        ScheduleManager *m = m_view->project()->createScheduleManager( sm->name() + QString(".%1").arg( sm->children().count() + 1 ) );
        
        part()->addCommand( new AddScheduleManagerCmd( sm, m, i18n( "Create sub-schedule" ) ) );
    } else {
        emit addScheduleManager( m_view->project() );
    }
}

void ScheduleEditor::slotDeleteSelection()
{
    //kDebug()<<endl;
    ScheduleManager *sm = m_view->currentManager();
    if ( sm ) {
        emit deleteScheduleManager( m_view->project(), sm );
    }
}

bool ScheduleEditor::loadContext( const KoXmlElement &context )
{
    kDebug()<<endl;
    return m_view->loadContext( context );
}

void ScheduleEditor::saveContext( QDomElement &context ) const
{
    m_view->saveContext( context );
}


//---------------------------

ScheduleHandlerView::ScheduleHandlerView( KoDocument *part, QWidget *parent )
    : SplitterView( part, parent )
{
    m_scheduleEditor = new ScheduleEditor( part, this );
    m_scheduleEditor->setObjectName( "ScheduleEditor" );
    addView( m_scheduleEditor );

    QTabWidget *tab = addTabWidget();
    
    PertResult *p = new PertResult( part );
    p->setObjectName( "PertResult" );
    addView( p, tab, i18n( "Result" ) );

    connect( m_scheduleEditor, SIGNAL( scheduleSelectionChanged( ScheduleManager* ) ), p, SLOT( slotScheduleSelectionChanged( ScheduleManager* ) ) );
    
    PertCpmView *c = new PertCpmView( part );
    c->setObjectName( "PertCpmView" );
    addView( c, tab, i18n( "Critical Path" ) );

    connect( m_scheduleEditor, SIGNAL( scheduleSelectionChanged( ScheduleManager* ) ), c, SLOT( slotScheduleSelectionChanged( ScheduleManager* ) ) );
    
}

ViewBase *ScheduleHandlerView::hitView( const QPoint &glpos )
{
    //kDebug()<<this<<glpos<<"->"<<mapFromGlobal( glpos )<<"in"<<frameGeometry();
    return this;
}


void ScheduleHandlerView::setGuiActive( bool active ) // virtual slot
{
    //kDebug()<<active;
    for ( int i = 0; i < m_splitter->count(); ++i ) {
        ViewBase *v = dynamic_cast<ViewBase*>( m_splitter->widget( i ) );
        if ( v  ) {
            v->setGuiActive( active );
        } else {
            QTabWidget *tw = dynamic_cast<QTabWidget*>( m_splitter->widget( i ) );
            if (tw ) {
                v = dynamic_cast<ViewBase*>( tw->currentWidget() );
                if ( v ) {
                    v->setGuiActive( active );
                }
            }
        }
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
    for ( int i = 0; i < m_splitter->count(); ++i ) {
        ViewBase *v = dynamic_cast<ViewBase*>( m_splitter->widget( i ) );
        if ( v  ) {
            lst += v->actionListNames();
        } else {
            QTabWidget *tw = dynamic_cast<QTabWidget*>( m_splitter->widget( i ) );
            if (tw ) {
                v = dynamic_cast<ViewBase*>( tw->currentWidget() );
                lst += v->actionListNames();
            }
        }
    }
    //kDebug()<<lst;
    return lst;
}
    
QList<QAction*> ScheduleHandlerView::actionList( const QString name ) const
{
    //kDebug()<<name;
    QList<QAction*> lst;
    for ( int i = 0; i < m_splitter->count(); ++i ) {
        ViewBase *v = dynamic_cast<ViewBase*>( m_splitter->widget( i ) );
        if ( v ) {
            lst += v->actionList( name );
        } else {
            QTabWidget *tw = dynamic_cast<QTabWidget*>( m_splitter->widget( i ) );
            if (tw ) {
                v = dynamic_cast<ViewBase*>( tw->currentWidget() );
                if ( v  ) {
                    lst += v->actionList( name );
                }
            }
        }
    }
    //kDebug()<<lst;
    return lst;
}

} // namespace KPlato

#include "kptscheduleeditor.moc"
