/* This file is part of the KDE project
  Copyright (C) 2006 - 2007 Dag Andersen kplato@kde.org>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation;
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

#include "kptdocumentseditor.h"

#include "kptcommand.h"
#include "kptitemmodelbase.h"
#include "kptduration.h"
#include "kptnode.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptdocuments.h"
#include "kptdatetime.h"
#include "kptcontext.h"
#include "kptitemviewsettup.h"

#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QMenu>
#include <QPainter>
#include <QStyle>
#include <QList>
#include <QHeaderView>
#include <QObject>
#include <QTreeWidget>
#include <QStringList>
#include <QVBoxLayout>


#include <kaction.h>
#include <kicon.h>
#include <kglobal.h>
#include <klocale.h>
#include <kactioncollection.h>

#include <kdebug.h>

#include <KoDocument.h>

namespace KPlato
{


//--------------------
    DocumentTreeView::DocumentTreeView( KoDocument *part, QWidget *parent )
    : DoubleTreeViewBase( parent )
{
//    header()->setContextMenuPolicy( Qt::CustomContextMenu );
    setStretchLastSection( false );
    
    setModel( new DocumentItemModel( part ) );
    
    setSelectionMode( QAbstractItemView::ExtendedSelection );

    setItemDelegateForColumn( 1, new EnumDelegate( this ) );
    setItemDelegateForColumn( 4, new EnumDelegate( this ) );

    setAcceptDrops( true );
    setDropIndicatorShown( true );
    
    QList<int> lst1; lst1 << 1 << -1;
    QList<int> lst2; lst2 << 0;
    hideColumns( lst1, lst2 );
}

void DocumentTreeView::slotActivated( const QModelIndex index )
{
    kDebug()<<index.column();
}

Document *DocumentTreeView::currentDocument() const
{
    return itemModel()->document( selectionModel()->currentIndex() );
}

QList<Document*> DocumentTreeView::selectedDocuments() const
{
    QList<Document*> lst;
    foreach (QModelIndex i, selectionModel()->selectedRows() ) {
        Document *doc = itemModel()->document( i );
        if ( doc ) {
            lst << doc;
        }
    }
    return lst;
}

//-----------------------------------
DocumentsEditor::DocumentsEditor( KoDocument *part, QWidget *parent )
    : ViewBase( part, parent )
{
    setupGui();
    
    QVBoxLayout * l = new QVBoxLayout( this );
    l->setMargin( 0 );
    m_view = new DocumentTreeView( part, this );
    l->addWidget( m_view );
    
    m_view->setEditTriggers( m_view->editTriggers() | QAbstractItemView::EditKeyPressed );

    connect( m_view, SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ), this, SLOT( slotCurrentChanged( const QModelIndex & ) ) );

    connect( m_view, SIGNAL( selectionChanged( const QModelIndexList ) ), this, SLOT( slotSelectionChanged( const QModelIndexList ) ) );

    connect( m_view, SIGNAL( contextMenuRequested( QModelIndex, const QPoint& ) ), this, SLOT( slotContextMenuRequested( QModelIndex, const QPoint& ) ) );
    
    connect( m_view, SIGNAL( headerContextMenuRequested( const QPoint& ) ), SLOT( slotHeaderContextMenuRequested( const QPoint& ) ) );

}

void DocumentsEditor::updateReadWrite( bool readwrite )
{
    m_view->setReadWrite( readwrite );
}

void DocumentsEditor::draw( Documents &docs )
{
    m_view->setDocuments( &docs );
}

void DocumentsEditor::draw()
{
}

void DocumentsEditor::setGuiActive( bool activate )
{
    kDebug()<<activate;
    updateActionsEnabled( true );
    ViewBase::setGuiActive( activate );
    if ( activate && !m_view->selectionModel()->currentIndex().isValid() ) {
        m_view->selectionModel()->setCurrentIndex(m_view->model()->index( 0, 0 ), QItemSelectionModel::NoUpdate);
    }
}

void DocumentsEditor::slotContextMenuRequested( QModelIndex index, const QPoint& pos )
{
    //kDebug()<<index.row()<<","<<index.column()<<":"<<pos;
    QString name;
    if ( index.isValid() ) {
        Document *obj = m_view->itemModel()->document( index );
        if ( obj ) {
            name = "documentseditor_popup";
        }
    }
    emit requestPopupMenu( name, pos );
}

void DocumentsEditor::slotHeaderContextMenuRequested( const QPoint &pos )
{
    kDebug();
    QList<QAction*> lst = contextActionList();
    if ( ! lst.isEmpty() ) {
        QMenu::exec( lst, pos,  lst.first() );
    }
}

Document *DocumentsEditor::currentDocument() const
{
    return m_view->currentDocument();
}

void DocumentsEditor::slotCurrentChanged(  const QModelIndex & )
{
    //kDebug()<<curr.row()<<","<<curr.column();
//    slotEnableActions();
}

void DocumentsEditor::slotSelectionChanged( const QModelIndexList )
{
    //kDebug()<<list.count();
    updateActionsEnabled();
}

void DocumentsEditor::slotEnableActions( bool on )
{
    updateActionsEnabled( on );
}

void DocumentsEditor::updateActionsEnabled(  bool on )
{
}

void DocumentsEditor::setupGui()
{
    QString name = "documentseditor_edit_list";
    actionAddDocument  = new KAction(KIcon( "document-new" ), i18n("Add Document..."), this);
    actionCollection()->addAction("add_documents", actionAddDocument );
    actionAddDocument->setShortcut( KShortcut( Qt::CTRL + Qt::SHIFT + Qt::Key_I ) );
    
    connect( actionAddDocument, SIGNAL( triggered( bool ) ), SLOT( slotAddDocument() ) );
    addAction( name, actionAddDocument );
    
    actionDeleteSelection  = new KAction(KIcon( "edit-delete" ), i18n("Delete Selected Documents"), this);
    actionCollection()->addAction("delete_selection", actionDeleteSelection );
    actionDeleteSelection->setShortcut( KShortcut( Qt::Key_Delete ) );
    connect( actionDeleteSelection, SIGNAL( triggered( bool ) ), SLOT( slotDeleteSelection() ) );
    addAction( name, actionDeleteSelection );
    
    // Add the context menu actions for the view options
    actionOptions = new KAction(KIcon("configure"), i18n("Configure..."), this);
    connect(actionOptions, SIGNAL(triggered(bool) ), SLOT(slotOptions()));
    addContextAction( actionOptions );
}

void DocumentsEditor::slotOptions()
{
    kDebug();
    ItemViewSettupDialog dlg( m_view->slaveView() );
    dlg.exec();
}


void DocumentsEditor::slotAddDocument()
{
    //kDebug();
    QList<Document*> dl = m_view->selectedDocuments();
    Document *after = 0;
    if ( dl.count() > 0 ) {
        after = dl.last();
    }
    Document *doc = new Document();
    QModelIndex i = m_view->itemModel()->insertDocument( doc, after );
    if ( i.isValid() ) {
        m_view->selectionModel()->setCurrentIndex( i, QItemSelectionModel::NoUpdate );
        m_view->edit( i );
    }
}

void DocumentsEditor::slotDeleteSelection()
{
    QList<Document*> lst = m_view->selectedDocuments();
    //kDebug()<<lst.count()<<" objects";
    if ( ! lst.isEmpty() ) {
        emit deleteDocumentList( lst );
    }
}

bool DocumentsEditor::loadContext( const KoXmlElement &context )
{
    kDebug()<<endl;
    return m_view->loadContext( context );
}

void DocumentsEditor::saveContext( QDomElement &context ) const
{
    m_view->saveContext( context );
}


} // namespace KPlato

#include "kptdocumentseditor.moc"
