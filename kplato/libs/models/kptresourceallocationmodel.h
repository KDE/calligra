/* This file is part of the KDE project
  Copyright (C) 2009 Dag Andersen <kplato@kde.org>

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
  Boston, MA 02110-1301, USA.
*/

#ifndef KPTRESOURCEALLOCATIONMODEL_H
#define KPTRESOURCEALLOCATIONMODEL_H

#include "kplatomodels_export.h"

#include <kptitemmodelbase.h>

#include <QMetaEnum>
#include <QMap>

class QPoint;


namespace KPlato
{

class Project;
class Task;
class Resource;
class ResourceGroup;
class ResourceRequest;
class ResourceGroupRequest;

/**
 The ResourceAllocationModel gives access to resource requests
*/

class KPLATOMODELS_EXPORT ResourceAllocationModel : public QObject
{
    Q_OBJECT
    Q_ENUMS( Properties )
public:
    explicit ResourceAllocationModel( QObject *parent = 0 );
    ~ResourceAllocationModel();

    enum Properties {
        RequestName = 0,
        RequestType,
        RequestAllocation,
        RequestMaximum,
        RequestRequired
    };
    
    const QMetaEnum columnMap() const;
    void setProject( Project *project );
    Task *task() const { return m_task; }
    void setTask( Task *task );
    int propertyCount() const;
    QVariant data( const ResourceGroup *group, const Resource *resource, int property, int role = Qt::DisplayRole ) const;
    QVariant data( const ResourceGroup *group, int property, int role = Qt::DisplayRole ) const;
    static QVariant headerData( int section, int role = Qt::DisplayRole );

    QVariant name( const Resource *res, int role ) const;
    QVariant type( const Resource *res, int role ) const;
    QVariant allocation( const ResourceGroup *group, const Resource *res, int role ) const;
    QVariant maximum( const Resource *res, int role ) const;
    QVariant required( const Resource *res, int role ) const;
    
    QVariant name( const ResourceGroup *res, int role ) const;
    QVariant type( const ResourceGroup *res, int role ) const;
    QVariant allocation( const ResourceGroup *res, int role ) const;
    QVariant maximum( const ResourceGroup *res, int role ) const;

private:
    Project *m_project;
    Task *m_task;
};

/**
 The ResourceAllocationItemModel facilitates viewing and modifying
 resource allocations for a task.
*/

class KPLATOMODELS_EXPORT ResourceAllocationItemModel : public ItemModelBase
{
    Q_OBJECT
public:
    explicit ResourceAllocationItemModel( QObject *parent = 0 );
    ~ResourceAllocationItemModel();

    virtual const QMetaEnum columnMap() const { return m_model.columnMap(); }

    virtual void setProject( Project *project );
    void setTask( Task *task );
    Task *task() const { return m_model.task(); }

    virtual Qt::ItemFlags flags( const QModelIndex & index ) const;

    virtual QModelIndex parent( const QModelIndex & index ) const;
    virtual QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    QModelIndex index( const ResourceGroup *group ) const;
    QModelIndex index( const Resource *resource ) const;

    virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const; 
    virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const; 

    virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const; 
    virtual bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );


    virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    QAbstractItemDelegate *createDelegate( int col, QWidget *parent ) const;
    
    QObject *object( const QModelIndex &index ) const;

    const QMap<const Resource*, ResourceRequest*> &resourceCache() const { return m_resourceCache; }
    const QMap<const ResourceGroup*, ResourceGroupRequest*> &groupCache() const { return m_groupCache; }
    
    Resource *resource( const QModelIndex &idx ) const;
    void setRequired( const QModelIndex &idx, const QList<Resource*> &lst );
    const QList<Resource*> &required( const QModelIndex &idx ) const;
    
protected slots:
    void slotResourceChanged( Resource* );
    void slotResourceGroupChanged( ResourceGroup * );
    void slotResourceGroupToBeInserted( const ResourceGroup *group, int row );
    void slotResourceGroupInserted( const ResourceGroup *group );
    void slotResourceGroupToBeRemoved( const ResourceGroup *group );
    void slotResourceGroupRemoved( const ResourceGroup *group );
    void slotResourceToBeInserted( const ResourceGroup *group, int row );
    void slotResourceInserted( const Resource *resource );
    void slotResourceToBeRemoved( const Resource *resource );
    void slotResourceRemoved( const Resource *resource );
    
protected:
    void filldata( Task *task );

    QVariant notUsed( const ResourceGroup *res, int role ) const;
    
    QVariant allocation( const ResourceGroup *group, const Resource *res, int role ) const;
    QVariant allocation( const ResourceGroup *res, int role ) const;
    bool setAllocation( ResourceGroup *res, const QVariant &value, int role );
    bool setAllocation( Resource *res, const QVariant &value, int role );

    bool setRequired( const QModelIndex &idx, const QVariant &value, int role );
    QVariant required( const QModelIndex &idx, int role ) const;
    
private:
    ResourceAllocationModel m_model;

    QMap<const Resource*, ResourceRequest*> m_resourceCache;
    QMap<const Resource*, int> m_requiredChecked;
    QMap<const ResourceGroup*, ResourceGroupRequest*> m_groupCache;
};


}  //KPlato namespace

#endif
