 /* This file is part of the KDE project
   Copyright (C) 2005 - 2007 Dag Andersen <kplato@kde.org>

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

#ifndef KPTRESOURCEAPPOINTMENTSMODEL_H
#define KPTRESOURCEAPPOINTMENTSMODEL_H

#include "kplatomodels_export.h"

#include <kptitemmodelbase.h>
#include "kpteffortcostmap.h"

class QDropEvent;
class QDragMoveEvent;
class QDragEnterEvent;


namespace KPlato
{

class View;
class Project;
class Node;
class Appointment;
class AppointmentInterval;
class Resource;
class ResourceGroup;
class ScheduleManager;
class Calendar;

/**
    The ResourceAppointmentsItemModel organizes appointments
    as hours booked per day (or week, month).
    It handles both internal and external appointments.
*/
class KPLATOMODELS_EXPORT ResourceAppointmentsItemModel : public ItemModelBase
{
    Q_OBJECT
public:
    explicit ResourceAppointmentsItemModel( QObject *parent = 0 );
    ~ResourceAppointmentsItemModel();

    virtual void setProject( Project *project );
    virtual void setScheduleManager( ScheduleManager *sm );
    long id() const;

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
    virtual bool dropMimeData( const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent );
  

    QObject *object( const QModelIndex &index ) const;
    Node *node( const QModelIndex &index ) const;
    Appointment *appointment( const QModelIndex &index ) const;
    QModelIndex createAppointmentIndex( int row, int col, void *ptr ) const;
    Appointment *externalAppointment( const QModelIndex &index ) const;
    QModelIndex createExternalAppointmentIndex( int row, int col, void *ptr ) const;
    Resource *resource( const QModelIndex &index ) const;
    QModelIndex createResourceIndex( int row, int col, void *ptr ) const;
    ResourceGroup *resourcegroup( const QModelIndex &index ) const;
    QModelIndex createGroupIndex( int row, int col, void *ptr ) const;

    void refresh();
    void refreshData();

    QDate startDate() const;
    QDate endDate() const;

    Resource *parent( const Appointment *a ) const;
    int rowNumber( Resource *res, Appointment *a ) const;
    void setShowInternalAppointments( bool show );
    bool showInternalAppointments() const { return m_showInternal; }
    void setShowExternalAppointments( bool show );
    bool showExternalAppointments() const { return m_showExternal; }

signals:
    void refreshed();
    void appointmentInserted( Resource*, Appointment* );
    
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
    void slotProjectCalculated( ScheduleManager *sm );
    
    void slotAppointmentToBeInserted( Resource *r, int row );
    void slotAppointmentInserted( Resource*, Appointment* );
    void slotAppointmentToBeRemoved( Resource *r, int row );
    void slotAppointmentRemoved();
    void slotAppointmentChanged( Resource *r, Appointment *a );
    
protected:
    QVariant notUsed( const ResourceGroup *res, int role ) const;
    
    QVariant name( const Resource *res, int role ) const;
    QVariant name( const ResourceGroup *res, int role ) const;
    QVariant name( const Node *node, int role ) const;
    QVariant name( const Appointment *appointment, int role ) const;
    
    QVariant total( const Resource *res, int role ) const;
    QVariant total( const Resource *res, const QDate &date, int role ) const;
    QVariant total( const Appointment *a, int role ) const;
    
    QVariant assignment( const Appointment *a, const QDate &date, int role ) const;
    
private:
    int m_columnCount;
    QMap<const Appointment*, EffortCostMap> m_effortMap;
    QMap<const Appointment*, EffortCostMap> m_externalEffortMap;
    QDate m_start;
    QDate m_end;
    
    ResourceGroup *m_group; // Used for sanity checks
    Resource *m_resource; // Used for sanity checks
    
    ScheduleManager *m_manager;
    bool m_showInternal;
    bool m_showExternal;
};

/**
    The ResourceAppointmentsRowModel returns each appointment interval as a new row.
*/
class KPLATOMODELS_EXPORT ResourceAppointmentsRowModel : public ItemModelBase
{
    Q_OBJECT
    Q_ENUMS( Properties )
public:
    enum Properties {
        Name = 0,
        Type,
        StartTime,
        EndTime,
        Load
    };
    const QMetaEnum columnMap() const;

    explicit ResourceAppointmentsRowModel( QObject *parent = 0 );
    ~ResourceAppointmentsRowModel();

    virtual void setProject( Project *project );
    virtual void setScheduleManager( ScheduleManager *sm );
    long id() const;

    virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const; 
    virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const; 

    QModelIndex parent( const QModelIndex &idx = QModelIndex() ) const;
    QModelIndex index(  int row, int column, const QModelIndex &parent = QModelIndex() ) const;

    virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const; 
    virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    /// If @p index is a resource, return it's parent group, else 0
    ResourceGroup *parentGroup( const QModelIndex &index ) const;
    /// If @p idx is a resource group, return it, else 0
    ResourceGroup *resourcegroup( const QModelIndex &idx ) const;
    /// If @p idx is an appointment, return it's parent resource, else 0
    Resource *parentResource( const QModelIndex &idx ) const;
    /// If @p idx is a resource, return it, else 0
    Resource *resource( const QModelIndex &idx ) const;
    /// If @p idx is an appointment interval, return it's parent appointment, else 0
    Appointment *parentAppointment( const QModelIndex &idx ) const;
    /// If @p idx is an appointment, return it, else 0
    Appointment *appointment( const QModelIndex &idx ) const;
    /// If @p idx is an appointment interval, return it, else 0
    AppointmentInterval *interval( const QModelIndex &idx ) const;

    QModelIndex index( ResourceGroup *g ) const;
    QModelIndex index( Resource *r ) const;
    QModelIndex index( Appointment *a ) const;

    /// If @p idx is an appointment, return the node, else 0
    Node *node( const QModelIndex &idx ) const;
    
    class Private;

protected slots:
    void slotResourceToBeInserted( const ResourceGroup *group, int row );
    void slotResourceInserted( const Resource *r );
    void slotResourceToBeRemoved( const Resource *r );
    void slotResourceRemoved( const Resource *resource );
    void slotResourceGroupToBeInserted( const ResourceGroup *group, int row );
    void slotResourceGroupInserted( const ResourceGroup* );
    void slotResourceGroupToBeRemoved( const ResourceGroup *group );
    void slotResourceGroupRemoved( const ResourceGroup *group );
    void slotAppointmentToBeInserted( Resource *r, int row );
    void slotAppointmentInserted( Resource *r, Appointment *a );
    void slotAppointmentToBeRemoved( Resource *r, int row );
    void slotAppointmentRemoved();
    void slotAppointmentChanged( Resource *r, Appointment *a );
    void slotProjectCalculated( ScheduleManager *sm );

protected:
    QModelIndex createGroupIndex( int row, int column, Project *project );
    QModelIndex createResourceIndex( int row, int column, ResourceGroup *g );
    QModelIndex createAppointmentIndex( int row, int column, Resource *r );
    QModelIndex createIntervalIndex( int row, int column, Appointment *a );

    Private *find( void *ptr ) const;

protected:
    ScheduleManager *m_manager;
    QMap<void*, Private*> m_datamap;
};

/**
    The ResourceAppointmentsGanttModel specialized for use by KDGantt
*/
class KPLATOMODELS_EXPORT ResourceAppointmentsGanttModel : public ResourceAppointmentsRowModel
{
    Q_OBJECT
public:
    explicit ResourceAppointmentsGanttModel( QObject *parent = 0 );
    ~ResourceAppointmentsGanttModel();

    virtual QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;

protected:
    QVariant data( const ResourceGroup *g, int column, int role = Qt::DisplayRole ) const; 
    QVariant data( const Resource *r, int column, int role = Qt::DisplayRole ) const; 
    QVariant data( const Appointment *a, int column, int role = Qt::DisplayRole ) const; 
    QVariant data( const AppointmentInterval *a, int column, int role = Qt::DisplayRole ) const;
};

}  //KPlato namespace

#endif // KPTRESOURCEAPPOINTMENTSMODEL_H
