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
#include "kptcommonstrings.h"
#include "kptcommand.h"
#include "kptitemmodelbase.h"
#include "kptcalendar.h"
#include "kptduration.h"
#include "kptnode.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptdatetime.h"
#include "kcalendar/kdatetable.h"

#include <QMimeData>
#include <QList>
#include <QObject>
#include <QPainter>

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
    reset();
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
    reset();
}

Qt::ItemFlags CalendarItemModel::flags( const QModelIndex &index ) const
{
    Qt::ItemFlags flags = ItemModelBase::flags( index );
    if ( !m_readWrite ) {
        return flags &= ~Qt::ItemIsEditable;
    }
    flags |= Qt::ItemIsDropEnabled;
    if ( !index.isValid() ) {
        return flags;
    }
    flags |= Qt::ItemIsDragEnabled;
    if ( calendar ( index ) ) {
        switch ( index.column() ) {
            case 0:/*Name*/
                flags |= ( Qt::ItemIsEditable | Qt::ItemIsUserCheckable );
                break;
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
            return a->name();
        case Qt::ToolTipRole:
            if ( a->isDefault() ) {
                return i18nc( "1=calendar name", "%1 (Default calendar)", a->name() );
            }
            return a->name();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
        case Qt::CheckStateRole:
            return a->isDefault() ? Qt::Checked : Qt::Unchecked;
    }
    return QVariant();
}

bool CalendarItemModel::setName( Calendar *a, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            if ( value.toString() != a->name() ) {
                emit executeCommand( new CalendarModifyNameCmd( a, value.toString(), i18n( "Modify Calendar Name" ) ) );
                return true;
            }
            break;
        case Qt::CheckStateRole: {
            switch ( value.toInt() ) {
                case Qt::Unchecked:
                    if ( a->isDefault() ) {
                        emit executeCommand( new ProjectModifyDefaultCalendarCmd( m_project, 0, i18n( "De-select as default calendar" ) ) );
                        return true;
                    }
                    break;
                case Qt::Checked:
                    if ( ! a->isDefault() ) {
                        emit executeCommand( new ProjectModifyDefaultCalendarCmd( m_project, a, i18n( "Select as default calendar" ) ) );
                        return true;
                    }
                    break;
                default: break;
            }
        }
        default: break;
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
            foreach ( const QString &s, KSystemTimeZones::timeZones()->zones().keys() ) {
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
            if ( timeZone( a, Role::EnumListValue ) == value.toInt() ) {
                return false;
            }
            QStringList lst = timeZone( a, Role::EnumList ).toStringList();
            QString name = lst.value( value.toInt() );
            KTimeZone tz;
            foreach ( const QString &s, KSystemTimeZones::timeZones()->zones().keys() ) {
                if ( name == i18n( s.toUtf8() ) ) {
                    tz = KSystemTimeZones::zone( s );
                    break;
                }
            }
            if ( !tz.isValid() ) {
                return false;
            }
            emit executeCommand( new CalendarModifyTimeZoneCmd( a, tz, i18n( "Modify Calendar Timezone" ) ) );
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
    if ( ! index.isValid() ) {
        return ItemModelBase::setData( index, value, role );
    }
    if ( ( flags( index ) &( Qt::ItemIsEditable | Qt::CheckStateRole ) ) == 0 ) {
        Q_ASSERT( true );
        return false;
    }
    Calendar *a = calendar( index );
    switch (index.column()) {
        case 0: return setName( a, value, role );
        case 1: return setTimeZone( a, value, role );
        default:
            kWarning()<<"data: invalid display value column "<<index.column();
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
    foreach (const QModelIndex &index, indexes) {
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
            if ( c->parentCal() != par ) {
                if ( cmd == 0 ) cmd = new MacroCommand( i18n( "Re-parent Calendar" ) );
                cmd->addCommand( new CalendarModifyParentCmd( m_project, c, par ) );
            }
        }
        if ( cmd ) {
            emit executeCommand( cmd );
            return true;
        }
        //kDebug()<<row<<","<<column<<" parent="<<parent.row()<<","<<parent.column()<<":"<<par->name();
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
    kDebug()<<on<<data->hasFormat("application/x-vnd.kde.kplato.calendarid.internal");
    if ( !data->hasFormat("application/x-vnd.kde.kplato.calendarid.internal") ) {
        return false;
    }
    QByteArray encodedData = data->data( "application/x-vnd.kde.kplato.calendarid.internal" );
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    QList<Calendar*> lst = calendarList( stream );
    foreach ( Calendar *c, lst ) {
        if ( (flags( index( c ) ) & (int)Qt::ItemIsDropEnabled) == 0 ) {
            return false;
        }
        if ( on == c->parentCal() ) {
            return false;
        }
        if ( on != 0 && ( on == c || on->isChildOf( c ) ) ) {
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
}

CalendarDayItemModel::~CalendarDayItemModel()
{
}

void CalendarDayItemModel::slotWorkIntervalAdded( CalendarDay *day, TimeInterval *ti )
{
    //kDebug()<<day<<","<<ti;
    int c = m_calendar->indexOfWeekday( day );
    if ( c == -1 ) {
        return;
    }
    dataChanged( createIndex( 0, c, day ), createIndex( 0, c, day ) );
}

void CalendarDayItemModel::slotWorkIntervalRemoved( CalendarDay *day, TimeInterval *ti )
{
    int c = m_calendar->indexOfWeekday( day );
    if ( c == -1 ) {
        return;
    }
    dataChanged( createIndex( 0, c, day ), createIndex( 0, c, day ) );
}

void CalendarDayItemModel::slotDayChanged( CalendarDay *day )
{
    int c = m_calendar->indexOfWeekday( day );
    if ( c == -1 ) {
        return;
    }
    kDebug()<<day<<", "<<c;
    emit dataChanged( createIndex( 0, c, day ), createIndex( 0, c, day ) );
}

void CalendarDayItemModel::slotTimeIntervalChanged( TimeInterval *ti )
{
/*    CalendarDay *d = parentDay( ti );
    if ( d == 0 ) {
        return;
    }
    int row = d->indexOf( ti );
    emit dataChanged( createIndex( row, 0, ti ), createIndex( row, columnCount() - 1, ti ) );*/
}

void CalendarDayItemModel::setCalendar( Calendar *calendar )
{
    //kDebug()<<m_calendar<<" -->"<<calendar;
    if ( m_calendar ) {
        disconnect( m_calendar, SIGNAL( changed( CalendarDay*) ), this, SLOT( slotDayChanged( CalendarDay* ) ) );
        disconnect( m_calendar, SIGNAL( changed( TimeInterval* ) ), this, SLOT( slotTimeIntervalChanged( TimeInterval* ) ) );

        disconnect( m_calendar, SIGNAL( workIntervalAdded( CalendarDay*, TimeInterval* ) ), this, SLOT( slotWorkIntervalAdded( CalendarDay*, TimeInterval* ) ) );
        disconnect( m_calendar, SIGNAL( workIntervalRemoved( CalendarDay*, TimeInterval* ) ), this, SLOT( slotWorkIntervalRemoved( CalendarDay*, TimeInterval* ) ) );
    }
    m_calendar = calendar;
    if ( calendar ) {
        connect( m_calendar, SIGNAL( changed( CalendarDay*) ), this, SLOT( slotDayChanged( CalendarDay* ) ) );
        connect( m_calendar, SIGNAL( changed( TimeInterval* ) ), this, SLOT( slotTimeIntervalChanged( TimeInterval* ) ) );

        connect( m_calendar, SIGNAL( workIntervalAdded( CalendarDay*, TimeInterval* ) ), this, SLOT( slotWorkIntervalAdded( CalendarDay*, TimeInterval* ) ) );
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
    return flags |= Qt::ItemIsEditable;
}

QModelIndex CalendarDayItemModel::parent( const QModelIndex &index ) const
{
    return QModelIndex();
}

bool CalendarDayItemModel::hasChildren( const QModelIndex &parent ) const
{
    //kDebug()<<parent.internalPointer()<<":"<<parent.row()<<","<<parent.column();
    if ( m_project == 0 || m_calendar == 0 ) {
        return false;
    }
    return ! parent.isValid();
}

QModelIndex CalendarDayItemModel::index( int row, int column, const QModelIndex &par ) const
{
    if ( m_project == 0 || m_calendar == 0 ) {
        return QModelIndex();
    }
    if ( par.isValid() ) {
        return QModelIndex();
    }
    CalendarDay *d = m_calendar->weekday( column + 1 ); // weekdays are 1..7
    if ( d == 0 ) {
        return QModelIndex();
    }
    return createIndex( row, column, d );
}

QModelIndex CalendarDayItemModel::index( const CalendarDay *d) const
{
    if ( m_project == 0 || m_calendar == 0 ) {
        return QModelIndex();
    }
    int col = m_calendar->indexOfWeekday( d );
    if ( col == -1 ) {
        return QModelIndex();
    }
    return createIndex( 0, col, const_cast<CalendarDay*>( d ) );
}

int CalendarDayItemModel::columnCount( const QModelIndex &/*parent*/ ) const
{
    return 7;
}

int CalendarDayItemModel::rowCount( const QModelIndex &parent ) const
{
    if ( m_project == 0 || m_calendar == 0 || parent.isValid() ) {
        return 0;
    }
    return 1;
}

QVariant CalendarDayItemModel::name( int weekday, int role ) const
{
    //kDebug()<<res->name()<<","<<role;
    switch ( role ) {
        case Qt::DisplayRole:
            if ( weekday >= 1 && weekday <= 7 ) {
                return KGlobal::locale()->calendar()->weekDayName( weekday, KCalendarSystem::ShortDayName );
            }
            break;
        case Qt::ToolTipRole:
            if ( weekday >= 1 && weekday <= 7 ) {
                return KGlobal::locale()->calendar()->weekDayName( weekday );
            }
            break;
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
            switch ( d->state() ) {
                case CalendarDay::Undefined: return i18nc( "Undefined", "U" );
                case CalendarDay::NonWorking: return i18nc( "NonWorking", "NW" );
                case CalendarDay::Working: return i18nc( "Working", "W" );
            }
            break;
        case Qt::ToolTipRole:
            return CalendarDay::stateToString( d->state(), true );
        case Role::EnumList: {
            QStringList lst = CalendarDay::stateList( true );
            return lst;
        }
        case Qt::EditRole:
        case Role::EnumListValue: {
            return d->state();
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
            emit executeCommand( new CalendarModifyStateCmd( m_calendar, d, static_cast<CalendarDay::State>( v ), i18n( "Modify Calendar State" ) ) );
            return true;
    }
    return false;
}

QVariant CalendarDayItemModel::workDuration( const CalendarDay *day, int role ) const
{
    //kDebug()<<day->date()<<","<<role;
    switch ( role ) {
        case Qt::DisplayRole: {
            if ( day->state() == CalendarDay::Working ) {
                return KGlobal::locale()->formatNumber( day->workDuration().toDouble( Duration::Unit_h ), 1 );
            }
            return QVariant();
        }
        case Qt::ToolTipRole: {
            if ( day->state() == CalendarDay::Working ) {
                KLocale *l = KGlobal::locale();
                QStringList tip;
                foreach ( TimeInterval *i, day->workingIntervals() ) {
                    tip <<  i18nc( "1=time 2=The number of hours of work duration (non integer)", "%1, %2 hours", l->formatTime( i->startTime() ), l->formatNumber( i->hours() ) );
                }
                return tip.join( "\n" );
            }
            return QVariant();
        }
        case Qt::EditRole:
            break;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
    }
    return QVariant();
}

QVariant CalendarDayItemModel::data( const QModelIndex &index, int role ) const
{
    QVariant result;
    if ( ! index.isValid() ) {
        return result;
    }
    CalendarDay *d = day( index );
    if ( d ) {
        switch ( d->state() ) {
            case CalendarDay::Working:
                result = workDuration( d, role );
                break;
            default:
                result = dayState( d, role );
                break;
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
    return ItemModelBase::setData( index, value, role );
}

QVariant CalendarDayItemModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal ) {
        if ( role == Qt::DisplayRole ) {
            switch ( section ) {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                    return name( section + 1, role );
                default:
                    return QVariant();
             }
         } else if ( role == Qt::TextAlignmentRole ) {
             switch (section) {
                default: return Qt::AlignCenter;
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

CalendarDay *CalendarDayItemModel::day( const QModelIndex &index ) const
{
    return static_cast<CalendarDay*>( index.internalPointer() );
}

QAbstractItemDelegate *CalendarDayItemModel::createDelegate( int column, QWidget *parent ) const
{
    switch ( column ) {
        default: return 0;
    }
    return 0;
}

//-----------------------
DateTableDataModel::DateTableDataModel( QObject *parent )
    : KDateTableDataModel( parent ),
    m_calendar( 0 )
{
}

void DateTableDataModel::setCalendar( Calendar *calendar )
{
    if ( m_calendar ) {
        disconnect( m_calendar, SIGNAL( dayAdded( CalendarDay* ) ), this, SIGNAL( reset() ) );
        disconnect( m_calendar, SIGNAL( dayRemoved( CalendarDay* ) ), this, SIGNAL( reset() ) );
        disconnect( m_calendar, SIGNAL( changed( CalendarDay* ) ), this, SIGNAL( reset() ) );
    }
    m_calendar = calendar;
    if ( m_calendar ) {
        connect( m_calendar, SIGNAL( dayAdded( CalendarDay* ) ), this, SIGNAL( reset() ) );
        connect( m_calendar, SIGNAL( dayRemoved( CalendarDay* ) ), this, SIGNAL( reset() ) );
        connect( m_calendar, SIGNAL( changed( CalendarDay* ) ), this, SIGNAL( reset() ) );
    }
    emit reset();
}

QVariant DateTableDataModel::data( const QDate &date, int role, int dataType ) const
{
    //kDebug()<<date<<role<<dataType;
    if ( role ==  Qt::ToolTipRole ) {
        if ( m_calendar == 0 ) {
            return QVariant();
        }
        CalendarDay *day = m_calendar->findDay( date );
        if ( day == 0 || day->state() == CalendarDay::Undefined ) {
            return i18n( "Undefined" );
        }
        if ( day->state() == CalendarDay::NonWorking ) {
            return i18n( "Non-working" );
        }
        KLocale *l = KGlobal::locale();
        QStringList tip;
        foreach ( TimeInterval *i, day->workingIntervals() ) {
                    tip <<  i18nc( "1=time 2=The number of hours of work duration (non integer)", "%1, %2 hours", l->formatTime( i->startTime() ), l->formatNumber( i->hours() ) );
        }
        return tip.join( "\n" );
    }

    switch ( dataType ) {
        case -1: { //default (date)
            switch ( role ) {
                case Qt::DisplayRole: {
                    return QVariant();
                }
                case Qt::TextAlignmentRole:
                    return (uint)Qt::AlignLeft | Qt::AlignTop;
                case Qt::FontRole:
                    break;//return QFont( "Helvetica", 6 );
                case Qt::BackgroundRole:
                    break;//return QColor( "red" );
                default:
                    break;
            }
            break;
        }
        case 0: {
            switch ( role ) {
                case Qt::DisplayRole: {
                    if ( m_calendar == 0 ) {
                        return "";
                    }
                    CalendarDay *day = m_calendar->findDay( date );
                    if ( day == 0 || day->state() == CalendarDay::Undefined ) {
                        return "";
                    }
                    if ( day->state() == CalendarDay::NonWorking ) {
                        return i18nc( "NonWorking", "NW" );
                    }
                    double v;
                    v = day->workDuration().toDouble( Duration::Unit_h );
                    return KGlobal::locale()->formatNumber( v, 1 );
                }
                case Qt::TextAlignmentRole:
                    return (uint)( Qt::AlignHCenter | Qt::AlignBottom );
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }
    return QVariant();
}

QVariant DateTableDataModel::weekDayData( int day, int role ) const
{
    return QVariant();
}

QVariant DateTableDataModel::weekNumberData( int week, int role ) const
{
    return QVariant();
}

//-------------
DateTableDateDelegate::DateTableDateDelegate( QObject *parent )
    : KDateTableDateDelegate( parent )
{
}

QRectF DateTableDateDelegate::paint( QPainter *painter, const StyleOptionViewItem &option, const QDate &date, KDateTableDataModel *model )
{
    //kDebug()<<date;
    QRectF r;
    StyleOptionViewItem style = option;
    style.font.setPointSize( style.font.pointSize() - 2 );
    //kDebug()<<" fonts: "<<option.font.pointSize()<<style.font.pointSize();
    r = KDateTableDateDelegate::paint( painter, style, date, model );
    if ( model == 0 ) {
        return r;
    }
    painter->save();
    const KCalendarSystem * calendar = KGlobal::locale()->calendar();

    painter->translate( r.width(), 0.0 );
    QRectF rect( 1, 1, option.rectF.right() - r.width(), option.rectF.bottom() );
    //kDebug()<<" rects: "<<r<<rect;

    QString text = model->data( date, Qt::DisplayRole, 0 ).toString();
    int align = model->data( date, Qt::TextAlignmentRole, 0 ).toInt();
    painter->setPen( option.palette.color( QPalette::Text ) );
    painter->drawText(rect, align, text, &r);

    painter->restore();
    return r;
}

} // namespace KPlato

#include "kptcalendarmodel.moc"
