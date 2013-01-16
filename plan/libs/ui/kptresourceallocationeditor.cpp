/* This file is part of the KDE project
  Copyright (C) 2009, 2010, 2012 Dag Andersen <danders@get2net.dk>

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

#include "kptresourceallocationeditor.h"

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
#include "kptdebug.h"

#include <KoDocument.h>

#include <QMenu>
#include <QList>
#include <QObject>
#include <QVBoxLayout>


#include <kaction.h>
#include <kglobal.h>
#include <klocale.h>
#include <kactioncollection.h>
#include <kxmlguifactory.h>


namespace KPlato
{


ResourceAllocationTreeView::ResourceAllocationTreeView( QWidget *parent )
    : DoubleTreeViewBase( parent )
{
//    header()->setContextMenuPolicy( Qt::CustomContextMenu );
    ResourceAllocationItemModel *m = new ResourceAllocationItemModel( this );
    setModel( m );

    setSelectionMode( QAbstractItemView::ExtendedSelection );
    setSelectionBehavior( QAbstractItemView::SelectRows );

    createItemDelegates( m );

    connect( m, SIGNAL( dataChanged( const QModelIndex&, const QModelIndex& ) ), SIGNAL( dataChanged() ) );
}

QObject *ResourceAllocationTreeView::currentObject() const
{
    return model()->object( selectionModel()->currentIndex() );
}

//-----------------------------------
ResourceAllocationEditor::ResourceAllocationEditor(KoPart *part, KoDocument *doc, QWidget *parent)
    : ViewBase(part, doc, parent)
{
    QVBoxLayout * l = new QVBoxLayout( this );
    l->setMargin( 0 );
    m_view = new ResourceAllocationTreeView( this );
    l->addWidget( m_view );
    setupGui();

    m_view->setEditTriggers( m_view->editTriggers() | QAbstractItemView::EditKeyPressed );

    QList<int> lst1; lst1 << 1 << -1;
    QList<int> lst2; lst2 << 0;
    m_view->hideColumns( lst1, lst2 );

    m_view->masterView()->setDefaultColumns( QList<int>() << 0 );
    QList<int> show;
    for ( int c = 1; c < model()->columnCount(); ++c ) {
        show << c;
    }
    m_view->slaveView()->setDefaultColumns( show );

    connect( model(), SIGNAL( executeCommand( KUndo2Command* ) ), doc, SLOT( addCommand( KUndo2Command* ) ) );

    connect( m_view, SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ), this, SLOT( slotCurrentChanged( const QModelIndex & ) ) );

    connect( m_view, SIGNAL( selectionChanged( const QModelIndexList ) ), this, SLOT( slotSelectionChanged( const QModelIndexList ) ) );

    connect( m_view, SIGNAL( contextMenuRequested( QModelIndex, const QPoint& ) ), this, SLOT( slotContextMenuRequested( QModelIndex, const QPoint& ) ) );

    connect( m_view, SIGNAL( headerContextMenuRequested( const QPoint& ) ), SLOT( slotHeaderContextMenuRequested( const QPoint& ) ) );

}

void ResourceAllocationEditor::updateReadWrite( bool readwrite )
{
    m_view->setReadWrite( readwrite );
}

void ResourceAllocationEditor::setGuiActive( bool activate )
{
    kDebug(planDbg())<<activate;
    updateActionsEnabled( true );
    ViewBase::setGuiActive( activate );
    if ( activate && !m_view->selectionModel()->currentIndex().isValid() ) {
        m_view->selectionModel()->setCurrentIndex(m_view->model()->index( 0, 0 ), QItemSelectionModel::NoUpdate);
    }
}

void ResourceAllocationEditor::slotContextMenuRequested( QModelIndex index, const QPoint& pos )
{
    //kDebug(planDbg())<<index.row()<<","<<index.column()<<":"<<pos;
    QString name;
    if ( index.isValid() ) {
        QObject *obj = m_view->model()->object( index );
        ResourceGroup *g = qobject_cast<ResourceGroup*>( obj );
        if ( g ) {
            //name = "resourceeditor_group_popup";
        } else {
            Resource *r = qobject_cast<Resource*>( obj );
            if ( r ) {
                //name = "resourceeditor_resource_popup";
            }
        }
    }
    if ( name.isEmpty() ) {
        slotHeaderContextMenuRequested( pos );
        return;
    }
    emit requestPopupMenu( name, pos );
}

Resource *ResourceAllocationEditor::currentResource() const
{
    return qobject_cast<Resource*>( m_view->currentObject() );
}

ResourceGroup *ResourceAllocationEditor::currentResourceGroup() const
{
    return qobject_cast<ResourceGroup*>( m_view->currentObject() );
}

void ResourceAllocationEditor::slotCurrentChanged(  const QModelIndex & )
{
    //kDebug(planDbg())<<curr.row()<<","<<curr.column();
//    slotEnableActions();
}

void ResourceAllocationEditor::slotSelectionChanged( const QModelIndexList )
{
    //kDebug(planDbg())<<list.count();
    updateActionsEnabled();
}

void ResourceAllocationEditor::slotEnableActions( bool on )
{
    updateActionsEnabled( on );
}

void ResourceAllocationEditor::updateActionsEnabled(  bool /*on */)
{
}

void ResourceAllocationEditor::setupGui()
{
    // Add the context menu actions for the view options
    connect(m_view->actionSplitView(), SIGNAL(triggered(bool) ), SLOT(slotSplitView()));
    addContextAction( m_view->actionSplitView() );

    createOptionAction();
}

void ResourceAllocationEditor::slotSplitView()
{
    kDebug(planDbg());
    m_view->setViewSplitMode( ! m_view->isViewSplit() );
    emit optionsModified();
}

void ResourceAllocationEditor::slotOptions()
{
    kDebug(planDbg());
    SplitItemViewSettupDialog *dlg = new SplitItemViewSettupDialog( this, m_view, this );
    dlg->addPrintingOptions();
    connect(dlg, SIGNAL(finished(int)), SLOT(slotOptionsFinished(int)));
    dlg->show();
    dlg->raise();
    dlg->activateWindow();
}


bool ResourceAllocationEditor::loadContext( const KoXmlElement &context )
{
    kDebug(planDbg())<<objectName();
    ViewBase::loadContext( context );
    return m_view->loadContext( model()->columnMap(), context );
}

void ResourceAllocationEditor::saveContext( QDomElement &context ) const
{
    kDebug(planDbg())<<objectName();
    ViewBase::saveContext( context );
    m_view->saveContext( model()->columnMap(), context );
}

KoPrintJob *ResourceAllocationEditor::createPrintJob()
{
    return m_view->createPrintJob( this );
}


} // namespace KPlato

#include "kptresourceallocationeditor.moc"
