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

#ifndef KPTCALENDAREDITOR_H
#define KPTCALENDAREDITOR_H

#include "kptviewbase.h"
#include "kptitemmodelbase.h"
#include "kptcalendar.h"

#include <QTreeWidget>
#include <QAbstractProxyModel>

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
class Calendar;
class CalendarDay;

class CalendarDayItemModelBase : public ItemModelBase
{
    Q_OBJECT
public:
    explicit CalendarDayItemModelBase( Part *part, QObject *parent = 0 );
    ~CalendarDayItemModelBase();

    virtual void setCalendar( Calendar *calendar );
    virtual void setProject( Project *project );

    CalendarDay *day( const QModelIndex &index ) const;
    TimeInterval *interval( const QModelIndex &index ) const;
    
    QItemDelegate *createDelegate( int column, QWidget *parent ) const;
    QModelIndex insertInterval ( TimeInterval *ti, CalendarDay *day );
    void removeInterval( TimeInterval *ti );
    
    CalendarDay *parentDay( const TimeInterval *ti ) const { return m_days.value( const_cast<TimeInterval*>( ti ) ); }
    
protected slots:
    void slotCalendarToBeRemoved( const Calendar *calendar );

protected:
    Calendar *m_calendar; // current calendar
    QMap<TimeInterval*, CalendarDay*> m_days;
};


class CalendarItemModel : public ItemModelBase
{
    Q_OBJECT
public:
    explicit CalendarItemModel( Part *part, QObject *parent = 0 );
    ~CalendarItemModel();

    virtual void setProject( Project *project );

    virtual Qt::ItemFlags flags( const QModelIndex & index ) const;

    virtual QModelIndex parent( const QModelIndex & index ) const;
    virtual bool hasChildren( const QModelIndex & parent = QModelIndex() ) const;
    virtual QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    QModelIndex index( const Calendar* calendar ) const;

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
    virtual Qt::DropActions supportedDropActions() const;
    virtual bool dropMimeData( const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent );
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
    
    QList<Calendar*> calendarList( QDataStream &stream ) const;

private:
    Calendar *m_calendar; // test for sane operation
};

class CalendarDayItemModel : public CalendarDayItemModelBase
{
    Q_OBJECT
public:
    explicit CalendarDayItemModel( Part *part, QObject *parent = 0 );
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

    virtual void sort( int column, Qt::SortOrder order = Qt::AscendingOrder );

    CalendarDay *day( const QModelIndex &index ) const;
    TimeInterval *interval( const QModelIndex &index ) const;
    
    QItemDelegate *createDelegate( int column, QWidget *parent ) const;
    QModelIndex insertInterval ( TimeInterval *ti, CalendarDay *day );
    void removeInterval( TimeInterval *ti );
    QModelIndex insertDay ( CalendarDay *day );
    void removeDay ( CalendarDay *day );
    
    bool isDate( const CalendarDay *day ) const;
    bool isWeekday( const CalendarDay *day ) const;
    
protected slots:
    void slotDayChanged( CalendarDay *day );
    void slotTimeIntervalChanged( TimeInterval *ti );
    
    void slotDayToBeAdded( CalendarDay *day, int row );
    void slotDayAdded( CalendarDay *day );
    void slotDayToBeRemoved( CalendarDay *day );
    void slotDayRemoved( CalendarDay *day );
    
    void slotWorkIntervalToBeAdded( CalendarDay *day, TimeInterval *ti, int row );
    void slotWorkIntervalAdded( CalendarDay *day, TimeInterval *ti );
    void slotWorkIntervalToBeRemoved( CalendarDay *day, TimeInterval *ti );
    void slotWorkIntervalRemoved( CalendarDay *day, TimeInterval *ti );

protected:
    QVariant date( const CalendarDay *day, int role ) const;
    bool setDate( CalendarDay *day, const QVariant &value, int role );
    QVariant name( int weekday, int role ) const;
    QVariant dayState( const CalendarDay *day, int role ) const;
    bool setDayState( CalendarDay *day, const QVariant &value, int role );
    QVariant intervalStart( const TimeInterval *ti, int role ) const;
    bool setIntervalStart( TimeInterval *ti, const QVariant &value, int role );
    QVariant intervalEnd( const TimeInterval *ti, int role ) const;
    bool setIntervalEnd( TimeInterval *ti, const QVariant &value, int role );
    QVariant workDuration( const CalendarDay *day, int role ) const;
    QVariant intervalDuration( const TimeInterval *ti, int role ) const;
    
    void addIntervals( CalendarDay *day );
    void removeIntervals( CalendarDay *day );
    
    void setDayMap( CalendarDay *day );
    void clearDayMap( CalendarDay *day );
    virtual void setDayMap( Calendar *calendar );
    
    class TopLevelType
    {
    public:
        TopLevelType( QString n = "") { name = n; }
        QString name;
    };
    bool isDate( const QModelIndex &index ) const;
    bool isWeekday( const  QModelIndex &index ) const;
    bool isTopLevel( const  QModelIndex &index ) const { return isDate( index ) || isWeekday( index ); }


private:
    TopLevelType *typeWeekday;
    TopLevelType *typeDate;
};

class CalendarTreeView : public TreeViewBase
{
    Q_OBJECT
public:
    CalendarTreeView( Part *part, QWidget *parent );

    CalendarItemModel *itemModel() const { return static_cast<CalendarItemModel*>( model() ); }

    Project *project() const { return itemModel()->project(); }
    void setProject( Project *project ) { itemModel()->setProject( project ); }

    Calendar *currentCalendar() const;
    Calendar *selectedCalendar() const;
    QList<Calendar*> selectedCalendars() const;
    
signals:
    void currentChanged( const QModelIndex& );
    void currentColumnChanged( QModelIndex, QModelIndex );
    void selectionChanged( const QModelIndexList );

    void contextMenuRequested( QModelIndex, const QPoint& );
    void focusChanged();
    
protected slots:
    void headerContextMenuRequested( const QPoint &pos );
    void slotActivated( const QModelIndex index );
    virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    virtual void currentChanged ( const QModelIndex & current, const QModelIndex & previous );

protected:
    void contextMenuEvent ( QContextMenuEvent * event );
    void focusInEvent ( QFocusEvent * event );
    void focusOutEvent ( QFocusEvent * event );
    
    void dragMoveEvent(QDragMoveEvent *event);
};

class CalendarDayView : public TreeViewBase
{
    Q_OBJECT
public:
    CalendarDayView( Part *part, QWidget *parent );

    CalendarDayItemModel *itemModel() const { return m_model; }

    Project *project() const { return itemModel()->project(); }
    void setProject( Project *project ) { itemModel()->setProject( project ); }

    CalendarDay *selectedDay() const;
    TimeInterval *selectedInterval() const;
    
signals:
    void currentChanged( const QModelIndex& );
    void currentColumnChanged( QModelIndex, QModelIndex );
    void selectionChanged( const QModelIndexList );

    void contextMenuRequested( QModelIndex, const QPoint& );
    void focusChanged();
    
public slots:
    void setCurrentCalendar( Calendar *calendar );
    
protected slots:
    void headerContextMenuRequested( const QPoint &pos );
    void slotActivated( const QModelIndex index );
    virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    virtual void currentChanged ( const QModelIndex & current, const QModelIndex & previous );

protected:
    void contextMenuEvent ( QContextMenuEvent * event );
    void focusInEvent ( QFocusEvent * event );
    void focusOutEvent ( QFocusEvent * event );
    
private:
    CalendarDayItemModel *m_model;
};

class CalendarEditor : public ViewBase
{
    Q_OBJECT
public:
    CalendarEditor( Part *part, QWidget *parent );
    
    void setupGui();
    virtual void draw( Project &project );
    virtual void draw();

    virtual void updateReadWrite( bool /*readwrite*/ ) {};

    virtual Calendar *currentCalendar() const;
    
signals:
    void requestPopupMenu( const QString&, const QPoint& );
    void addCalendar( Calendar *calendar );
    void deleteCalendar( QList<Calendar*> );
    
public slots:
    /// Activate/deactivate the gui
    virtual void setGuiActive( bool activate );

protected:
    void updateActionsEnabled( bool on );
    void insertCalendar( Calendar *calendar, Calendar *parent );
    
private slots:
    void slotContextMenuCalendar( QModelIndex index, const QPoint& pos );
    void slotContextMenuDay( QModelIndex index, const QPoint& pos );
    
    void slotCalendarSelectionChanged( const QModelIndexList );
    void slotCurrentCalendarChanged( const QModelIndex& );
    
    void slotDaySelectionChanged( const QModelIndexList );
    void slotCurrentDayChanged( const QModelIndex& );
    
    void slotEnableActions();

    void slotAddCalendar();
    void slotAddSubCalendar();
    void slotDeleteCalendar();

    void slotAddDay();
    void slotAddInterval();
    void slotDeleteDaySelection();

private:
    CalendarTreeView *m_calendarview;
    CalendarDayView *m_dayview;
    
    KAction *actionAddCalendar;
    KAction *actionAddSubCalendar;
    KAction *actionDeleteSelection;

    KAction *actionAddDay;
    KAction *actionAddWorkInterval;
    KAction *actionDeleteDaySelection;

};

}  //KPlato namespace

#endif
