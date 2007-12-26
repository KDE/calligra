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

#include "kptrelationeditor.h"

#include "kptglobal.h"
#include "kptcommand.h"
#include "kptproject.h"
#include "kptitemviewsettup.h"

#include <KoDocument.h>

#include <QAbstractItemView>
#include <QItemSelectionModel>
#include <QModelIndex>
#include <QVBoxLayout>
#include <QWidget>
#include <QMenu>

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

namespace KPlato
{

//--------------------
RelationTreeView::RelationTreeView( QWidget *parent )
    : DoubleTreeViewBase( parent )
{
    setViewSplitMode( false );
    setModel( new RelationItemModel() );
    setSelectionMode( QAbstractItemView::SingleSelection );
    setSelectionBehavior( QAbstractItemView::SelectRows );
    
    setRootIsDecorated ( false );

    createItemDelegates();
}

void RelationTreeView::slotActivated( const QModelIndex index )
{
    kDebug()<<index.column();
}

//-----------------------------------
RelationEditor::RelationEditor( KoDocument *part, QWidget *parent )
    : ViewBase( part, parent )
{
    kDebug()<<"----------------- Create RelationEditor ----------------------";

    QVBoxLayout * l = new QVBoxLayout( this );
    l->setMargin( 0 );
    m_view = new RelationTreeView( this );
    l->addWidget( m_view );
    kDebug()<<m_view->actionSplitView();
    setupGui();
    
    connect( m_view, SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ), this, SLOT ( slotCurrentChanged( const QModelIndex &, const QModelIndex & ) ) );

    connect( m_view, SIGNAL( selectionChanged( const QModelIndexList ) ), this, SLOT ( slotSelectionChanged( const QModelIndexList ) ) );
    
    connect( m_view, SIGNAL( contextMenuRequested( const QModelIndex&, const QPoint& ) ), SLOT( slotContextMenuRequested( const QModelIndex&, const QPoint& ) ) );

    connect( m_view, SIGNAL( headerContextMenuRequested( const QPoint& ) ), SLOT( slotHeaderContextMenuRequested( const QPoint& ) ) );
    
    connect( model(), SIGNAL( executeCommand( QUndoCommand* ) ), part, SLOT( addCommand( QUndoCommand* ) ) );
    
}

void RelationEditor::updateReadWrite( bool rw )
{
    m_view->setReadWrite( rw );
}

void RelationEditor::draw( Project &project )
{
    m_view->setProject( &project );
}

void RelationEditor::draw()
{
}

void RelationEditor::setGuiActive( bool activate )
{
}

void RelationEditor::slotCurrentChanged(  const QModelIndex &curr, const QModelIndex & )
{
    kDebug()<<curr.row()<<","<<curr.column();
    slotEnableActions();
}

void RelationEditor::slotSelectionChanged( const QModelIndexList list)
{
    kDebug()<<list.count();
    slotEnableActions();
}

Relation *RelationEditor::currentRelation() const
{
    kDebug();
    return m_view->currentRelation();
}

void RelationEditor::slotContextMenuRequested( const QModelIndex& index, const QPoint& pos )
{
    Relation *rel = m_view->model()->relation( index );
    if ( rel == 0 ) {
        return;
    }
    emit requestPopupMenu( "relation_popup", pos );
}

void RelationEditor::slotHeaderContextMenuRequested( const QPoint &pos )
{
    kDebug();
    QList<QAction*> lst = contextActionList();
    if ( ! lst.isEmpty() ) {
        QMenu::exec( lst, pos,  lst.first() );
    }
}

void RelationEditor::slotEnableActions()
{
    updateActionsEnabled( true );
}

void RelationEditor::updateActionsEnabled( bool on )
{
}

void RelationEditor::setupGui()
{
    // Add the context menu actions for the view options
    connect(m_view->actionSplitView(), SIGNAL(triggered(bool) ), SLOT(slotSplitView()));
    addContextAction( m_view->actionSplitView() );

    actionOptions = new KAction(KIcon("configure"), i18n("Configure..."), this);
    connect(actionOptions, SIGNAL(triggered(bool) ), SLOT(slotOptions()));
    addContextAction( actionOptions );
}

void RelationEditor::slotSplitView()
{
    kDebug();
    m_view->setViewSplitMode( ! m_view->isViewSplit() );
}


void RelationEditor::slotOptions()
{
    kDebug();
    bool col0 = false;
    TreeViewBase *v = m_view->slaveView();
    if ( v->isHidden() ) {
        v = m_view->masterView();
        col0 = true;
    }
    ItemViewSettupDialog dlg( v, col0 );
    dlg.exec();
}

void RelationEditor::slotAddRelation()
{
    kDebug();
}

void RelationEditor::edit( QModelIndex i )
{
    if ( i.isValid() ) {
        QModelIndex p = m_view->model()->parent( i );
//        m_view->setExpanded( p );
        m_view->selectionModel()->setCurrentIndex( i, QItemSelectionModel::NoUpdate );
        m_view->edit( i );
    }
}

void RelationEditor::slotDeleteRelation( Relation *r)
{
    emit deleteRelation( r );
}

bool RelationEditor::loadContext( const KoXmlElement &context )
{
    kDebug()<<endl;
    return m_view->loadContext( context );
}

void RelationEditor::saveContext( QDomElement &context ) const
{
   m_view->saveContext( context );
}

} // namespace KPlato

#include "kptrelationeditor.moc"
