/* This file is part of the KDE project
  Copyright (C) 2006 Dag Andersen <kplato@kde.org>

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

#ifndef KPTRESOURCEEDITOR_H
#define KPTRESOURCEEDITOR_H

#include <kptviewbase.h>
#include <kptitemmodelbase.h>

#include <QTreeWidget>

#include "kptcontext.h"

class QPoint;
class QTreeWidgetItem;
class QSplitter;

class KToggleAction;
class KPrinter;

namespace KPlato
{

class View;
class Project;
class Resource;
class ResourceGroup;

class ResourceItemModel : public ItemModelBase
{
    Q_OBJECT
public:
    explicit ResourceItemModel( Part *part, QObject *parent = 0 );
    ~ResourceItemModel();

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

    virtual QMimeData * mimeData( const QModelIndexList & indexes ) const;
    virtual QStringList mimeTypes () const;

    QObject *object( const QModelIndex &index ) const;
        
protected slots:
    void slotResourceChanged( Resource* );
    void slotResourceGroupChanged( ResourceGroup * );

protected:
    QVariant notUsed( const ResourceGroup *res, int role ) const;
    
    QVariant name( const Resource *res, int role ) const;
    QVariant name( const ResourceGroup *res, int role ) const;
    bool setName( Resource *res, const QVariant &value, int role );
    bool setName( ResourceGroup *res, const QVariant &value, int role );
    
    QVariant type( const Resource *res, int role ) const;
    QVariant type( const ResourceGroup *res, int role ) const;
    bool setType( Resource *res, const QVariant &value, int role );
    bool setType( ResourceGroup *res, const QVariant &value, int role );

    QVariant initials( const Resource *res, int role ) const;
    bool setInitials( Resource *res, const QVariant &value, int role );
    
    QVariant email( const Resource *res, int role ) const;
    bool setEmail( Resource *res, const QVariant &value, int role );

    QVariant units( const Resource *res, int role ) const;
    bool setUnits( Resource *res, const QVariant &value, int role );

    QVariant availableFrom( const Resource *res, int role ) const;
    bool setAvailableFrom( Resource *res, const QVariant &value, int role );

    QVariant availableUntil( const Resource *res, int role ) const;
    bool setAvailableUntil( Resource *res, const QVariant &value, int role );

    QVariant normalRate( const Resource *res, int role ) const;
    bool setNormalRate( Resource *res, const QVariant &value, int role );

    QVariant overtimeRate( const Resource *res, int role ) const;
    bool setOvertimeRate( Resource *res, const QVariant &value, int role );

    QVariant fixedCost( const Resource *res, int role ) const;
    bool setFixedCost( Resource *res, const QVariant &value, int role );

};

class ResourceTreeView : public QTreeView
{
    Q_OBJECT
public:
    ResourceTreeView( Part *part, QWidget *parent );

    ResourceItemModel *itemModel() const { return static_cast<ResourceItemModel*>( model() ); }

    void setArrowKeyNavigation( bool on ) { m_arrowKeyNavigation = on; }
    bool arrowKeyNavigation() const { return m_arrowKeyNavigation; }

    Project *project() const { return itemModel()->project(); }
    void setProject( Project *project ) { itemModel()->setProject( project ); }

    QObject *currentObject() const;
    
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
    void keyPressEvent(QKeyEvent *event);
    QItemSelectionModel::SelectionFlags selectionCommand(const QModelIndex &index, const QEvent *event) const;

private:
    bool m_arrowKeyNavigation;
};

class ResourceEditor : public ViewBase
{
    Q_OBJECT
public:
    ResourceEditor( View *view, QWidget *parent );
    virtual void draw( Project &project );
    virtual void draw();

    virtual void updateReadWrite( bool /*readwrite*/ ) {};

    virtual Resource *currentResource() const;
    virtual ResourceGroup *currentResourceGroup() const;
    
signals:
    void requestPopupMenu( const QString&, const QPoint& );
    
public slots:
    virtual void setViewActive( bool activate, KXMLGUIFactory *factory=0  );

protected:
    void setupGui();
    void updateActionsEnabled(  bool resource = false, bool group = false  );

private slots:
    void slotContextMenuRequested( QModelIndex index, const QPoint& pos );
    
    void slotSelectionChanged( const QModelIndexList );
    void slotCurrentChanged( const QModelIndex& );
    void slotEnableActions( bool resource = false, bool group = false );

    void slotAddResource();
    void slotAddGroup();
    void slotDeleteSelection();

private:
    ResourceTreeView *m_editor;

    KAction *actionAddResource;
    KAction *actionAddGroup;
    KAction *actionDeleteSelection;

};

}  //KPlato namespace

#endif
