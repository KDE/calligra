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

#ifndef KPTSCHEDULEMODEL_H
#define KPTSCHEDULEMODEL_H

#include "kplatomodels_export.h"

#include <kptitemmodelbase.h>

#define ScheduleName 0
#define ScheduleState 1
#define ScheduleDirection 2
#define ScheduleOverbooking 3
#define ScheduleDistribution 4
#define ScheduleCalculate 5
#define SchedulePlannedStart 6
#define SchedulePlannedFinish 7

#define SCHEDULE_PROPERTY_COUNT 8

namespace KPlato
{

class View;
class Project;
class ScheduleManager;
class MainSchedule;
class Schedule;

class ScheduleColumnMap : public ColumnMap
{
    public:
        ScheduleColumnMap();
};


class KPLATOMODELS_EXPORT ScheduleItemModel : public ItemModelBase
{
    Q_OBJECT
public:
    explicit ScheduleItemModel( QObject *parent = 0 );
    ~ScheduleItemModel();

    const ColumnMap &columnNames() const { return columnMap; }
    
    virtual void setProject( Project *project );

    virtual Qt::ItemFlags flags( const QModelIndex & index ) const;

    virtual QModelIndex parent( const QModelIndex & index ) const;
    virtual QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    QModelIndex index( const ScheduleManager *manager ) const;

    virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const; 
    virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const; 

    virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const; 
    virtual bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    QItemDelegate *createDelegate( int column, QWidget *parent ) const;
    
    virtual void sort( int column, Qt::SortOrder order = Qt::AscendingOrder );

    virtual QMimeData * mimeData( const QModelIndexList & indexes ) const;
    virtual QStringList mimeTypes () const;

    ScheduleManager *manager( const QModelIndex &index ) const;
    
protected slots:
    void slotManagerChanged( ScheduleManager *sch );
    void slotScheduleChanged( MainSchedule *sch );

    void slotScheduleManagerToBeInserted( const ScheduleManager *manager, int row );
    void slotScheduleManagerInserted( const ScheduleManager *manager );
    void slotScheduleManagerToBeRemoved( const ScheduleManager *manager );
    void slotScheduleManagerRemoved( const ScheduleManager *manager );
    void slotScheduleToBeInserted( const ScheduleManager *manager, int row );
    void slotScheduleInserted( const MainSchedule *schedule );
    void slotScheduleToBeRemoved( const MainSchedule *schedule );
    void slotScheduleRemoved( const MainSchedule *schedule );

protected:
    int row( const Schedule *sch ) const;
    
    QVariant name( const QModelIndex &index, int role ) const;
    bool setName( const QModelIndex &index, const QVariant &value, int role );
    
    QVariant state( const QModelIndex &index, int role ) const;
    bool setState( const QModelIndex &index, const QVariant &value, int role );

    QVariant allowOverbooking( const QModelIndex &index, int role ) const;
    bool setAllowOverbooking( const QModelIndex &index, const QVariant &value, int role );
    
    QVariant usePert( const QModelIndex &index, int role ) const;
    bool setUsePert( const QModelIndex &index, const QVariant &value, int role );

    QVariant calculateAll( const QModelIndex &index, int role ) const;
    bool setCalculateAll( const QModelIndex &index, const QVariant &value, int role );

    QVariant projectStart( const QModelIndex &index, int role ) const;
    QVariant projectEnd( const QModelIndex &index, int role ) const;

    QVariant schedulingDirection( const QModelIndex &index, int role ) const;
    bool setSchedulingDirection( const QModelIndex &index, const QVariant &value, int role );

    QVariant schedulingStartTime( const QModelIndex &index, int role ) const;
    bool setSchedulingStartTime( const QModelIndex &index, const QVariant &value, int role );

private:
    ScheduleManager *m_manager; // for sanety check
    
    static ScheduleColumnMap columnMap;
    
};


}  //KPlato namespace

#endif
