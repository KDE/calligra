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

ResourceAppointmentsDisplayOptionsPanel::ResourceAppointmentsDisplayOptionsPanel( ResourceAppointmentsItemModel *model, QWidget *parent )
    : QWidget( parent ),
    m_model( model )
{
    setupUi( this );
    setValues( *model );

    connect( ui_internalAppointments, SIGNAL(  stateChanged ( int ) ), SIGNAL( changed() ) );
    connect( ui_externalAppointments, SIGNAL(  stateChanged ( int ) ), SIGNAL( changed() ) );
}

void ResourceAppointmentsDisplayOptionsPanel::slotOk()
{
    m_model->setShowInternalAppointments( ui_internalAppointments->checkState() == Qt::Checked );
    m_model->setShowExternalAppointments( ui_externalAppointments->checkState() == Qt::Checked );
}

void ResourceAppointmentsDisplayOptionsPanel::setValues( const ResourceAppointmentsItemModel &m )
{
    ui_internalAppointments->setCheckState( m.showInternalAppointments() ? Qt::Checked : Qt::Unchecked );
    ui_externalAppointments->setCheckState( m.showExternalAppointments() ? Qt::Checked : Qt::Unchecked );
}

void ResourceAppointmentsDisplayOptionsPanel::setDefault()
{
    ResourceAppointmentsItemModel m;
    setValues( m );
}

//----
ResourceAppointmentsSettingsDialog::ResourceAppointmentsSettingsDialog( ResourceAppointmentsItemModel *model, QWidget *parent )
    : KPageDialog( parent )
{
    ResourceAppointmentsDisplayOptionsPanel *panel = new ResourceAppointmentsDisplayOptionsPanel( model );
    KPageWidgetItem *page = addPage( panel, i18n( "General" ) );
    page->setHeader( i18n( "Resource Assignments View Settings" ) );
    
    connect( this, SIGNAL( okClicked() ), panel, SLOT( slotOk() ) );
    connect( this, SIGNAL( defaultClicked() ), panel, SLOT( setDefault() ) );
}

//---------------------------------------
ResourceAppointmentsTreeView::ResourceAppointmentsTreeView( QWidget *parent )
    : DoubleTreeViewBase( true, parent )
{
//    header()->setContextMenuPolicy( Qt::CustomContextMenu );
    m_rightview->setStretchLastSection( false );
    
    ResourceAppointmentsItemModel *m = new ResourceAppointmentsItemModel( this );
    setModel( m );
    setSelectionMode( QAbstractItemView::ExtendedSelection );
    QList<int> lst1; lst1 << 2 << -1;
    QList<int> lst2; lst2 << 0 << 1;
    hideColumns( lst1, lst2 );
    
    m_leftview->resizeColumnToContents ( 1 );
    connect( m, SIGNAL( modelReset() ), SLOT( slotRefreshed() ) );
    
    m_rightview->setObjectName( "ResourceAppointments" );
}

bool ResourceAppointmentsTreeView::loadContext( const KoXmlElement &context )
{
    kDebug();
    KoXmlElement e = context.namedItem( "common" ).toElement();
    if ( ! e.isNull() ) {
        model()->setShowInternalAppointments( (bool)( e.attribute( "show-internal-appointments", "0" ).toInt() ) );
        model()->setShowExternalAppointments( (bool)( e.attribute( "show-external-appointments", "0" ).toInt() ) );
    }
    return true;
}

void ResourceAppointmentsTreeView::saveContext( QDomElement &settings ) const
{
    kDebug();
    QDomElement e = settings.ownerDocument().createElement( "common" );
    settings.appendChild( e );
    e.setAttribute( "show-internal-appointments", model()->showInternalAppointments() );
    e.setAttribute( "show-external-appointments", model()->showExternalAppointments() );
}

void ResourceAppointmentsTreeView::slotRefreshed()
{
    //kDebug()<<model()->columnCount()<<", "<<m_leftview->header()->count()<<", "<<m_rightview->header()->count()<<", "<<m_leftview->header()->hiddenSectionCount()<<", "<<m_rightview->header()->hiddenSectionCount();
    ResourceAppointmentsItemModel *m = model();
    setModel( 0 );
    setModel( m );
    setSelectionMode( QAbstractItemView::ExtendedSelection );
    QList<int> lst1; lst1 << 2 << -1;
    QList<int> lst2; lst2 << 0 << 1;
    hideColumns( lst1, lst2 );
}

QModelIndex ResourceAppointmentsTreeView::currentIndex() const
{
    return selectionModel()->currentIndex();
}

//-----------------------------------

ResourceAppointmentsView::ResourceAppointmentsView( KoDocument *part, QWidget *parent )
    : ViewBase( part, parent )
{
    kDebug()<<"------------------- ResourceAppointmentsView -----------------------";

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
    kDebug()<<activate;
    updateActionsEnabled( true );
    ViewBase::setGuiActive( activate );
    if ( activate && !m_view->selectionModel()->currentIndex().isValid() ) {
        m_view->selectionModel()->setCurrentIndex(m_view->model()->index( 0, 0 ), QItemSelectionModel::NoUpdate);
    }
}

void ResourceAppointmentsView::slotContextMenuRequested( QModelIndex index, const QPoint& pos )
{
    kDebug()<<index<<pos;
    QString name;
    if ( index.isValid() ) {
        Node *n = m_view->model()->node( index );
        if ( n ) {
            name = "taskview_popup";
        }
    }
    if ( name.isEmpty() ) {
        slotHeaderContextMenuRequested( pos );
        return;
    }
    emit requestPopupMenu( name, pos );
}

Node *ResourceAppointmentsView::currentNode() const
{
    return m_view->model()->node( m_view->currentIndex() );
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
    //kDebug()<<curr.row()<<", "<<curr.column();
//    slotEnableActions();
}

void ResourceAppointmentsView::slotSelectionChanged( const QModelIndexList )
{
    //kDebug()<<list.count();
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
    // Add the context menu actions for the view options
    createOptionAction();
}

void ResourceAppointmentsView::slotOptions()
{
    kDebug();
    ResourceAppointmentsSettingsDialog *dlg = new ResourceAppointmentsSettingsDialog( m_view->model(), this );
    dlg->exec();
    delete dlg;
}


void ResourceAppointmentsView::slotAddResource()
{
    //kDebug();
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
    //kDebug();
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
    //kDebug()<<lst.count()<<" objects";
    if ( ! lst.isEmpty() ) {
        emit deleteObjectList( lst );
    }*/
}

bool ResourceAppointmentsView::loadContext( const KoXmlElement &context )
{
    return m_view->loadContext( context );
}

void ResourceAppointmentsView::saveContext( QDomElement &context ) const
{
    m_view->saveContext( context );
}

KoPrintJob *ResourceAppointmentsView::createPrintJob()
{
    return m_view->createPrintJob( this );
}

} // namespace KPlato

#include "kptresourceappointmentsview.moc"
