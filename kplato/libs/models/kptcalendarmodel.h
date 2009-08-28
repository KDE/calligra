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

#ifndef KPTCALENDARMODEL_H
#define KPTCALENDARMODEL_H

#include "kplatomodels_export.h"

#include "kptitemmodelbase.h"
#include "kptcalendar.h"
#include "kptcalendarmodel.h"

#include "kcalendar/kdatetable.h"

class QPoint;
class QDragMoveEvent;
class QPainter;

namespace KPlato
{

class View;
class Project;
class Calendar;
class CalendarDay;

class KPLATOMODELS_EXPORT CalendarDayItemModelBase : public ItemModelBase
{
    Q_OBJECT
public:
    explicit CalendarDayItemModelBase( QObject *parent = 0 );
    ~CalendarDayItemModelBase();

    virtual void setCalendar( Calendar *calendar );
    virtual Calendar *calendar() const { return m_calendar; }
    virtual void setProject( Project *project );

    CalendarDay *day( const QModelIndex &index ) const;
//    TimeInterval *interval( const QModelIndex &index ) const;
    
//    QModelIndex insertInterval ( TimeInterval *ti, CalendarDay *day );
//    void removeInterval( TimeInterval *ti );
    
//    CalendarDay *parentDay( const TimeInterval *ti ) const { return m_days.value( const_cast<TimeInterval*>( ti ) ); }
    
protected slots:
    void slotCalendarToBeRemoved( const Calendar *calendar );

protected:
    Calendar *m_calendar; // current calendar
//    QMap<TimeInterval*, CalendarDay*> m_days;
};


class KPLATOMODELS_EXPORT CalendarItemModel : public ItemModelBase
{
    Q_OBJECT
public:
    explicit CalendarItemModel( QObject *parent = 0 );
    ~CalendarItemModel();

    virtual void setProject( Project *project );

    virtual Qt::ItemFlags flags( const QModelIndex & index ) const;

    virtual QModelIndex parent( const QModelIndex & index ) const;
    virtual QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    QModelIndex index( const Calendar* calendar ) const;

    virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const; 
    virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const; 

    virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const; 
    virtual bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );


    virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    
    virtual QMimeData * mimeData( const QModelIndexList & indexes ) const;
    virtual QStringList mimeTypes () const;
    virtual Qt::DropActions supportedDropActions() const;
    virtual bool dropMimeData( const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent );
    using ItemModelBase::dropAllowed;
    bool dropAllowed( Calendar *on, const QMimeData *data );

    Calendar *calendar( const QModelIndex &index ) const;
    QModelIndex insertCalendar( Calendar *calendar, Calendar *parent = 0 );
    void removeCalendar( QList<Calendar*> lst );
    void removeCalendar( Calendar *calendar );
    
protected slots:
    void slotCalendarChanged( Calendar* );
    void slotCalendarToBeInserted( const Calendar *parent, int row );
    void slotCalendarInserted( const Calendar *calendar );
    void slotCalendarToBeRemoved( const Calendar *calendar );
    void slotCalendarRemoved( const Calendar *calendar );

protected:
    QVariant name( const Calendar *calendar, int role ) const;
    bool setName( Calendar *calendar, const QVariant &value, int role );
    QVariant timeZone( const Calendar *calendar, int role ) const;
    bool setTimeZone( Calendar *calendar, const QVariant &value, int role );
    
    QList<Calendar*> calendarList( QDataStream &stream ) const;

private:
    Calendar *m_calendar; // test for sane operation
};

class KPLATOMODELS_EXPORT CalendarDayItemModel : public CalendarDayItemModelBase
{
    Q_OBJECT
public:
    explicit CalendarDayItemModel( QObject *parent = 0 );
    ~CalendarDayItemModel();

    virtual void setCalendar( Calendar *calendar );
    
    virtual Qt::ItemFlags flags( const QModelIndex & index ) const;

    virtual QModelIndex parent( const QModelIndex & index ) const;
    virtual bool hasChildren( const QModelIndex & parent = QModelIndex() ) const;
    virtual QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    QModelIndex index( const CalendarDay* day ) const;
    QModelIndex index( const TimeInterval* ti ) const;

    virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const; 
    virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const; 

    virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const; 
    virtual bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );


    virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    CalendarDay *day( const QModelIndex &index ) const;
    TimeInterval *interval( const QModelIndex &index ) const;
    
    QAbstractItemDelegate *createDelegate( int column, QWidget *parent ) const;
    QModelIndex insertInterval ( TimeInterval *ti, CalendarDay *day );
    void removeInterval( TimeInterval *ti );
    QModelIndex insertDay ( CalendarDay *day );
    void removeDay ( CalendarDay *day );
    
    bool isDate( const CalendarDay *day ) const;
    bool isWeekday( const CalendarDay *day ) const;
    
protected slots:
    void slotDayChanged( CalendarDay *day );
    void slotTimeIntervalChanged( TimeInterval *ti );
    
/*    void slotDayToBeAdded( CalendarDay *day, int row );
    void slotDayAdded( CalendarDay *day );
    void slotDayToBeRemoved( CalendarDay *day );
    void slotDayRemoved( CalendarDay *day );*/
    
//    void slotWorkIntervalToBeAdded( CalendarDay *day, TimeInterval *ti, int row );
    void slotWorkIntervalAdded( CalendarDay *day, TimeInterval *ti );
//    void slotWorkIntervalToBeRemoved( CalendarDay *day, TimeInterval *ti );
    void slotWorkIntervalRemoved( CalendarDay *day, TimeInterval *ti );

protected:
/*    QVariant date( const CalendarDay *day, int role ) const;
    bool setDate( CalendarDay *day, const QVariant &value, int role );*/
    QVariant name( int weekday, int role ) const;
    QVariant dayState( const CalendarDay *day, int role ) const;
    bool setDayState( CalendarDay *day, const QVariant &value, int role );
/*    QVariant intervalStart( const TimeInterval *ti, int role ) const;
    bool setIntervalStart( TimeInterval *ti, const QVariant &value, int role );
    QVariant intervalEnd( const TimeInterval *ti, int role ) const;
    bool setIntervalEnd( TimeInterval *ti, const QVariant &value, int role );*/
    QVariant workDuration( const CalendarDay *day, int role ) const;
//    QVariant intervalDuration( const TimeInterval *ti, int role ) const;
    
    void addIntervals( CalendarDay *day );
    void removeIntervals( CalendarDay *day );
    
};

//----->
class KPLATOMODELS_EXPORT DateTableDataModel : public KDateTableDataModel
{
    Q_OBJECT
public:
    DateTableDataModel( QObject *parent );

    /// Fetch data for @p date, @p dataType specifies the type of data
    virtual QVariant data( const QDate &date, int role = Qt::DisplayRole,  int dataType = -1 ) const;
    virtual QVariant weekDayData( int day, int role = Qt::DisplayRole ) const;
    virtual QVariant weekNumberData( int week, int role = Qt::DisplayRole ) const;

public slots:
    void setCalendar( Calendar *calendar );

signals:
    void reset();
    void dataChanged( const QDate &start, const QDate &end );

private:
    Calendar *m_calendar;
};

//-------
class KPLATOMODELS_EXPORT DateTableDateDelegate : public KDateTableDateDelegate
{
    Q_OBJECT
public:
    DateTableDateDelegate( QObject *parent = 0 );
    ~DateTableDateDelegate() {}

    virtual QRectF paint( QPainter *painter, const StyleOptionViewItem &option, const QDate &date,  KDateTableDataModel *model );
    
};


}  //KPlato namespace

#endif
