/* This file is part of the KDE project
  Copyright (C) 2007 Dag Andersen <kplato@kde.org>

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

#include "kptaccountseditor.h"

#include "kptcommand.h"
#include "kptcalendar.h"
#include "kptduration.h"
#include "kptnode.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptaccount.h"
#include "kptdatetime.h"

#include <KoDocument.h>

#include <QList>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QContextMenuEvent>
#include <QDragMoveEvent>

#include <kaction.h>
#include <kicon.h>
#include <kglobal.h>
#include <klocale.h>
#include <kactioncollection.h>

#include <kdebug.h>

namespace KPlato
{


//--------------------
AccountTreeView::AccountTreeView( QWidget *parent )
    : TreeViewBase( parent )
{
    header()->setContextMenuPolicy( Qt::CustomContextMenu );
    setModel( new AccountItemModel() );
    setSelectionModel( new QItemSelectionModel( model() ) );
    setSelectionMode( QAbstractItemView::ExtendedSelection );
    setSelectionBehavior( QAbstractItemView::SelectRows );

    setAcceptDrops( false );
    setDropIndicatorShown( false );
    
    connect( header(), SIGNAL( customContextMenuRequested ( const QPoint& ) ), this, SLOT( headerContextMenuRequested( const QPoint& ) ) );
}

void AccountTreeView::headerContextMenuRequested( const QPoint &pos )
{
    kDebug()<<header()->logicalIndexAt(pos)<<" at"<<pos;
}

void AccountTreeView::contextMenuEvent ( QContextMenuEvent *event )
{
    kDebug();
    emit contextMenuRequested( indexAt(event->pos()), event->globalPos() );
}

void AccountTreeView::selectionChanged( const QItemSelection &sel, const QItemSelection &desel )
{
    kDebug()<<sel.indexes().count();
    foreach( const QModelIndex &i, selectionModel()->selectedIndexes() ) {
        kDebug()<<i.row()<<","<<i.column();
    }
    QTreeView::selectionChanged( sel, desel );
    emit selectionChanged( selectionModel()->selectedIndexes() );
}

void AccountTreeView::currentChanged( const QModelIndex & current, const QModelIndex & previous )
{
    kDebug();
    QTreeView::currentChanged( current, previous );
    emit currentChanged( current );
}

Account *AccountTreeView::currentAccount() const
{
    return model()->account( currentIndex() );
}

Account *AccountTreeView::selectedAccount() const
{
    QModelIndexList lst = selectionModel()->selectedRows();
    if ( lst.count() == 1 ) {
        return model()->account( lst.first() );
    }
    return 0;
}

QList<Account*> AccountTreeView::selectedAccounts() const
{
    QList<Account*> lst;
    foreach ( const QModelIndex &i, selectionModel()->selectedRows() ) {
        Account *a = model()->account( i );
        if ( a ) {
            lst << a;
        }
    }
    return lst;
}


//-----------------------------------
AccountsEditor::AccountsEditor( KoDocument *part, QWidget *parent )
    : ViewBase( part, parent )
{
    setupGui();
    
    QVBoxLayout * l = new QVBoxLayout( this );
    l->setMargin( 0 );
    m_view = new AccountTreeView( this );
    l->addWidget( m_view );
    m_view->setEditTriggers( m_view->editTriggers() | QAbstractItemView::EditKeyPressed );

    connect( model(), SIGNAL( executeCommand( QUndoCommand* ) ), part, SLOT( addCommand( QUndoCommand* ) ) );
    
    connect( m_view, SIGNAL( currentChanged( QModelIndex ) ), this, SLOT( slotCurrentChanged( QModelIndex ) ) );

    connect( m_view, SIGNAL( selectionChanged( const QModelIndexList ) ), this, SLOT( slotSelectionChanged( const QModelIndexList ) ) );
    
    connect( m_view, SIGNAL( contextMenuRequested( QModelIndex, const QPoint& ) ), this, SLOT( slotContextMenuRequested( QModelIndex, const QPoint& ) ) );
}

void AccountsEditor::updateReadWrite( bool readwrite )
{
    m_view->setReadWrite( readwrite );
}

void AccountsEditor::draw( Project &project )
{
    m_view->setProject( &project );
}

void AccountsEditor::draw()
{
}

void AccountsEditor::setGuiActive( bool activate )
{
    kDebug()<<activate;
    updateActionsEnabled( true );
    ViewBase::setGuiActive( activate );
    if ( activate ) {
        if ( !m_view->currentIndex().isValid() ) {
            m_view->selectionModel()->setCurrentIndex(m_view->model()->index( 0, 0 ), QItemSelectionModel::NoUpdate);
        }
        slotSelectionChanged( m_view->selectionModel()->selectedRows() );
    }
}

void AccountsEditor::slotContextMenuRequested( QModelIndex index, const QPoint& pos )
{
    kDebug()<<index.row()<<","<<index.column()<<":"<<pos;
    QString name;
    if ( index.isValid() ) {
        Account *a = m_view->model()->account( index );
        if ( a ) {
            name = "accountseditor_accounts_popup";
        } else {
            name = "accountseditor_popup";
        }
    }
    kDebug()<<name;
    emit requestPopupMenu( name, pos );
}

Account *AccountsEditor::currentAccount() const
{
    return m_view->currentAccount();
}

void AccountsEditor::slotCurrentChanged(  const QModelIndex &curr )
{
    kDebug()<<curr.row()<<","<<curr.column();
    //slotEnableActions( curr.isValid() );
}

void AccountsEditor::slotSelectionChanged( const QModelIndexList list)
{
    kDebug()<<list.count();
    updateActionsEnabled( true );
}

void AccountsEditor::slotEnableActions( bool on )
{
    updateActionsEnabled( on );
}

void AccountsEditor::updateActionsEnabled(  bool on )
{
    QList<Account*> lst = m_view->selectedAccounts();
    bool one = lst.count() == 1;
    bool more = lst.count() > 1;
    actionAddAccount->setEnabled( on && !more );
    actionAddSubAccount->setEnabled( on && one );
    actionDeleteSelection->setEnabled( on && ( one || more ) );
}

void AccountsEditor::setupGui()
{
    QString name = "accountseditor_edit_list";
    
    actionAddAccount  = new KAction(KIcon( "document-new" ), i18n("Add Account"), this);
    actionCollection()->addAction("add_account", actionAddAccount );
    actionAddAccount->setShortcut( KShortcut( Qt::CTRL + Qt::Key_I ) );
    connect( actionAddAccount, SIGNAL( triggered( bool ) ), SLOT( slotAddAccount() ) );
    addAction( name, actionAddAccount );
    
    actionAddSubAccount  = new KAction(KIcon( "document-new" ), i18n("Add Subaccount"), this);
    actionCollection()->addAction("add_subaccount", actionAddSubAccount );
    actionAddSubAccount->setShortcut( KShortcut( Qt::SHIFT + Qt::CTRL + Qt::Key_I ) );
    connect( actionAddSubAccount, SIGNAL( triggered( bool ) ), SLOT( slotAddSubAccount() ) );
    addAction( name, actionAddSubAccount );
    
    actionDeleteSelection  = new KAction(KIcon( "edit-delete" ), i18n("Delete Selected Accounts"), this);
    actionCollection()->addAction("delete_selection", actionDeleteSelection );
    actionDeleteSelection->setShortcut( KShortcut( Qt::Key_Delete ) );
    connect( actionDeleteSelection, SIGNAL( triggered( bool ) ), SLOT( slotDeleteSelection() ) );
    addAction( name, actionDeleteSelection );
    
}

void AccountsEditor::slotAddAccount()
{
    kDebug();
    Account *parent = m_view->selectedAccount(); // sibling
    if ( parent ) {
        parent = parent->parent();
    }
    insertAccount( new Account(), parent );
}

void AccountsEditor::slotAddSubAccount()
{
    kDebug();
    insertAccount( new Account(), m_view->selectedAccount() );
}

void AccountsEditor::insertAccount( Account *account, Account *parent )
{
    QModelIndex i = m_view->model()->insertAccount( account, parent );
    if ( i.isValid() ) {
        QModelIndex p = m_view->model()->parent( i );
        if (parent) kDebug()<<" parent="<<parent->name()<<":"<<p.row()<<","<<p.column();
        kDebug()<<i.row()<<","<<i.column();
        if ( p.isValid() ) {
            m_view->setExpanded( p, true );
        }
        m_view->selectionModel()->select( i, QItemSelectionModel::Rows | QItemSelectionModel::ClearAndSelect );
        m_view->selectionModel()->setCurrentIndex( i, QItemSelectionModel::NoUpdate );
        m_view->edit( i );
    }
}

void AccountsEditor::slotDeleteSelection()
{
    kDebug();
    m_view->model()->removeAccounts( m_view->selectedAccounts() );
}

void AccountsEditor::slotAccountsOk()
{
     kDebug()<<"Account Editor : slotAccountsOk";
     //QModelList
     

     //QModelIndex i = m_view->model()->insertGroup( g );
     
}

KoPrintJob *AccountsEditor::createPrintJob()
{
    return m_view->createPrintJob( this );
}

} // namespace KPlato

#include "kptaccountseditor.moc"
