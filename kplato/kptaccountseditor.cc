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
    : ItemModelBase( part, parent ),
    m_account( 0 )
{
}

AccountItemModel::~AccountItemModel()
{
}

void AccountItemModel::slotAccountToBeInserted( const Account *account, int row )
{
    //kDebug()<<k_funcinfo<<account->name()<<endl;
    Q_ASSERT( m_account == 0 );
    m_account = const_cast<Account*>(account);
    beginInsertRows( index( account->parent() ), row, row );
}

void AccountItemModel::slotAccountInserted( const Account *account )
{
    //kDebug()<<k_funcinfo<<account->name()<<endl;
    Q_ASSERT( account == m_account );
    endInsertRows();
    m_account = 0;
}

void AccountItemModel::slotAccountToBeRemoved( const Account *account )
{
    //kDebug()<<k_funcinfo<<account->name()<<endl;
    Q_ASSERT( m_account == 0 );
    m_account = const_cast<Account*>(account);
    int row = index( account ).row();
    beginRemoveRows( index( account->parent() ), row, row );
}

void AccountItemModel::slotAccountRemoved( const Account *account )
{
    //kDebug()<<k_funcinfo<<account->name()<<endl;
    Q_ASSERT( account == m_account );
    endRemoveRows();
    m_account = 0;
}

void AccountItemModel::setProject( Project *project )
{
    if ( m_project ) {
        Accounts *acc = &( m_project->accounts() );
        disconnect( acc , SIGNAL( changed( Account* ) ), this, SLOT( slotAccountChanged( Account* ) ) );
        
        disconnect( acc, SIGNAL( accountAdded( const Account*, int ) ), this, SLOT( slotAccountInserted( const Account*, int ) ) );
        disconnect( acc, SIGNAL( accountToBeAdded( const Account* ) ), this, SLOT( slotLayoutToBeChanged( const Account* ) ) );
        
        disconnect( acc, SIGNAL( accountRemoved( const Account* ) ), this, SLOT( slotAccountRemoved( const Account* ) ) );
        disconnect( acc, SIGNAL( accountToBeRemoved( const Account* ) ), this, SLOT( slotAccountToBeRemoved( const Account* ) ) );
    }
    m_project = project;
    if ( project ) {
        Accounts *acc = &( project->accounts() );
        kDebug()<<k_funcinfo<<acc<<endl;
        connect( acc, SIGNAL( changed( Account* ) ), this, SLOT( slotAccountChanged( Account* ) ) );
        
        connect( acc, SIGNAL( accountAdded( const Account* ) ), this, SLOT( slotLayoutChanged() ) );
        connect( acc, SIGNAL( accountToBeAdded( const Account* ) ), this, SLOT( slotLayoutToBeChanged() ) );
        
        connect( acc, SIGNAL( accountRemoved( const Account* ) ), this, SLOT( slotAccountRemoved( const Account* ) ) );
        connect( acc, SIGNAL( accountToBeRemoved( const Account* ) ), this, SLOT( slotAccountToBeRemoved( const Account* ) ) );
    }
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
    if ( account ( index ) ) {
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
        a = par->parent();
        int row = -1;
        if ( a ) {
            row = a->accountList().indexOf( par );
        } else {
            row = m_project->accounts().accountList().indexOf( par );
        }
        //kDebug()<<k_funcinfo<<par->name()<<": "<<row<<endl;
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

QModelIndex AccountItemModel::index( const Account *account ) const
{
    Account *a = const_cast<Account*>(account);
    if ( m_project == 0 || account == 0 ) {
        return QModelIndex();
    }
    int row = -1;
    Account *par = a->parent();
    if ( par == 0 ) {
         row = m_project->accounts().accountList().indexOf( a );
    } else {
        row = par->accountList().indexOf( a );
    }
    if ( row == -1 ) {
        return QModelIndex();
    }
    return createIndex( row, 0, a );

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
            if ( value.toString() != a->name() ) {
                m_part->addCommand( new RenameAccountCmd( m_part, a, value.toString(), "Modify account name" ) );
            }
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
            if ( value.toString() != a->description() ) {
                m_part->addCommand( new ModifyAccountDescriptionCmd( m_part, a, value.toString(), "Modify account description" ) );
            }
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
        if ( role == Qt::DisplayRole && result.type() == QVariant::String && result.toString().isEmpty()) {
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
    if ( par ) {
        int row = par->accountList().indexOf( account );
        emit dataChanged( createIndex( row, 0, account ), createIndex( row, columnCount() - 1, account ) );
    } else {
        int row = m_project->accounts().accountList().indexOf( account );
        emit dataChanged( createIndex( row, 0, account ), createIndex( row, columnCount() - 1, account ) );
    }
}

QModelIndex AccountItemModel::insertAccount( Account *account, Account *parent )
{
    kDebug()<<k_funcinfo<<endl;
    if ( account->name().isEmpty() || m_project->accounts().findAccount( account->name() ) ) {
        QString s = parent == 0 ? account->name() : parent->name();
        account->setName( m_project->accounts().uniqueId( s ) );
        //m_project->accounts().insertId( account );
    }
    m_part->addCommand( new AddAccountCmd( m_part, *m_project, account, parent, i18n( "Add account" ) ) );
    int row = -1;
    if ( parent ) {
        row = parent->accountList().indexOf( account );
    } else {
        row = m_project->accounts().accountList().indexOf( account );
    }
    if ( row != -1 ) {
        //kDebug()<<k_funcinfo<<"Inserted: "<<account->name()<<endl;
        return createIndex( row, 0, account );
    }
    kDebug()<<k_funcinfo<<"Can't find "<<account->name()<<endl;
    return QModelIndex();
}

void AccountItemModel::removeAccounts( QList<Account*> lst )
{
    KMacroCommand *cmd = 0;
    QString c = lst.count() > 1 ? i18n( "Delete Accounts" ) : i18n( "Delete Account" );
    while ( ! lst.isEmpty() ) {
        bool del = true;
        Account *acc = lst.takeFirst();
        foreach ( Account *a, lst ) {
            if ( acc->isChildOf( a ) ) {
                del = false; // acc will be deleted when a is deleted
                break;
            }
        }
        if ( del ) {
            if ( cmd == 0 ) cmd = new KMacroCommand( c );
            cmd->addCommand( new RemoveAccountCmd( m_part, *m_project, acc ) );
        }
    }
    if ( cmd )
        m_part->addCommand( cmd );
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

Account *AccountTreeView::selectedAccount() const
{
    QModelIndexList lst = selectionModel()->selectedRows();
    if ( lst.count() == 1 ) {
        return itemModel()->account( lst.first() );
    }
    return 0;
}

QList<Account*> AccountTreeView::selectedAccounts() const
{
    QList<Account*> lst;
    foreach ( QModelIndex i, selectionModel()->selectedRows() ) {
        Account *a = itemModel()->account( i );
        if ( a ) {
            lst << a;
        }
    }
    return lst;
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
    Account *parent = m_view->selectedAccount(); // sibling
    if ( parent ) {
        parent = parent->parent();
    }
    insertAccount( new Account(), parent );
}

void AccountsEditor::slotAddSubAccount()
{
    kDebug()<<k_funcinfo<<endl;
    insertAccount( new Account(), m_view->selectedAccount() );
}

void AccountsEditor::insertAccount( Account *account, Account *parent )
{
    QModelIndex i = m_view->itemModel()->insertAccount( account, parent );
    if ( i.isValid() ) {
        QModelIndex p = m_view->model()->parent( i );
        if (parent) kDebug()<<k_funcinfo<<" parent="<<parent->name()<<": "<<p.row()<<", "<<p.column()<<endl;
        kDebug()<<k_funcinfo<<i.row()<<", "<<i.column()<<endl;
        m_view->setExpanded( p, true );
        m_view->setCurrentIndex( i );
        m_view->edit( i );
    }
}

void AccountsEditor::slotDeleteSelection()
{
    kDebug()<<k_funcinfo<<endl;
    m_view->itemModel()->removeAccounts( m_view->selectedAccounts() );
}


} // namespace KPlato

#include "kptaccountseditor.moc"
