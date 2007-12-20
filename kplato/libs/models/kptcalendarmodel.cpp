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

#include "kptcalendarmodel.h"

#include "kptglobal.h"
#include "kptcommand.h"
#include "kptitemmodelbase.h"
#include "kptcalendar.h"
#include "kptduration.h"
#include "kptnode.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptdatetime.h"

#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QList>
#include <QHeaderView>
#include <QObject>
#include <QStringList>

#include <kglobal.h>
#include <klocale.h>
#include <kcalendarsystem.h>
#include <ksystemtimezone.h>
#include <ktimezone.h>

#include <kdebug.h>

namespace KPlato
{


//-----------------------------------------
CalendarDayItemModelBase::CalendarDayItemModelBase( QObject *parent )
    : ItemModelBase( parent ),
    m_calendar( 0 )
{
}

CalendarDayItemModelBase::~CalendarDayItemModelBase()
{
}

void CalendarDayItemModelBase::slotCalendarToBeRemoved( const Calendar *calendar )
{
    if ( calendar && calendar == m_calendar ) {
        setCalendar( 0 );
    }
}


void CalendarDayItemModelBase::setCalendar( Calendar *calendar )
{
    m_calendar = calendar;
}

void CalendarDayItemModelBase::setProject( Project *project )
{
    setCalendar( 0 );
    if ( m_project ) {
        disconnect( m_project, SIGNAL( calendarToBeRemoved( const Calendar* ) ), this, SLOT( slotCalendarToBeRemoved( const Calendar* ) ) );
    }
    m_project = project;
    if ( project ) {
        connect( m_project, SIGNAL( calendarToBeRemoved( const Calendar* ) ), this, SLOT( slotCalendarToBeRemoved( const Calendar* ) ) );
    }
}


//-------------------------------------
CalendarItemModel::CalendarItemModel( QObject *parent )
    : ItemModelBase( parent ),
    m_calendar( 0 )
{
}

CalendarItemModel::~CalendarItemModel()
{
}

void CalendarItemModel::slotCalendarToBeInserted( const Calendar *parent, int row )
{
    //kDebug()<<(parent?parent->name():"Top level")<<","<<row;
    Q_ASSERT( m_calendar == 0 );
    m_calendar = const_cast<Calendar *>(parent);
    beginInsertRows( index( parent ), row, row );
}

void CalendarItemModel::slotCalendarInserted( const Calendar *calendar )
{
    //kDebug()<<calendar->name();
    Q_ASSERT( calendar->parentCal() == m_calendar );
    endInsertRows();
    m_calendar = 0;
}

void CalendarItemModel::slotCalendarToBeRemoved( const Calendar *calendar )
{
    //kDebug()<<calendar->name();
    int row = index( calendar ).row();
    beginRemoveRows( index( calendar->parentCal() ), row, row );
}

void CalendarItemModel::slotCalendarRemoved( const Calendar * )
{
    //kDebug()<<calendar->name();
    endRemoveRows();
}

void CalendarItemModel::setProject( Project *project )
{
    if ( m_project ) {
        disconnect( m_project , SIGNAL( calendarChanged( Calendar* ) ), this, SLOT( slotCalendarChanged( Calendar* ) ) );
        
        disconnect( m_project, SIGNAL( calendarAdded( const Calendar* ) ), this, SLOT( slotCalendarInserted( const Calendar* ) ) );
        disconnect( m_project, SIGNAL( calendarToBeAdded( const Calendar*, int ) ), this, SLOT( slotCalendarToBeInserted( const Calendar*, int ) ) );
        
        disconnect( m_project, SIGNAL( calendarRemoved( const Calendar* ) ), this, SLOT( slotCalendarRemoved( const Calendar* ) ) );
        disconnect( m_project, SIGNAL( calendarToBeRemoved( const Calendar* ) ), this, SLOT( slotCalendarToBeRemoved( const Calendar* ) ) );
    }
    m_project = project;
    if ( project ) {
        connect( m_project, SIGNAL( calendarChanged( Calendar* ) ), this, SLOT( slotCalendarChanged( Calendar* ) ) );
        
        connect( m_project, SIGNAL( calendarAdded( const Calendar* ) ), this, SLOT( slotCalendarInserted( const Calendar* ) ) );
        connect( m_project, SIGNAL( calendarToBeAdded( const Calendar*, int ) ), this, SLOT( slotCalendarToBeInserted( const Calendar*, int ) ) );
        
        connect( m_project, SIGNAL( calendarRemoved( const Calendar* ) ), this, SLOT( slotCalendarRemoved( const Calendar* ) ) );
        connect( m_project, SIGNAL( calendarToBeRemoved( const Calendar* ) ), this, SLOT( slotCalendarToBeRemoved( const Calendar* ) ) );
    }
}

Qt::ItemFlags CalendarItemModel::flags( const QModelIndex &index ) const
{
    Qt::ItemFlags flags = ItemModelBase::flags( index );
    if ( !m_readWrite ) {
        return flags &= ~Qt::ItemIsEditable;
    }
    flags |= Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
    if ( !index.isValid() ) {
        return flags;
    }

    if ( !index.isValid() )
        return flags;
    if ( !m_readWrite ) {
        return flags &= ~Qt::ItemIsEditable;
    }
    if ( calendar ( index ) ) {
        switch ( index.column() ) {
            case 1: 
                if ( parent( index ).isValid() ) {
                    flags &= ~Qt::ItemIsEditable;
                } else {
                    flags |= Qt::ItemIsEditable;
                }
                break;
            default: 
                flags |= Qt::ItemIsEditable;
                break;
        }
    }
    return flags;
}


QModelIndex CalendarItemModel::parent( const QModelIndex &index ) const
{
    if ( !index.isValid() || m_project == 0 ) {
        return QModelIndex();
    }
    //kDebug()<<index.internalPointer()<<":"<<index.row()<<","<<index.column();
    Calendar *a = calendar( index );
    if ( a == 0 ) {
        return QModelIndex();
    }
    Calendar *par = a->parentCal();
    if ( par ) {
        a = par->parentCal();
        int row = -1;
        if ( a ) {
            row = a->indexOf( par );
        } else {
            row = m_project->indexOf( par );
        }
        //kDebug()<<par->name()<<":"<<row;
        return createIndex( row, 0, par );
    }
    return QModelIndex();
}

QModelIndex CalendarItemModel::index( int row, int column, const QModelIndex &parent ) const
{
    if ( m_project == 0 || column < 0 || column >= columnCount() || row < 0 ) {
        return QModelIndex();
    }
    Calendar *par = calendar( parent );
    if ( par == 0 ) {
        if ( row < m_project->calendars().count() ) {
            return createIndex( row, column, m_project->calendars().at( row ) );
        }
    } else if ( row < par->calendars().count() ) {
        return createIndex( row, column, par->calendars().at( row ) );
    }
    return QModelIndex();
}

QModelIndex CalendarItemModel::index( const Calendar *calendar) const
{
    if ( m_project == 0 || calendar == 0 ) {
        return QModelIndex();
    }
    Calendar *a = const_cast<Calendar*>(calendar);
    int row = -1;
    Calendar *par = a->parentCal();
    if ( par == 0 ) {
         row = m_project->calendars().indexOf( a );
    } else {
        row = par->indexOf( a );
    }
    if ( row == -1 ) {
        return QModelIndex();
    }
    return createIndex( row, 0, a );

}

int CalendarItemModel::columnCount( const QModelIndex & ) const
{
    return 2;
}

int CalendarItemModel::rowCount( const QModelIndex &parent ) const
{
    if ( m_project == 0 ) {
        return 0;
    }
    Calendar *par = calendar( parent );
    if ( par == 0 ) {
        return m_project->calendars().count();
    }
    return par->calendars().count();
}

QVariant CalendarItemModel::name( const Calendar *a, int role ) const
{
    //kDebug()<<res->name()<<","<<role;
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

bool CalendarItemModel::setName( Calendar *a, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            if ( value.toString() != a->name() ) {
                emit executeCommand( new CalendarModifyNameCmd( a, value.toString(), "Modify Calendar Name" ) );
            }
            return true;
    }
    return false;
}

QVariant CalendarItemModel::timeZone( const Calendar *a, int role ) const
{
    //kDebug()<<res->name()<<","<<role;
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return i18n( a->timeZone().name().toUtf8() );
        case Role::EnumList: {
            QStringList lst;
            foreach ( QString s, KSystemTimeZones::timeZones()->zones().keys() ) {
                lst << i18n( s.toUtf8() );
            }
            lst.sort();
            return lst;
        }
        case Role::EnumListValue: {
            QStringList lst = timeZone( a, Role::EnumList ).toStringList();
            return lst.indexOf( i18n ( a->timeZone().name().toUtf8() ) );
        }
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool CalendarItemModel::setTimeZone( Calendar *a, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole: {
            QStringList lst = timeZone( a, Role::EnumList ).toStringList();
            QString name = lst.value( value.toInt() );
            KTimeZone tz;
            foreach ( QString s, KSystemTimeZones::timeZones()->zones().keys() ) {
                if ( name == i18n( s.toUtf8() ) ) {
                    tz = KSystemTimeZones::zone( s );
                    break;
                }
            }
            if ( !tz.isValid() ) {
                return false;
            }
            emit executeCommand( new CalendarModifyTimeZoneCmd( a, tz, "Modify Calendar Timezone" ) );
            return true;
        }
    }
    return false;
}

QVariant CalendarItemModel::data( const QModelIndex &index, int role ) const
{
    QVariant result;
    Calendar *a = calendar( index );
    if ( a == 0 ) {
        return QVariant();
    }
    switch ( index.column() ) {
        case 0: result = name( a, role ); break;
        case 1: result = timeZone( a, role ); break;
        default:
            kDebug()<<"data: invalid display value column"<<index.column();;
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

bool CalendarItemModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
    if ( ( flags( index ) &Qt::ItemIsEditable ) == 0 || role != Qt::EditRole ) {
        return false;
    }
    Calendar *a = calendar( index );
    switch (index.column()) {
        case 0: return setName( a, value, role );
        case 1: return setTimeZone( a, value, role );
        default:
            kWarning()<<"data: invalid display value column "<<index.column()<<endl;
            return false;
    }
    return false;
}

QVariant CalendarItemModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal ) {
        if ( role == Qt::DisplayRole ) {
            switch ( section ) {
                case 0: return i18n( "Name" );
                case 1: return i18n( "Timezone" );
                default: return QVariant();
            }
        } else if ( role == Qt::TextAlignmentRole ) {
            switch (section) {
                default: return QVariant();
            }
        }
    }
    if ( role == Qt::ToolTipRole ) {
        switch ( section ) {
            case 0: return ToolTip::calendarName();
            case 1: return ToolTip::calendarTimeZone();
            default: return QVariant();
        }
    }
    return ItemModelBase::headerData(section, orientation, role);
}

void CalendarItemModel::sort( int , Qt::SortOrder  )
{
}

Calendar *CalendarItemModel::calendar( const QModelIndex &index ) const
{
    return static_cast<Calendar*>( index.internalPointer() );
}

void CalendarItemModel::slotCalendarChanged( Calendar *calendar )
{
    Calendar *par = calendar->parentCal();
    if ( par ) {
        int row = par->indexOf( calendar );
        emit dataChanged( createIndex( row, 0, calendar ), createIndex( row, columnCount() - 1, calendar ) );
    } else {
        int row = m_project->indexOf( calendar );
        emit dataChanged( createIndex( row, 0, calendar ), createIndex( row, columnCount() - 1, calendar ) );
    }
}

Qt::DropActions CalendarItemModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}


QStringList CalendarItemModel::mimeTypes() const
{
    return QStringList() << "application/x-vnd.kde.kplato.calendarid.internal";
}

QMimeData *CalendarItemModel::mimeData( const QModelIndexList & indexes ) const
{
    QMimeData *m = new QMimeData();
    QByteArray encodedData;
    QDataStream stream(&encodedData, QIODevice::WriteOnly);
    QList<int> rows;
    foreach (QModelIndex index, indexes) {
        if ( index.isValid() && !rows.contains( index.row() ) ) {
            kDebug()<<index.row();
            Calendar *c = calendar( index );
            if ( c ) {
                stream << c->id();
            }
        }
    }
    m->setData("application/x-vnd.kde.kplato.calendarid.internal", encodedData);
    return m;
}

bool CalendarItemModel::dropMimeData( const QMimeData *data, Qt::DropAction action, int /*row*/, int /*column*/, const QModelIndex &parent )
{
    kDebug()<<action;
    if (action == Qt::IgnoreAction) {
        return true;
    }
    if ( !data->hasFormat( "application/x-vnd.kde.kplato.calendarid.internal" ) ) {
        return false;
    }
    if ( action == Qt::MoveAction ) {
        kDebug()<<"MoveAction";
        
        QByteArray encodedData = data->data( "application/x-vnd.kde.kplato.calendarid.internal" );
        QDataStream stream(&encodedData, QIODevice::ReadOnly);
        Calendar *par = 0;
        if ( parent.isValid() ) {
            par = calendar( parent );
        }
        MacroCommand *cmd = 0;
        QList<Calendar*> lst = calendarList( stream );
        foreach ( Calendar *c, lst ) {
            if ( cmd == 0 ) cmd = new MacroCommand( i18n( "Re-parent Calendar" ) );
            cmd->addCommand( new CalendarModifyParentCmd( m_project, c, par ) );
        }
        if ( cmd ) {
            emit executeCommand( cmd );
        }
        //kDebug()<<row<<","<<column<<" parent="<<parent.row()<<","<<parent.column()<<":"<<par->name();
        return true;
    }
    return false;
}

QList<Calendar*> CalendarItemModel::calendarList( QDataStream &stream ) const
{
    QList<Calendar*> lst;
    while (!stream.atEnd()) {
        QString id;
        stream >> id;
        Calendar *c = m_project->findCalendar( id );
        if ( c ) {
            lst << c;
        }
    }
    return lst;
}

bool CalendarItemModel::dropAllowed( Calendar *on, const QMimeData *data )
{
    if ( !data->hasFormat("application/x-vnd.kde.kplato.calendarid.internal") ) {
        return false;
    }
    if ( on == 0 ) {
        return true;
    }
    QByteArray encodedData = data->data( "application/x-vnd.kde.kplato.calendarid.internal" );
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    QList<Calendar*> lst = calendarList( stream );
    foreach ( Calendar *c, lst ) {
        if ( on == c || on->isChildOf( c ) ) {
            return false;
        }
    }
    return true;
}

QModelIndex CalendarItemModel::insertCalendar ( Calendar *calendar, Calendar *parent )
{
    //kDebug();
    emit executeCommand( new CalendarAddCmd( m_project, calendar, parent, i18n( "Add Calendar" ) ) );
    int row = -1;
    if ( parent ) {
        row = parent->indexOf( calendar );
    } else {
        row = m_project->indexOf( calendar );
    }
    if ( row != -1 ) {
        //kDebug()<<"Inserted:"<<calendar->name();
        return createIndex( row, 0, calendar );
    }
    return QModelIndex();
}

void CalendarItemModel::removeCalendar( QList<Calendar *> /*lst*/ )
{
/*    MacroCommand *cmd = 0;
    QString s = lst.count() > 1 ? i18n( "Delete Calendars" ) : i18n( "Delete Calendar" );
    while ( ! lst.isEmpty() ) {
        bool del = true;
        Calendar *c = lst.takeFirst();
        foreach ( Calendar *a, lst ) {
            if ( c->hasParent( a ) ) {
                del = false; // c will be deleted when a is deleted
                break;
            }
        }
        if ( del ) {
            if ( cmd == 0 ) cmd = new MacroCommand( s );
            cmd->addCommand( new CalendarRemoveCmd( m_project, c ) );
        }
    }
    if ( cmd )
        emit executeCommand( cmd );*/
}

void CalendarItemModel::removeCalendar( Calendar *calendar )
{
    if ( calendar == 0 ) {
        return;
    }
    emit executeCommand( new CalendarRemoveCmd( m_project, calendar, i18n( "Delete Calendar" ) ) );
}


//------------------------------------------
CalendarDayItemModel::CalendarDayItemModel( QObject *parent )
    : CalendarDayItemModelBase( parent )
{
    typeWeekday = new TopLevelType( i18n( "Weekdays" ) );
    typeDate = new TopLevelType(  i18n( "Days" ) );
}

CalendarDayItemModel::~CalendarDayItemModel()
{
}

void CalendarDayItemModel::slotDayToBeAdded( CalendarDay *, int row )
{
    // only dates, weekdays are not added
    beginInsertRows( createIndex( 1, 0, typeDate ), row, row );
}

void CalendarDayItemModel::slotDayAdded( CalendarDay *day )
{
    setDayMap( day );
    endInsertRows();
}

void CalendarDayItemModel::slotDayToBeRemoved( CalendarDay *day )
{
    // only dates, weekdays are not removed
    int row = m_calendar->indexOf( day );
    beginRemoveRows( createIndex( 1, 0, typeDate ), row, row );
    clearDayMap( day );
    //kDebug()<<day->date()<<","<<row;
}

void CalendarDayItemModel::slotDayRemoved( CalendarDay * )
{
    //kDebug()<<day->date();
    endRemoveRows();
}

void CalendarDayItemModel::slotWorkIntervalToBeAdded( CalendarDay *day, TimeInterval *, int row )
{
    int r = m_calendar->indexOfWeekday( day );
    if ( r == -1 ) {
        r = m_calendar->indexOf( day );
    }
    //kDebug()<<day<<","<<ti<<","<<row;
    beginInsertRows( createIndex( r, 0, day ), row, row );
}

void CalendarDayItemModel::slotWorkIntervalAdded( CalendarDay *day, TimeInterval *ti )
{
    //kDebug()<<day<<","<<ti;
    m_days.insert( ti, day );
    endInsertRows();
}

void CalendarDayItemModel::slotWorkIntervalToBeRemoved( CalendarDay *day, TimeInterval *ti )
{
    Q_ASSERT( day == parentDay( ti ) );
    int r = m_calendar->indexOfWeekday( day );
    if ( r == -1 ) {
        r = m_calendar->indexOf( day );
    }
    int row = day->indexOf( ti );
    beginRemoveRows( createIndex( r, 0, day ), row, row );
}

void CalendarDayItemModel::slotWorkIntervalRemoved( CalendarDay *day, TimeInterval *ti )
{
    Q_ASSERT( day == parentDay( ti ) );
    m_days.remove( ti );
    endRemoveRows();
}

void CalendarDayItemModel::slotDayChanged( CalendarDay *day )
{
    int row = m_calendar->indexOfWeekday( day );
    if ( row == -1 ) {
        row = m_calendar->indexOf( day );
    }
    kDebug()<<day<<","<<row;
    emit dataChanged( createIndex( row, 0, day ), createIndex( row, columnCount() - 1, day ) );
}

void CalendarDayItemModel::slotTimeIntervalChanged( TimeInterval *ti )
{
    CalendarDay *d = parentDay( ti );
    if ( d == 0 ) {
        return;
    }
    int row = d->indexOf( ti );
    emit dataChanged( createIndex( row, 0, ti ), createIndex( row, columnCount() - 1, ti ) );
}

void CalendarDayItemModel::setCalendar( Calendar *calendar )
{
    //kDebug()<<m_calendar<<" -->"<<calendar;
    if ( m_calendar ) {
        disconnect( m_calendar, SIGNAL( changed( CalendarDay*) ), this, SLOT( slotDayChanged( CalendarDay* ) ) );
        disconnect( m_calendar, SIGNAL( changed( TimeInterval* ) ), this, SLOT( slotTimeIntervalChanged( TimeInterval* ) ) );

        disconnect( m_calendar, SIGNAL( dayToBeAdded( CalendarDay*, int ) ), this, SLOT( slotDayToBeAdded( CalendarDay*, int ) ) );
        disconnect( m_calendar, SIGNAL( dayAdded( CalendarDay* ) ), this, SLOT( slotDayAdded( CalendarDay* ) ) );
        disconnect( m_calendar, SIGNAL( dayToBeRemoved( CalendarDay* ) ), this, SLOT( slotDayToBeRemoved( CalendarDay* ) ) );
        disconnect( m_calendar, SIGNAL( dayRemoved( CalendarDay* ) ), this, SLOT( slotDayRemoved( CalendarDay* ) ) );

        disconnect( m_calendar, SIGNAL( workIntervalToBeAdded( CalendarDay*, TimeInterval*, int ) ), this, SLOT( slotWorkIntervalToBeAdded( CalendarDay*, TimeInterval*, int ) ) );
        disconnect( m_calendar, SIGNAL( workIntervalAdded( CalendarDay*, TimeInterval* ) ), this, SLOT( slotWorkIntervalAdded( CalendarDay*, TimeInterval* ) ) );
        disconnect( m_calendar, SIGNAL( workIntervalToBeRemoved( CalendarDay*, TimeInterval* ) ), this, SLOT( slotWorkIntervalToBeRemoved( CalendarDay*, TimeInterval* ) ) );
        disconnect( m_calendar, SIGNAL( workIntervalRemoved( CalendarDay*, TimeInterval* ) ), this, SLOT( slotWorkIntervalRemoved( CalendarDay*, TimeInterval* ) ) );
    }
    setDayMap( calendar );
    m_calendar = calendar;
    if ( calendar ) {
        connect( m_calendar, SIGNAL( changed( CalendarDay*) ), this, SLOT( slotDayChanged( CalendarDay* ) ) );
        connect( m_calendar, SIGNAL( changed( TimeInterval* ) ), this, SLOT( slotTimeIntervalChanged( TimeInterval* ) ) );

        connect( m_calendar, SIGNAL( dayToBeAdded( CalendarDay*, int ) ), this, SLOT( slotDayToBeAdded( CalendarDay*, int ) ) );
        connect( m_calendar, SIGNAL( dayAdded( CalendarDay* ) ), this, SLOT( slotDayAdded( CalendarDay* ) ) );
        connect( m_calendar, SIGNAL( dayToBeRemoved( CalendarDay* ) ), this, SLOT( slotDayToBeRemoved( CalendarDay* ) ) );
        connect( m_calendar, SIGNAL( dayRemoved( CalendarDay* ) ), this, SLOT( slotDayRemoved( CalendarDay* ) ) );

        connect( m_calendar, SIGNAL( workIntervalToBeAdded( CalendarDay*, TimeInterval*, int ) ), this, SLOT( slotWorkIntervalToBeAdded( CalendarDay*, TimeInterval*, int ) ) );
        connect( m_calendar, SIGNAL( workIntervalAdded( CalendarDay*, TimeInterval* ) ), this, SLOT( slotWorkIntervalAdded( CalendarDay*, TimeInterval* ) ) );
        connect( m_calendar, SIGNAL( workIntervalToBeRemoved( CalendarDay*, TimeInterval* ) ), this, SLOT( slotWorkIntervalToBeRemoved( CalendarDay*, TimeInterval* ) ) );
        connect( m_calendar, SIGNAL( workIntervalRemoved( CalendarDay*, TimeInterval* ) ), this, SLOT( slotWorkIntervalRemoved( CalendarDay*, TimeInterval* ) ) );
    }
    reset();
}

Qt::ItemFlags CalendarDayItemModel::flags( const QModelIndex &index ) const
{
    Qt::ItemFlags flags = ItemModelBase::flags( index );
    if ( !m_readWrite ) {
        return flags &= ~Qt::ItemIsEditable;
    }
    if ( !index.isValid() || isTopLevel( index ) ) {
        return flags;
    }
    if ( day ( index ) ) {
        flags &= ~Qt::ItemIsEditable;
        switch ( index.column() ) {
            case 0:
                if ( isDate( day( index ) ) ) {
                    flags |= Qt::ItemIsEditable;
                }
                break;
            case 1:
                flags |= Qt::ItemIsEditable;
                break;
            default:
                flags &= ~Qt::ItemIsEditable;
                break;
        }
    } else if ( interval( index ) ) {
        switch ( index.column() ) {
            case 0:
            case 1:
                flags |= Qt::ItemIsEditable;
                break;
            default: 
                flags &= ~Qt::ItemIsEditable; 
                break;
        }
    }
    return flags;
}

bool CalendarDayItemModel::isDate( const CalendarDay *day ) const
{
    return m_calendar->indexOf( day ) != -1;
}

bool CalendarDayItemModel::isWeekday( const CalendarDay *day ) const
{
    return m_calendar->indexOfWeekday( day ) != -1;
}

bool CalendarDayItemModel::isDate( const QModelIndex &index ) const
{
    return typeDate == index.internalPointer();
}

bool CalendarDayItemModel::isWeekday( const  QModelIndex &index ) const
{
    return typeWeekday == index.internalPointer();
}

QModelIndex CalendarDayItemModel::parent( const QModelIndex &index ) const
{
    if ( ! index.isValid() ) {
        return QModelIndex();
    }
    if ( isTopLevel( index ) ) {
         return QModelIndex();
    }
    CalendarDay *d = day( index );
    if ( d ) {
        if ( isWeekday( d ) ) {
            return createIndex( 0, 0, typeWeekday );
        }
        if ( isDate( d ) ) {
            return createIndex( 1, 0, typeDate );
        }
        kError()<<1<<endl;
        return QModelIndex();
    }
    TimeInterval *ti = interval( index );
    if ( ti == 0 ) {
        kError()<<2<<endl;
        return QModelIndex();
    }
    d = parentDay( ti );
    if ( d == 0 ) {
        kError()<<3<<endl;
        return QModelIndex();
    }
    int row = -1;
    if ( isWeekday( d ) ) {
        row = m_calendar->indexOfWeekday( d );
    } else if ( isDate( d ) ) {
        row = m_calendar->indexOf( d );
    } else {
        kError()<<4<<endl;
        return QModelIndex();
    }
    return createIndex( row, 0, d );
}

bool CalendarDayItemModel::hasChildren( const QModelIndex &parent ) const
{
    //kDebug()<<parent.internalPointer()<<":"<<parent.row()<<","<<parent.column();
    if ( m_project == 0 || m_calendar == 0 ) {
        return false;
    }
    if ( ! parent.isValid() ) {
        return typeWeekday != 0 || typeDate != 0;
    }
    if ( isWeekday( parent ) ) {
        return true;
    }
    if ( isDate( parent ) ) {
        return m_calendar->numDays() > 0;
    }
    CalendarDay *d = day( parent );
    if ( d == 0 ) {
        return false; // intervals have no children
    }
    if ( d->state() != CalendarDay::Working ) {
        return false; // hmmm
    }
    return d->hasInterval();
}

QModelIndex CalendarDayItemModel::index( int row, int column, const QModelIndex &par ) const
{
    if ( m_project == 0 || m_calendar == 0 ) {
        return QModelIndex();
    }
    if ( ! par.isValid() ) {
        // toplevel
        if ( row == 0 ) {
            return createIndex( row, column, typeWeekday );
        }
        if ( row == 1 ) {
            return createIndex( row, column, typeDate );
        }
        return QModelIndex();
    }
    CalendarDay *d = 0;
    if ( isWeekday( par ) ) {
        d = m_calendar->weekday( row + 1 ); // weekdays are 1..7
        if ( d == 0 ) {
            return QModelIndex();
        }
        return createIndex( row, column, d );
    }
    if ( isDate( par ) ) {
        d = m_calendar->dayAt( row );
        if ( d == 0 ) {
            return QModelIndex();
        }
        return createIndex( row, column, d );
    }
    // interval?
    d = day( par );
    if ( d == 0 ) {
        return QModelIndex();
    }
    return createIndex( row, column, d->intervalAt( row ) ); // work interval
}

QModelIndex CalendarDayItemModel::index( const CalendarDay *d) const
{
    if ( m_project == 0 || m_calendar == 0 ) {
        return QModelIndex();
    }
    int row = m_calendar->indexOfWeekday( d );
    if ( row == -1 ) {
        row = m_calendar->indexOf( d );
    }
    if ( row == -1 ) {
        return QModelIndex();
    }
    return createIndex( row, 0, const_cast<CalendarDay*>( d ) );
}

QModelIndex CalendarDayItemModel::index( const TimeInterval *ti) const
{
    if ( m_project == 0 || m_calendar == 0 || ! m_days.contains( const_cast<TimeInterval*>( ti ) ) ) {
        return QModelIndex();
    }
    return createIndex( m_days[ const_cast<TimeInterval*>( ti ) ]->indexOf( ti ), 0, const_cast<TimeInterval*>( ti ) );
}

int CalendarDayItemModel::columnCount( const QModelIndex &/*parent*/ ) const
{
    return 3;
}

int CalendarDayItemModel::rowCount( const QModelIndex &parent ) const
{
    if ( m_project == 0 || m_calendar == 0 ) {
        return 0;
    }
    if ( ! parent.isValid() ) {
        return 2; // weekdays and dates
    }
    if ( isWeekday( parent ) ) {
        return 7;
    }
    if ( isDate( parent ) ) {
        return m_calendar->numDays();
    }
    CalendarDay *d = day( parent );
    if ( d == 0 || d->state() != CalendarDay::Working ) {
        return 0;
    }
    return d->numIntervals();
}

QVariant CalendarDayItemModel::date( const CalendarDay *d, int role ) const
{
    //kDebug()<<res->name()<<","<<role;
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            return KGlobal::locale()->formatDate( d->date() );
            break;
        case Qt::EditRole:
            return d->date();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool CalendarDayItemModel::setDate( CalendarDay *d, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole: {
            QDate date = value.toDate();
            if ( m_calendar->findDay( date ) ) {
                return false;
            }
            //kDebug()<<d;
            emit executeCommand( new CalendarModifyDateCmd( m_calendar, d, date,  "Modify Calendar Date" ) );
            return true;
        }
    }
    return false;
}

QVariant CalendarDayItemModel::name( int weekday, int role ) const
{
    //kDebug()<<res->name()<<","<<role;
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole: {
            if ( weekday >= 1 && weekday <= 7 ) {
                return KGlobal::locale()->calendar()->weekDayName( weekday );
            }
            break;
        }
        case Qt::EditRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant CalendarDayItemModel::dayState( const CalendarDay *d, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            return CalendarDay::stateToString( d->state(), true );
        case Role::EnumList: {
            QStringList lst = CalendarDay::stateList( true );
            if ( isDate( d ) ) {
                lst.removeAt( CalendarDay::Undefined );
            }
            return lst;
        }
        case Qt::EditRole:
        case Role::EnumListValue: {
            int v = d->state();
            if ( isDate( d ) && v >= CalendarDay::Undefined ) {
                v--;
            }
            return v;
        }
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
        case Role::EditorType:
            return Delegate::EnumEditor;
    }
    return QVariant();
}
bool CalendarDayItemModel::setDayState( CalendarDay *d, const QVariant &value, int role )
{
    //kDebug();
    switch ( role ) {
        case Qt::EditRole:
            int v = value.toInt();
            if ( isDate ( d ) ) {
                if ( v >= CalendarDay::Undefined )
                    v++; // Undefined not in list
            }
            emit executeCommand( new CalendarModifyStateCmd( m_calendar, d, static_cast<CalendarDay::State>( v ), "Modify Calendar State" ) );
            return true;
    }
    return false;
}

QVariant CalendarDayItemModel::intervalStart( const TimeInterval *ti, int role ) const
{
    //kDebug()<<res->name()<<","<<role;
    switch ( role ) {
        case Qt::DisplayRole:
            return ti->first;
        case Qt::ToolTipRole:
        case Qt::EditRole:
            return ti->first;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
        case Role::Minimum: {
                return QTime();
/*            CalendarDay *d = m_days.value( const_cast<TimeInterval*>( ti ) );
            if ( d == 0 ) {
                return QTime();
            }
            return d->minStartTime( ti );*/
        }
        case Role::Maximum: {
            return QTime();
/*            CalendarDay *d = m_days.value( const_cast<TimeInterval*>( ti ) );
            if ( d == 0 ) {
                return QTime();
            }
            return d->maxStartTime( ti );*/
        }
        case Role::EditorType:
            return Delegate::TimeEditor;
    }
    return QVariant();
}

bool CalendarDayItemModel::setIntervalStart( TimeInterval *ti, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole: {
            QTime start = value.toTime();
            TimeInterval t = TimeInterval( start, ti->second );
            emit executeCommand( new CalendarModifyTimeIntervalCmd( m_calendar, t, ti,  "Modify Calendar Working Interval" ) );
            return true;
        }
    }
    return false;
}

QVariant CalendarDayItemModel::intervalEnd( const TimeInterval *ti, int role ) const
{
    //kDebug()<<res->name()<<","<<role;
    KLocale *locale = KGlobal::locale();
    double value = (double)(ti->second) / ( 1000 * 60 * 60 );
    switch ( role ) {
        case Qt::DisplayRole:
            return locale->formatNumber( value ) + Duration::unitToString( Duration::Unit_h, true );
        case Qt::ToolTipRole:
            return i18n( "Work interval length: %1", locale->formatNumber( value ) );
        case Qt::EditRole:
            return value;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
        case Role::EditorType:
            return Delegate::TimeEditor;
    }
    return QVariant();
}

bool CalendarDayItemModel::setIntervalEnd( TimeInterval *ti, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole: {
            int end = value.toInt();
            TimeInterval t = TimeInterval( ti->first, end );
            emit executeCommand( new CalendarModifyTimeIntervalCmd( m_calendar, t, ti,  "Modify Calendar Working Interval" ) );
            return true;
        }
    }
    return false;
}

QVariant CalendarDayItemModel::workDuration( const CalendarDay *day, int role ) const
{
    //kDebug()<<res->name()<<","<<role;
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole: {
            if ( day->state() == CalendarDay::Working ) {
                return KGlobal::locale()->formatNumber( day->workDuration().toDouble( Duration::Unit_h ), 1 ) + Duration::unitToString( Duration::Unit_h );
            }
            return QVariant();
        }
        case Qt::EditRole:
            break;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant CalendarDayItemModel::intervalDuration( const TimeInterval *ti, int role ) const
{
    //kDebug()<<res->name()<<","<<role;
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole: {
            Duration d( Duration( (qint64)(ti->second ) ) );
            return KGlobal::locale()->formatNumber( d.toDouble( Duration::Unit_h ), 1 ) + Duration::unitToString( Duration::Unit_h );
        }
        case Qt::EditRole:
            break;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}


QVariant CalendarDayItemModel::data( const QModelIndex &index, int role ) const
{
    QVariant result;
    if ( ! index.isValid() ) {
        return result;
    }
    if ( isTopLevel( index ) ) {
        switch ( index.column() ) {
            case 0: {
                if ( index.internalPointer() == typeWeekday ) {
                    result = typeWeekday->name;
                } else if ( index.internalPointer() == typeDate ) {
                    result = typeDate->name;
                } else kDebug()<<"data: invalid top level item"<<index.row();
                break;
            }
            case 1:
            case 2:
                result = QVariant();
                break;
            default:
                result = QVariant();
                kDebug()<<"data: invalid display value column"<<index.column();;
                break;
        }
    } else {
        CalendarDay *d = day( index );
        if ( d ) {
            switch ( index.column() ) {
                case 0: {
                    if ( isWeekday( d ) ) {
                        result = name( index.row() + 1, role ); // weekdays 1..7
                    } else if ( isDate( d ) ) {
                        result = date( d, role ); 
                    }
                    break;
                }
                case 1: result = dayState( d, role ); break;
                case 2: result = workDuration( d, role ); break;
                default:
                    kDebug()<<"data: invalid display value column"<<index.column();
            }
        } else {
            TimeInterval *ti = interval( index );
            if ( ti ) {
                switch ( index.column() ) {
                    case 0: result = intervalStart( ti, role ); break;
                    case 1: result = intervalEnd( ti, role ); break;
                    case 2: result = intervalDuration( ti, role ); break;
                    default:
                        kDebug()<<"data: invalid display value column"<<index.column();;
                }
            }
        }
    }
    if ( result.isValid() ) {
        if ( role == Qt::DisplayRole && result.type() == QVariant::String && result.toString().isEmpty()) {
            // HACK to show focus in empty cells
            result = " ";
        }
    }
    return result;
}

bool CalendarDayItemModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
    if ( ! index.isValid() || ( flags( index ) &Qt::ItemIsEditable ) == 0 ) {
        return false;
    }
    CalendarDay *d = day( index );
    if ( d ) {
        switch (index.column()) {
            case 0: return setDate( d, value, role );
            case 1: return setDayState( d, value, role );
            case 2: return false;
            default:
                kWarning()<<"data: invalid display value column "<<index.column()<<endl;
        }
        return false;
    }
    TimeInterval *ti = interval( index );
    if ( ti ) {
        switch ( index.column() ) {
/*            case 0: return false;
            case 1: return false;*/
            case 0: return setIntervalStart( ti, value, role );
            case 1: return setIntervalEnd( ti, value, role );
            case 2: return false;
            default:
                kWarning()<<"data: invalid display value column "<<index.column()<<endl;
        }
        return false;
    }
    return false;
}

QVariant CalendarDayItemModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal ) {
        if ( role == Qt::DisplayRole ) {
            switch ( section ) {
                case 0: return i18n( "Day" );
                case 1: return i18n( "Type" );
                case 2: return i18n( "Hours" );
/*                case 3: return QVariant();//i18n( "End" );*/
                default: return QVariant();
            }
        } else if ( role == Qt::TextAlignmentRole ) {
            switch (section) {
                default: return QVariant();
            }
        }
    }
    if ( role == Qt::ToolTipRole ) {
        switch ( section ) {
       /*     case 0: return ToolTip::Calendar Name;*/
            default: return QVariant();
        }
    }
    return ItemModelBase::headerData(section, orientation, role);
}

void CalendarDayItemModel::sort( int , Qt::SortOrder  )
{
}

CalendarDay *CalendarDayItemModel::day( const QModelIndex &index ) const
{
    CalendarDay *d = static_cast<CalendarDay*>( index.internalPointer() );
    if ( d == 0 ) {
        return 0;
    }
    if ( ! isWeekday( d ) && ! isDate( d ) ) {
        return 0;
    }
    return d;
}

TimeInterval *CalendarDayItemModel::interval( const QModelIndex &index ) const
{
    TimeInterval *ti = static_cast<TimeInterval*>( index.internalPointer() );
    if ( ti ==  0 || ! m_days.contains( ti ) ) {
        return 0;
    }
    return ti;
}

QItemDelegate *CalendarDayItemModel::createDelegate( int column, QWidget *parent ) const
{
    switch ( column ) {
        case 1: return new SelectorDelegate( parent );
/*        case 2: return new TimeDelegate( parent );
        case 3: return new TimeDelegate( parent );*/
        default: return 0;
    }
    return 0;
}

void CalendarDayItemModel::addIntervals( CalendarDay *day )
{
    if ( day->state() == CalendarDay::Working ) {
        foreach ( TimeInterval *i, day->workingIntervals() ) {
            m_days.insert( i, day );
        }
    }
}

void CalendarDayItemModel::removeIntervals( CalendarDay *day )
{
    QList<TimeInterval*> keys;
    QMapIterator<TimeInterval*, CalendarDay*> d( m_days );
    while ( d.findNext( day ) ) {
        keys.append( d.key() );
    }
    foreach ( TimeInterval *k, keys ) {
        m_days.remove( k );
    }
}

QModelIndex CalendarDayItemModel::insertInterval( TimeInterval *ti, CalendarDay *day )
{
    //kDebug();
    if ( day->state() == CalendarDay::Working ) {
        emit executeCommand( new CalendarAddTimeIntervalCmd( m_calendar, day, ti, i18n( "Add Work Interval" ) ) );
        int row = day->indexOf( ti );
        if ( row != -1 ) {
            return createIndex( row, 0, ti );
        }
    }
    return QModelIndex();
}

void CalendarDayItemModel::removeInterval( TimeInterval *ti )
{
    //kDebug();
    CalendarDay *d = parentDay( ti );
    if ( d == 0 ) {
        return;
    }
    emit executeCommand( new CalendarRemoveTimeIntervalCmd( m_calendar, d, ti, i18n( "Remove Work Interval" ) ) );
}

QModelIndex CalendarDayItemModel::insertDay( CalendarDay *day )
{
    //kDebug();
    emit executeCommand( new CalendarAddDayCmd( m_calendar, day, i18n( "Add Calendar Day" ) ) );
    int row = m_calendar->indexOf( day );
    if ( row != -1 ) {
        return createIndex( row, 0, day );
    }
    return QModelIndex();
}

void CalendarDayItemModel::removeDay( CalendarDay *day )
{
    //kDebug();
    emit executeCommand( new CalendarRemoveDayCmd( m_calendar, day, i18n( "Remove Calendar Day" ) ) );
}

void CalendarDayItemModel::setDayMap( Calendar *calendar ) {
    m_days.clear();
    if ( calendar == 0 ) {
        return;
    }
    foreach ( CalendarDay *d, calendar->weekdayList() ) {
        addIntervals( d );
    }
    foreach ( CalendarDay *d, calendar->days() ) {
        addIntervals( d );
    }
}

void CalendarDayItemModel::setDayMap( CalendarDay *day )
{
    clearDayMap( day );
    if ( day->state() == CalendarDay::Working ) {
        foreach ( TimeInterval *i, day->workingIntervals() ) {
            //kDebug()<<day->date()<<i;
            m_days.insert( i, day );
        }
    }
}

void CalendarDayItemModel::clearDayMap( CalendarDay *day )
{
    QList<TimeInterval*> keys;
    QMapIterator<TimeInterval*, CalendarDay*> d( m_days );
    while ( d.findNext( day ) ) {
        keys.append( d.key() );
    }
    foreach ( TimeInterval *k, keys ) {
        //kDebug()<<day->date()<<k;
        m_days.remove( k );
    }
}


} // namespace KPlato

#include "kptcalendarmodel.moc"
