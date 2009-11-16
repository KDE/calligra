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

#ifndef KPTRESOURCEMODEL_H
#define KPTRESOURCEMODEL_H

#include "kplatomodels_export.h"

#include <kptitemmodelbase.h>

#include <QMetaEnum>

class QPoint;


namespace KPlato
{

class Project;
class Resource;
class ResourceGroup;
class Calendar;

class KPLATOMODELS_EXPORT ResourceModel : public QObject
{
    Q_OBJECT
    Q_ENUMS( Properties )
public:
    explicit ResourceModel( QObject *parent = 0 );
    ~ResourceModel();

    enum Properties {
        ResourceName = 0,
        ResourceType,
        ResourceInitials,
        ResourceEmail,
        ResourceCalendar,
        ResourceLimit,
        ResourceAvailableFrom,
        ResourceAvailableUntil,
        ResourceNormalRate,
        ResourceOvertimeRate
    };
    
    const QMetaEnum columnMap() const;
    void setProject( Project *project );
    int propertyCount() const;
    QVariant data( const Resource *resource, int property, int role = Qt::DisplayRole ) const;
    QVariant data( const ResourceGroup *group, int property, int role = Qt::DisplayRole ) const;
    static QVariant headerData( int section, int role = Qt::DisplayRole );

    QVariant name( const Resource *res, int role ) const;
    QVariant type( const Resource *res, int role ) const;
    QVariant initials( const Resource *res, int role ) const;
    QVariant email( const Resource *res, int role ) const;
    QVariant calendar( const Resource *res, int role ) const;
    QVariant units( const Resource *res, int role ) const;
    QVariant availableFrom( const Resource *res, int role ) const;
    QVariant availableUntil( const Resource *res, int role ) const;
    QVariant normalRate( const Resource *res, int role ) const;
    QVariant overtimeRate( const Resource *res, int role ) const;
    
    QVariant name( const ResourceGroup *res, int role ) const;
    QVariant type( const ResourceGroup *res, int role ) const;

private:
    Project *m_project;
};

class KPLATOMODELS_EXPORT ResourceItemModel : public ItemModelBase
{
    Q_OBJECT
public:
    explicit ResourceItemModel( QObject *parent = 0 );
    ~ResourceItemModel();

    virtual const QMetaEnum columnMap() const { return m_model.columnMap(); }

    virtual void setProject( Project *project );

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

    virtual QStringList mimeTypes () const;
    virtual Qt::DropActions supportedDropActions() const;
    virtual bool dropAllowed( const QModelIndex &index, int dropIndicatorPosition, const QMimeData *data );
    virtual bool dropMimeData( const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent );
    QMimeData *mimeData( const QModelIndexList & indexes ) const;
    
    QAbstractItemDelegate *createDelegate( int col, QWidget *parent ) const;
    
    QObject *object( const QModelIndex &index ) const;
    QModelIndex insertGroup( ResourceGroup *g );
    QModelIndex insertResource( ResourceGroup *g, Resource *r, Resource *after = 0 );

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
    void slotCalendarChanged( Calendar* cal );
    void slotLayoutChanged();

protected:
    QVariant notUsed( const ResourceGroup *res, int role ) const;
    
    QVariant name( const ResourceGroup *res, int role ) const;
    bool setName( Resource *res, const QVariant &value, int role );
    bool setName( ResourceGroup *res, const QVariant &value, int role );
    
    QVariant type( const ResourceGroup *res, int role ) const;
    bool setType( Resource *res, const QVariant &value, int role );
    bool setType( ResourceGroup *res, const QVariant &value, int role );

    bool setInitials( Resource *res, const QVariant &value, int role );
    bool setEmail( Resource *res, const QVariant &value, int role );
    bool setCalendar( Resource *res, const QVariant &value, int role );
    bool setUnits( Resource *res, const QVariant &value, int role );
    bool setAvailableFrom( Resource *res, const QVariant &value, int role );
    bool setAvailableUntil( Resource *res, const QVariant &value, int role );
    bool setNormalRate( Resource *res, const QVariant &value, int role );
    bool setOvertimeRate( Resource *res, const QVariant &value, int role );

    QList<Resource*> resourceList( QDataStream &stream );
    
private:
    ResourceGroup *m_group; // Used for sanity checks
    Resource *m_resource; // Used for sanity checks
    ResourceModel m_model;

};


}  //KPlato namespace

#endif
