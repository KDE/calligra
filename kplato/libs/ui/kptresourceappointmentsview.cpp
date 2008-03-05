/* This file is part of the KDE project
   Copyright (C) 2005 - 2007 Dag Andersen kplato@kde.org>

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

#include "kptresourceappointmentsview.h"

#include "kptappointment.h"
#include "kptcommand.h"
#include "kpteffortcostmap.h"
#include "kptitemmodelbase.h"
#include "kptcalendar.h"
#include "kptduration.h"
#include "kptnode.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptresource.h"
#include "kptdatetime.h"
#include "kptitemviewsettup.h"

#include <KoDocument.h>

#include <QMenu>
#include <QList>
#include <QObject>
#include <QVBoxLayout>
#include <QHeaderView>

#include <kicon.h>
#include <kglobal.h>
#include <klocale.h>
#include <kactioncollection.h>
#include <kxmlguifactory.h>

#include <kabc/addressee.h>
#include <kabc/vcardconverter.h>

#include <kdebug.h>

namespace KPlato
{


//--------------------
ResourceAppointmentsTreeView::ResourceAppointmentsTreeView( QWidget *parent )
    : DoubleTreeViewBase( true, parent )
{
//    header()->setContextMenuPolicy( Qt::CustomContextMenu );
    setStretchLastSection( false );
    
    ResourceAppointmentsItemModel *m = new ResourceAppointmentsItemModel();
    setModel( m );
    setSelectionMode( QAbstractItemView::ExtendedSelection );
    QList<int> lst1; lst1 << 2 << -1;
    QList<int> lst2; lst2 << 0 << 1;
    hideColumns( lst1, lst2 );
    
    //connect( model(), SIGNAL( columnsInserted ( const QModelIndex&, int, int ) ), SLOT( slotColumnsInserted( const QModelIndex&, int, int ) ) );
    //slotRefreshed();
    connect( m, SIGNAL( refreshed() ) , SLOT( slotRefreshed() ) );
}

void ResourceAppointmentsTreeView::slotActivated( const QModelIndex index )
{
    kDebug()<<index.column()<<endl;
}

void ResourceAppointmentsTreeView::slotColumnsInserted( const QModelIndex&, int c1, int c2 )
{
    kDebug()<<c1<<", "<<c2<<endl;
    slotRefreshed();
}

void ResourceAppointmentsTreeView::slotRefreshed()
{
    kDebug()<<model()->columnCount()<<", "<<m_leftview->header()->count()<<", "<<m_rightview->header()->count()<<", "<<m_leftview->header()->hiddenSectionCount()<<", "<<m_rightview->header()->hiddenSectionCount()<<endl;
    m_leftview->selectionModel()->clear();
    QList<int> lst1; lst1 << 2 << -1;
    QList<int> lst2; lst2 << 0 << 1;
    hideColumns( lst1, lst2 );
}

//-----------------------------------

ResourceAppointmentsView::ResourceAppointmentsView( KoDocument *part, QWidget *parent )
    : ViewBase( part, parent )
{
    kDebug()<<"------------------- ResourceAppointmentsView -----------------------"<<endl;

    setupGui();
    
    QVBoxLayout * l = new QVBoxLayout( this );
    l->setMargin( 0 );
    m_view = new ResourceAppointmentsTreeView( this );
    l->addWidget( m_view );
    
    m_view->setEditTriggers( m_view->editTriggers() | QAbstractItemView::EditKeyPressed );

    connect( model(), SIGNAL( executeCommand( QUndoCommand* ) ), part, SLOT( addCommand( QUndoCommand* ) ) );
    
    connect( m_view, SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ), this, SLOT( slotCurrentChanged( const QModelIndex & ) ) );

    connect( m_view, SIGNAL( selectionChanged( const QModelIndexList ) ), this, SLOT( slotSelectionChanged( const QModelIndexList ) ) );

    connect( m_view, SIGNAL( contextMenuRequested( QModelIndex, const QPoint& ) ), this, SLOT( slotContextMenuRequested( QModelIndex, const QPoint& ) ) );
    
    connect( m_view, SIGNAL( headerContextMenuRequested( const QPoint& ) ), SLOT( slotHeaderContextMenuRequested( const QPoint& ) ) );

}

void ResourceAppointmentsView::draw( Project &project )
{
    setProject( &project );
}

void ResourceAppointmentsView::setProject( Project *project )
{
    m_view->setProject( project );
}

void ResourceAppointmentsView::setScheduleManager( ScheduleManager *sm )
{
    m_view->setScheduleManager( sm );
}

void ResourceAppointmentsView::draw()
{
}

void ResourceAppointmentsView::setGuiActive( bool activate )
{
    kDebug()<<activate<<endl;
    updateActionsEnabled( true );
    ViewBase::setGuiActive( activate );
    if ( activate && !m_view->selectionModel()->currentIndex().isValid() ) {
        m_view->selectionModel()->setCurrentIndex(m_view->model()->index( 0, 0 ), QItemSelectionModel::NoUpdate);
    }
}

void ResourceAppointmentsView::slotContextMenuRequested( QModelIndex index, const QPoint& pos )
{
    //kDebug()<<index.row()<<", "<<index.column()<<": "<<pos<<endl;
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
    emit requestPopupMenu( name, pos );*/
}

void ResourceAppointmentsView::slotHeaderContextMenuRequested( const QPoint &pos )
{
    kDebug()<<endl;
    QList<QAction*> lst = contextActionList();
    if ( ! lst.isEmpty() ) {
        QMenu::exec( lst, pos,  lst.first() );
    }
}

Resource *ResourceAppointmentsView::currentResource() const
{
    //return qobject_cast<Resource*>( m_view->currentObject() );
    return 0;
}

ResourceGroup *ResourceAppointmentsView::currentResourceGroup() const
{
    //return qobject_cast<ResourceGroup*>( m_view->currentObject() );
    return 0;
}

void ResourceAppointmentsView::slotCurrentChanged(  const QModelIndex & )
{
    //kDebug()<<curr.row()<<", "<<curr.column()<<endl;
//    slotEnableActions();
}

void ResourceAppointmentsView::slotSelectionChanged( const QModelIndexList )
{
    //kDebug()<<list.count()<<endl;
    updateActionsEnabled();
}

void ResourceAppointmentsView::slotEnableActions( bool on )
{
    updateActionsEnabled( on );
}

void ResourceAppointmentsView::updateActionsEnabled(  bool on )
{
/*    bool o = on && m_view->project();

    QList<ResourceGroup*> groupList = m_view->selectedGroups();
    bool nogroup = groupList.isEmpty();
    bool group = groupList.count() == 1;
    QList<Resource*> resourceList = m_view->selectedResources();
    bool noresource = resourceList.isEmpty(); 
    bool resource = resourceList.count() == 1;
    
    bool any = !nogroup || !noresource;
    
    actionAddResource->setEnabled( o && ( (group  && noresource) || (resource && nogroup) ) );
    actionAddGroup->setEnabled( o );
    actionDeleteSelection->setEnabled( o && any );*/
}

void ResourceAppointmentsView::setupGui()
{
/*    QString name = "resourceeditor_edit_list";
    actionAddResource  = new KAction(KIcon( "document-new" ), i18n("Add Resource..."), this);
    actionCollection()->addAction("add_resource", actionAddResource );
    actionAddResource->setShortcut( KShortcut( Qt::CTRL Qt::SHIFT + Qt::Key_I ) );
    connect( actionAddResource, SIGNAL( triggered( bool ) ), SLOT( slotAddResource() ) );
    addAction( name, actionAddResource );
    
    actionAddGroup  = new KAction(KIcon( "document-new" ), i18n("Add Resource Group..."), this);
    actionCollection()->addAction("add_group", actionAddGroup );
    actionAddGroup->setShortcut( KShortcut( Qt::CTRL Qt::Key_I ) );
    connect( actionAddGroup, SIGNAL( triggered( bool ) ), SLOT( slotAddGroup() ) );
    addAction( name, actionAddGroup );
    
    actionDeleteSelection  = new KAction(KIcon( "edit-delete" ), i18n("Delete Selected Items"), this);
    actionCollection()->addAction("delete_selection", actionDeleteSelection );
    actionDeleteSelection->setShortcut( KShortcut( Qt::Key_Delete ) );
    connect( actionDeleteSelection, SIGNAL( triggered( bool ) ), SLOT( slotDeleteSelection() ) );
    addAction( name, actionDeleteSelection );
    */
    // Add the context menu actions for the view options
/*    actionOptions = new KAction(KIcon("configure"), i18n("Configure..."), this);
    connect(actionOptions, SIGNAL(triggered(bool) ), SLOT(slotOptions()));
    addContextAction( actionOptions );*/
}

void ResourceAppointmentsView::slotOptions()
{
    kDebug()<<endl;
    ItemViewSettupDialog dlg( m_view->masterView() );
    dlg.exec();
}


void ResourceAppointmentsView::slotAddResource()
{
    //kDebug()<<endl;
/*    QList<ResourceGroup*> gl = m_view->selectedGroups();
    if ( gl.count() > 1 ) {
        return;
    }
    ResourceGroup *g = 0;
    if ( !gl.isEmpty() ) {
        g = gl.first();
    } else {
        QList<Resource*> rl = m_view->selectedResources();
        if ( rl.count() != 1 ) {
            return;
        }
        g = rl.first()->parentGroup();
    }
    if ( g == 0 ) {
        return;
    }
    Resource *r = new Resource();
    QModelIndex i = m_view->model()->insertResource( g, r );
    if ( i.isValid() ) {
        m_view->selectionModel()->setCurrentIndex( i, QItemSelectionModel::NoUpdate );
        m_view->edit( i );
    }
*/
}

void ResourceAppointmentsView::slotAddGroup()
{
    //kDebug()<<endl;
/*    ResourceGroup *g = new ResourceGroup();
    QModelIndex i = m_view->model()->insertGroup( g );
    if ( i.isValid() ) {
        m_view->selectionModel()->setCurrentIndex( i, QItemSelectionModel::NoUpdate );
        m_view->edit( i );
    }*/
}

void ResourceAppointmentsView::slotDeleteSelection()
{
/*    QObjectList lst = m_view->selectedObjects();
    //kDebug()<<lst.count()<<" objects"<<endl;
    if ( ! lst.isEmpty() ) {
        emit deleteObjectList( lst );
    }*/
}

bool ResourceAppointmentsView::loadContext( const KoXmlElement &context )
{
    kDebug()<<endl;
    return m_view->loadContext( model()->columnMap(), context );
}

void ResourceAppointmentsView::saveContext( QDomElement &context ) const
{
    m_view->saveContext( model()->columnMap(), context );
}


} // namespace KPlato

#include "kptresourceappointmentsview.moc"
