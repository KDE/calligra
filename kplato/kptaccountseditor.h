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

#ifndef KPTACCOUNTSEDITOR_H
#define KPTACCOUNTSEDITOR_H

#include <kptviewbase.h>
#include <kptitemmodelbase.h>

#include <QTreeWidget>

#include "kptcontext.h"

class QPoint;
class QTreeWidgetItem;
class QSplitter;
class QDropEvent;
class QDragMoveEvent;
class QDragEnterEvent;

class KToggleAction;
class KPrinter;

namespace KPlato
{

class View;
class Project;
class Account;

class AccountItemModel : public ItemModelBase
{
    Q_OBJECT
public:
    explicit AccountItemModel( Part *part, QObject *parent = 0 );
    ~AccountItemModel();

    virtual void setProject( Project *project );

    virtual Qt::ItemFlags flags( const QModelIndex & index ) const;

    virtual QModelIndex parent( const QModelIndex & index ) const;
    virtual bool hasChildren( const QModelIndex & parent = QModelIndex() ) const;
    virtual QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;

    virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const; 
    virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const; 
    virtual bool insertRows( int row, int count, const QModelIndex & parent = QModelIndex() );
    virtual bool removeRows( int row, int count, const QModelIndex & parent = QModelIndex() );

    virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const; 
    virtual bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );


    virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    virtual void sort( int column, Qt::SortOrder order = Qt::AscendingOrder );

    Account *account( const QModelIndex &index ) const;
        
protected slots:
    void slotAccountChanged( Account* );

protected:
    QVariant name( const Account *account, int role ) const;
    bool setName( Account *account, const QVariant &value, int role );
    
    QVariant description( const Account *account, int role ) const;
    bool setDescription( Account *account, const QVariant &value, int role );

};

class AccountTreeView : public TreeViewBase
{
    Q_OBJECT
public:
    AccountTreeView( Part *part, QWidget *parent );

    AccountItemModel *itemModel() const { return static_cast<AccountItemModel*>( model() ); }

    Project *project() const { return itemModel()->project(); }
    void setProject( Project *project ) { itemModel()->setProject( project ); }

    Account *currentAccount() const;
    
signals:
    void currentChanged( const QModelIndex& );
    void currentColumnChanged( QModelIndex, QModelIndex );
    void selectionChanged( const QModelIndexList );

    void contextMenuRequested( QModelIndex, const QPoint& );
    
protected slots:
    void headerContextMenuRequested( const QPoint &pos );
    void slotActivated( const QModelIndex index );
    virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    virtual void currentChanged ( const QModelIndex & current, const QModelIndex & previous );

protected:
    void contextMenuEvent ( QContextMenuEvent * event );
    
};

class AccountsEditor : public ViewBase
{
    Q_OBJECT
public:
    AccountsEditor( Part *part, QWidget *parent );
    
    void setupGui();
    virtual void draw( Project &project );
    virtual void draw();

    virtual void updateReadWrite( bool /*readwrite*/ ) {};

    virtual Account *currentAccount() const;
    
signals:
    void requestPopupMenu( const QString&, const QPoint& );
    void addAccount( Account *account );
    
public slots:
    /// Activate/deactivate the gui
    virtual void setGuiActive( bool activate );

protected:
    void updateActionsEnabled( bool on );

private slots:
    void slotContextMenuRequested( QModelIndex index, const QPoint& pos );
    
    void slotSelectionChanged( const QModelIndexList );
    void slotCurrentChanged( const QModelIndex& );
    void slotEnableActions( bool on );

    void slotAddAccount();
    void slotAddSubAccount();
    void slotDeleteSelection();

private:
    AccountTreeView *m_view;

    KAction *actionAddAccount;
    KAction *actionAddSubAccount;
    KAction *actionDeleteSelection;

};

}  //KPlato namespace

#endif
