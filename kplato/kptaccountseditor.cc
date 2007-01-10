/* This file is part of the KDE project
  Copyright (C) 2007 Dag Andersen kplato@kde.org>

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

#include "kptaccountseditor.h"

#include "kptcommand.h"
#include "kptitemmodelbase.h"
#include "kptcalendar.h"
#include "kptduration.h"
#include "kptfactory.h"
#include "kptview.h"
#include "kptnode.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptaccount.h"
#include "kptdatetime.h"
#include "kptcontext.h"

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


#include <kicon.h>
#include <kglobal.h>
#include <klocale.h>
#include <kprinter.h>
#include <kactioncollection.h>
#include <kxmlguifactory.h>

#include <kabc/addressee.h>
#include <kabc/vcardconverter.h>

#include <kdebug.h>

namespace KPlato
{

AccountItemModel::AccountItemModel( Part *part, QObject *parent )
    : ItemModelBase( part, parent )
{
}

AccountItemModel::~AccountItemModel()
{
}

void AccountItemModel::setProject( Project *project )
{
    if ( m_project ) {
/*        disconnect( m_project, SIGNAL( accountsChanged( Account* ) ), this, SLOT( slotAccountChanged( Account* ) ) );
        disconnect( m_project, SIGNAL( accountsGroupChanged( AccountGroup* ) ), this, SLOT( slotAccountGroupChanged( Account* ) ) );
        
        disconnect( m_project, SIGNAL( accountsGroupToBeAdded( const AccountGroup* ) ), this, SLOT( slotLayoutToBeChanged() ) );*/
    }
    m_project = project;
//     connect( m_project, SIGNAL( accountsChanged( Account* ) ), this, SLOT( slotAccountChanged( Account* ) ) );
//     connect( m_project, SIGNAL( accountsGroupChanged( AccountGroup* ) ), this, SLOT( slotAccountGroupChanged( Account* ) ) );

}

Qt::ItemFlags AccountItemModel::flags( const QModelIndex &index ) const
{
    Qt::ItemFlags flags = ItemModelBase::flags( index );
    if ( !m_readWrite ) {
        return flags &= ~Qt::ItemIsEditable;
    }
    if ( !index.isValid() ) {
        return flags;
    }

    if ( !index.isValid() )
        return flags;
    if ( !m_readWrite ) {
        return flags &= ~Qt::ItemIsEditable;
    }
    if ( static_cast<Account*>( account ( index ) ) ) {
        switch ( index.column() ) {
            default: flags |= Qt::ItemIsEditable;
        }
    }
    return flags;
}


QModelIndex AccountItemModel::parent( const QModelIndex &index ) const
{
    if ( !index.isValid() || m_project == 0 ) {
        return QModelIndex();
    }
    //kDebug()<<k_funcinfo<<index.internalPointer()<<": "<<index.row()<<", "<<index.column()<<endl;
    Account *a = account( index );
    if ( a == 0 ) {
        return QModelIndex();
    }
    Account *par = a->parent();
    if ( par ) {
        int row = par->accountList().indexOf( a );
        return createIndex( row, 0, par );
    }
    return QModelIndex();
}

bool AccountItemModel::hasChildren( const QModelIndex &parent ) const
{
    //kDebug()<<k_funcinfo<<parent.internalPointer()<<": "<<parent.row()<<", "<<parent.column()<<endl;
    if ( m_project == 0 ) {
        return false;
    }
    Account *par = account( parent );
    if ( par == 0 ) {
        return ! m_project->accounts().accountList().isEmpty();
    }
    return ! par->isElement();
}

QModelIndex AccountItemModel::index( int row, int column, const QModelIndex &parent ) const
{
    if ( m_project == 0 || column < 0 || column >= columnCount() || row < 0 ) {
        return QModelIndex();
    }
    Account *par = account( parent );
    if ( par == 0 ) {
        if ( row < m_project->accounts().accountList().count() ) {
            return createIndex( row, column, m_project->accounts().accountList().at( row ) );
        }
    } else if ( row < par->accountList().count() ) {
        return createIndex( row, column, par->accountList().at( row ) );
    }
    return QModelIndex();
}

int AccountItemModel::columnCount( const QModelIndex &parent ) const
{
    return 2;
}

int AccountItemModel::rowCount( const QModelIndex &parent ) const
{
    if ( m_project == 0 ) {
        return 0;
    }
    Account *par = account( parent );
    if ( par == 0 ) {
        return m_project->accounts().accountList().count();
    }
    return par->accountList().count();
}

bool AccountItemModel::insertRows( int row, int count, const QModelIndex &parent )
{
//TODO
    return false;
}

bool AccountItemModel::removeRows( int row, int count, const QModelIndex &parent )
{
//TODO
    return false;
}

QVariant AccountItemModel::name( const Account *a, int role ) const
{
    //kDebug()<<k_funcinfo<<res->name()<<", "<<role<<endl;
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return a->name();
            break;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool AccountItemModel::setName( Account *a, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            m_part->addCommand( new RenameAccountCmd( m_part, a, value.toString(), "Modify account name" ) );
            return true;
    }
    return false;
}

QVariant AccountItemModel::description( const Account *a, int role ) const
{
    //kDebug()<<k_funcinfo<<res->name()<<", "<<role<<endl;
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return a->description();
            break;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool AccountItemModel::setDescription( Account *a, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            m_part->addCommand( new ModifyAccountDescriptionCmd( m_part, a, value.toString(), "Modify account description" ) );
            return true;
    }
    return false;
}

QVariant AccountItemModel::data( const QModelIndex &index, int role ) const
{
    QVariant result;
    Account *a = account( index );
    if ( a == 0 ) {
        return QVariant();
    }
    switch ( index.column() ) {
        case 0: result = name( a, role ); break;
        case 1: result = description( a, role ); break;
        default:
            kDebug()<<k_funcinfo<<"data: invalid display value column "<<index.column()<<endl;;
            return QVariant();
    }
    if ( result.isValid() ) {
        if ( result.type() == QVariant::String && result.toString().isEmpty()) {
            // HACK to show focus in empty cells
            result = " ";
        }
        return result;
    }
    // define default action

    return QVariant();
}

bool AccountItemModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
    if ( ( flags( index ) &Qt::ItemIsEditable ) == 0 || role != Qt::EditRole ) {
        return false;
    }
    Account *a = account( index );
    switch (index.column()) {
        case 0: return setName( a, value, role );
        case 1: return setDescription( a, value, role );
        default:
            qWarning("data: invalid display value column %d", index.column());
            return false;
    }
    return false;
}

QVariant AccountItemModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal ) {
        if ( role == Qt::DisplayRole ) {
            switch ( section ) {
                case 0: return i18n( "Name" );
                case 1: return i18n( "Description" );
                default: return QVariant();
            }
        } else if ( role == Qt::TextAlignmentRole ) {
            switch (section) {
                case 1: return Qt::AlignLeft;
                default: return QVariant();
            }
        }
    }
    if ( role == Qt::ToolTipRole ) {
        switch ( section ) {
            case 0: return ToolTip::AccountName;
            case 1: return ToolTip::AccountDescription;
            default: return QVariant();
        }
    }
    return ItemModelBase::headerData(section, orientation, role);
}

void AccountItemModel::sort( int column, Qt::SortOrder order )
{
}

Account *AccountItemModel::account( const QModelIndex &index ) const
{
    return static_cast<Account*>( index.internalPointer() );
}

void AccountItemModel::slotAccountChanged( Account *account )
{
    Account *par = account->parent();
    if ( par == 0 ) {
        int row = par->accountList().indexOf( account );
        emit dataChanged( createIndex( row, 0, account ), createIndex( row, columnCount() - 1, account ) );
    } else {
        int row = m_project->accounts().accountList().indexOf( account );
        emit dataChanged( createIndex( row, 0, account ), createIndex( row, columnCount() - 1, account ) );
    }
}

//--------------------
AccountTreeView::AccountTreeView( Part *part, QWidget *parent )
    : TreeViewBase( parent )
{
    header()->setContextMenuPolicy( Qt::CustomContextMenu );
    setModel( new AccountItemModel( part ) );
    setSelectionModel( new QItemSelectionModel( model() ) );
    setSelectionMode( QAbstractItemView::ExtendedSelection );

    setAcceptDrops( false );
    setDropIndicatorShown( false );
    
    connect( header(), SIGNAL( customContextMenuRequested ( const QPoint& ) ), this, SLOT( headerContextMenuRequested( const QPoint& ) ) );
    connect( this, SIGNAL( activated ( const QModelIndex ) ), this, SLOT( slotActivated( const QModelIndex ) ) );


}

void AccountTreeView::slotActivated( const QModelIndex index )
{
    kDebug()<<k_funcinfo<<index.column()<<endl;
}

void AccountTreeView::headerContextMenuRequested( const QPoint &pos )
{
    kDebug()<<k_funcinfo<<header()->logicalIndexAt(pos)<<" at "<<pos<<endl;
}

void AccountTreeView::contextMenuEvent ( QContextMenuEvent *event )
{
    kDebug()<<k_funcinfo<<endl;
    emit contextMenuRequested( indexAt(event->pos()), event->globalPos() );
}

void AccountTreeView::selectionChanged( const QItemSelection &sel, const QItemSelection &desel )
{
    kDebug()<<k_funcinfo<<sel.indexes().count()<<endl;
    foreach( QModelIndex i, selectionModel()->selectedIndexes() ) {
        kDebug()<<k_funcinfo<<i.row()<<", "<<i.column()<<endl;
    }
    QTreeView::selectionChanged( sel, desel );
    emit selectionChanged( selectionModel()->selectedIndexes() );
}

void AccountTreeView::currentChanged( const QModelIndex & current, const QModelIndex & previous )
{
    kDebug()<<k_funcinfo<<endl;
    QTreeView::currentChanged( current, previous );
    emit currentChanged( current );
}

Account *AccountTreeView::currentAccount() const
{
    return itemModel()->account( currentIndex() );
}


//-----------------------------------
AccountsEditor::AccountsEditor( Part *part, QWidget *parent )
    : ViewBase( part, parent )
{
    setupGui();
    
    QVBoxLayout * l = new QVBoxLayout( this );
    l->setMargin( 0 );
    m_view = new AccountTreeView( part, this );
    l->addWidget( m_view );
    m_view->setEditTriggers( m_view->editTriggers() | QAbstractItemView::EditKeyPressed );

    connect( m_view, SIGNAL( currentChanged( QModelIndex ) ), this, SLOT( slotCurrentChanged( QModelIndex ) ) );

    connect( m_view, SIGNAL( selectionChanged( const QModelIndexList ) ), this, SLOT( slotSelectionChanged( const QModelIndexList ) ) );
    
    connect( m_view, SIGNAL( contextMenuRequested( QModelIndex, const QPoint& ) ), this, SLOT( slotContextMenuRequested( QModelIndex, const QPoint& ) ) );
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
    kDebug()<<k_funcinfo<<activate<<endl;
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
    kDebug()<<k_funcinfo<<index.row()<<", "<<index.column()<<": "<<pos<<endl;
    QString name;
    if ( index.isValid() ) {
        Account *a = m_view->itemModel()->account( index );
        if ( a ) {
            name = "accountseditor_accounts_popup";
        } else {
            name = "accountseditor_popup";
        }
    }
    kDebug()<<k_funcinfo<<name<<endl;
    emit requestPopupMenu( name, pos );
}

Account *AccountsEditor::currentAccount() const
{
    return m_view->currentAccount();
}

void AccountsEditor::slotCurrentChanged(  const QModelIndex &curr )
{
    kDebug()<<k_funcinfo<<curr.row()<<", "<<curr.column()<<endl;
    //slotEnableActions( curr.isValid() );
}

void AccountsEditor::slotSelectionChanged( const QModelIndexList list)
{
    kDebug()<<k_funcinfo<<list.count()<<endl;
    foreach ( QModelIndex i, list ) {
        Account *a = m_view->itemModel()->account( i );
        if ( a ) {
            slotEnableActions( true );
            return;
        }
    }
    slotEnableActions( false );
}

void AccountsEditor::slotEnableActions( bool on )
{
    updateActionsEnabled( on );
}

void AccountsEditor::updateActionsEnabled(  bool on )
{
    actionAddAccount->setEnabled( true );
    actionAddSubAccount->setEnabled( on );
    actionDeleteSelection->setEnabled( on );
}

void AccountsEditor::setupGui()
{
    KActionCollection *coll = actionCollection();
    QString name = "accountseditor_edit_list";
    
    actionAddSubAccount  = new KAction(KIcon( "filenew" ), i18n("Add Subaccount"), this);
    actionCollection()->addAction("add_subaccount", actionAddSubAccount );
    actionAddSubAccount->setShortcut( KShortcut( Qt::SHIFT + Qt::CTRL + Qt::Key_I ) );
    connect( actionAddSubAccount, SIGNAL( triggered( bool ) ), SLOT( slotAddSubAccount() ) );
    addAction( name, actionAddSubAccount );
    
    actionAddAccount  = new KAction(KIcon( "filenew" ), i18n("Add Account"), this);
    actionCollection()->addAction("add_account", actionAddAccount );
    actionAddAccount->setShortcut( KShortcut( Qt::CTRL + Qt::Key_I ) );
    connect( actionAddAccount, SIGNAL( triggered( bool ) ), SLOT( slotAddAccount() ) );
    addAction( name, actionAddAccount );
    
    actionDeleteSelection  = new KAction(KIcon( "editdelete" ), i18n("Delete Selected Accounts"), this);
    actionCollection()->addAction("delete_selection", actionDeleteSelection );
    actionDeleteSelection->setShortcut( KShortcut( Qt::Key_Delete ) );
    connect( actionDeleteSelection, SIGNAL( triggered( bool ) ), SLOT( slotDeleteSelection() ) );
    addAction( name, actionDeleteSelection );
    
}

void AccountsEditor::slotAddAccount()
{
    kDebug()<<k_funcinfo<<endl;
    emit addAccount( currentAccount() );
}

void AccountsEditor::slotAddSubAccount()
{
    kDebug()<<k_funcinfo<<endl;
    emit addAccount( currentAccount() );
}

void AccountsEditor::slotDeleteSelection()
{
    kDebug()<<k_funcinfo<<endl;
    /*    emit deleteTaskList( selectedAccounts() );*/
}


} // namespace KPlato

#include "kptaccountseditor.moc"
