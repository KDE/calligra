/* This file is part of the KDE project
  Copyright (C) 2006 - 2007 Dag Andersen kplato@kde.org>

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

#include "kptresourceeditor.h"

#include "kptresourcemodel.h"
#include "kptcommand.h"
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
#include <QDragMoveEvent>

#include <kaction.h>
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


ResourceTreeView::ResourceTreeView( QWidget *parent )
    : DoubleTreeViewBase( parent )
{
//    header()->setContextMenuPolicy( Qt::CustomContextMenu );
    setStretchLastSection( false );
    ResourceItemModel *m = new ResourceItemModel( this );
    setModel( m );
    
    setSelectionMode( QAbstractItemView::ExtendedSelection );
    setSelectionBehavior( QAbstractItemView::SelectRows );

    createItemDelegates( m );

    connect( this, SIGNAL( dropAllowed( const QModelIndex&, int, QDragMoveEvent* ) ), SLOT(slotDropAllowed( const QModelIndex&, int, QDragMoveEvent* ) ) );

}

void ResourceTreeView::slotDropAllowed( const QModelIndex &index, int dropIndicatorPosition, QDragMoveEvent *event )
{
    if ( model()->dropAllowed( index, dropIndicatorPosition, event->mimeData() ) ) {
        event->accept();
    }
}

QObject *ResourceTreeView::currentObject() const
{
    return model()->object( selectionModel()->currentIndex() );
}

QList<QObject*> ResourceTreeView::selectedObjects() const
{
    QList<QObject*> lst;
    foreach (const QModelIndex &i, selectionModel()->selectedRows() ) {
        lst << static_cast<QObject*>( i.internalPointer() );
    }
    return lst;
}

QList<ResourceGroup*> ResourceTreeView::selectedGroups() const
{
    QList<ResourceGroup*> gl;
    foreach ( QObject *o, selectedObjects() ) {
        ResourceGroup* g = qobject_cast<ResourceGroup*>( o );
        if ( g ) {
            gl << g;
        }
    }
    return gl;
}

QList<Resource*> ResourceTreeView::selectedResources() const
{
    QList<Resource*> rl;
    foreach ( QObject *o, selectedObjects() ) {
        Resource* r = qobject_cast<Resource*>( o );
        if ( r ) {
            rl << r;
        }
    }
    return rl;
}

//-----------------------------------
ResourceEditor::ResourceEditor( KoDocument *part, QWidget *parent )
    : ViewBase( part, parent )
{
    QVBoxLayout * l = new QVBoxLayout( this );
    l->setMargin( 0 );
    m_view = new ResourceTreeView( this );
    l->addWidget( m_view );
    setupGui();
    
    m_view->setEditTriggers( m_view->editTriggers() | QAbstractItemView::EditKeyPressed );
    m_view->setDragDropMode( QAbstractItemView::DragDrop );
    m_view->setDropIndicatorShown( true );
    m_view->setDragEnabled ( true );
    m_view->setAcceptDrops( true );
//    m_view->setAcceptDropsOnView( true );


    QList<int> lst1; lst1 << 1 << -1;
    QList<int> lst2; lst2 << 0;
    m_view->hideColumns( lst1, lst2 );
    
    m_view->masterView()->setDefaultColumns( QList<int>() << 0 );
    QList<int> show;
    for ( int c = 1; c < model()->columnCount(); ++c ) {
        show << c;
    }
    m_view->slaveView()->setDefaultColumns( show );

    connect( model(), SIGNAL( executeCommand( QUndoCommand* ) ), part, SLOT( addCommand( QUndoCommand* ) ) );

    connect( m_view, SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ), this, SLOT( slotCurrentChanged( const QModelIndex & ) ) );

    connect( m_view, SIGNAL( selectionChanged( const QModelIndexList ) ), this, SLOT( slotSelectionChanged( const QModelIndexList ) ) );

    connect( m_view, SIGNAL( contextMenuRequested( QModelIndex, const QPoint& ) ), this, SLOT( slotContextMenuRequested( QModelIndex, const QPoint& ) ) );
    
    connect( m_view, SIGNAL( headerContextMenuRequested( const QPoint& ) ), SLOT( slotHeaderContextMenuRequested( const QPoint& ) ) );

}

void ResourceEditor::updateReadWrite( bool readwrite )
{
    m_view->setReadWrite( readwrite );
}

void ResourceEditor::draw( Project &project )
{
    m_view->setProject( &project );
}

void ResourceEditor::draw()
{
}

void ResourceEditor::setGuiActive( bool activate )
{
    kDebug()<<activate;
    updateActionsEnabled( true );
    ViewBase::setGuiActive( activate );
    if ( activate && !m_view->selectionModel()->currentIndex().isValid() ) {
        m_view->selectionModel()->setCurrentIndex(m_view->model()->index( 0, 0 ), QItemSelectionModel::NoUpdate);
    }
}

void ResourceEditor::slotContextMenuRequested( QModelIndex index, const QPoint& pos )
{
    //kDebug()<<index.row()<<","<<index.column()<<":"<<pos;
    QString name;
    if ( index.isValid() ) {
        QObject *obj = m_view->model()->object( index );
        ResourceGroup *g = qobject_cast<ResourceGroup*>( obj );
        if ( g ) {
            //name = "resourceeditor_group_popup";
        } else {
            Resource *r = qobject_cast<Resource*>( obj );
            if ( r ) {
                name = "resourceeditor_resource_popup";
            }
        }
    }
    if ( name.isEmpty() ) {
        slotHeaderContextMenuRequested( pos );
        return;
    }
    emit requestPopupMenu( name, pos );
}

Resource *ResourceEditor::currentResource() const
{
    return qobject_cast<Resource*>( m_view->currentObject() );
}

ResourceGroup *ResourceEditor::currentResourceGroup() const
{
    return qobject_cast<ResourceGroup*>( m_view->currentObject() );
}

void ResourceEditor::slotCurrentChanged(  const QModelIndex & )
{
    //kDebug()<<curr.row()<<","<<curr.column();
//    slotEnableActions();
}

void ResourceEditor::slotSelectionChanged( const QModelIndexList )
{
    //kDebug()<<list.count();
    updateActionsEnabled();
}

void ResourceEditor::slotEnableActions( bool on )
{
    updateActionsEnabled( on );
}

void ResourceEditor::updateActionsEnabled(  bool on )
{
    bool o = on && m_view->project();

    QList<ResourceGroup*> groupList = m_view->selectedGroups();
    bool nogroup = groupList.isEmpty();
    bool group = groupList.count() == 1;
    QList<Resource*> resourceList = m_view->selectedResources();
    bool noresource = resourceList.isEmpty(); 
    bool resource = resourceList.count() == 1;
    
    bool any = !nogroup || !noresource;
    
    actionAddResource->setEnabled( o && ( (group  && noresource) || (resource && nogroup) ) );
    actionAddGroup->setEnabled( o );
    actionDeleteSelection->setEnabled( o && any );
}

void ResourceEditor::setupGui()
{
    QString name = "resourceeditor_edit_list";
    actionAddGroup  = new KAction(KIcon( "resource-group-new" ), i18n("Add Resource Group..."), this);
    actionCollection()->addAction("add_group", actionAddGroup );
    actionAddGroup->setShortcut( KShortcut( Qt::CTRL + Qt::Key_I ) );
    connect( actionAddGroup, SIGNAL( triggered( bool ) ), SLOT( slotAddGroup() ) );
    addAction( name, actionAddGroup );
    
    actionAddResource  = new KAction(KIcon( "list-add-user" ), i18n("Add Resource..."), this);
    actionCollection()->addAction("add_resource", actionAddResource );
    actionAddResource->setShortcut( KShortcut( Qt::CTRL + Qt::SHIFT + Qt::Key_I ) );
    connect( actionAddResource, SIGNAL( triggered( bool ) ), SLOT( slotAddResource() ) );
    addAction( name, actionAddResource );
    
    actionDeleteSelection  = new KAction(KIcon( "edit-delete" ), i18n("Delete Selected Items"), this);
    actionCollection()->addAction("delete_selection", actionDeleteSelection );
    actionDeleteSelection->setShortcut( KShortcut( Qt::Key_Delete ) );
    connect( actionDeleteSelection, SIGNAL( triggered( bool ) ), SLOT( slotDeleteSelection() ) );
    addAction( name, actionDeleteSelection );
    
    // Add the context menu actions for the view options
    connect(m_view->actionSplitView(), SIGNAL(triggered(bool) ), SLOT(slotSplitView()));
    addContextAction( m_view->actionSplitView() );
    
    createOptionAction();
}

void ResourceEditor::slotSplitView()
{
    kDebug();
    m_view->setViewSplitMode( ! m_view->isViewSplit() );
}

void ResourceEditor::slotOptions()
{
    kDebug();
    SplitItemViewSettupDialog *dlg = new SplitItemViewSettupDialog( m_view, this );
    dlg->exec();
    delete dlg;
}


void ResourceEditor::slotAddResource()
{
    //kDebug();
    QList<ResourceGroup*> gl = m_view->selectedGroups();
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
        m_view->selectionModel()->select( i, QItemSelectionModel::Rows | QItemSelectionModel::ClearAndSelect );
        m_view->selectionModel()->setCurrentIndex( i, QItemSelectionModel::NoUpdate );
        m_view->edit( i );
    }

}

void ResourceEditor::slotAddGroup()
{
    //kDebug();
    ResourceGroup *g = new ResourceGroup();
    QModelIndex i = m_view->model()->insertGroup( g );
    if ( i.isValid() ) {
        m_view->selectionModel()->select( i, QItemSelectionModel::Rows | QItemSelectionModel::ClearAndSelect );
        m_view->selectionModel()->setCurrentIndex( i, QItemSelectionModel::NoUpdate );
        m_view->edit( i );
    }
}

void ResourceEditor::slotDeleteSelection()
{
    QObjectList lst = m_view->selectedObjects();
    //kDebug()<<lst.count()<<" objects";
    if ( ! lst.isEmpty() ) {
        emit deleteObjectList( lst );
        QModelIndex i = m_view->selectionModel()->currentIndex();
        if ( i.isValid() ) {
            m_view->selectionModel()->select( i, QItemSelectionModel::Rows | QItemSelectionModel::ClearAndSelect );
            m_view->selectionModel()->setCurrentIndex( i, QItemSelectionModel::NoUpdate );
        }
    }
}

bool ResourceEditor::loadContext( const KoXmlElement &context )
{
    kDebug()<<objectName();
    return m_view->loadContext( model()->columnMap(), context );
}

void ResourceEditor::saveContext( QDomElement &context ) const
{
    kDebug()<<objectName();
    m_view->saveContext( model()->columnMap(), context );
}

KoPrintJob *ResourceEditor::createPrintJob()
{
    return m_view->createPrintJob( this );
}


} // namespace KPlato

#include "kptresourceeditor.moc"
