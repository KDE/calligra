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

#include "kptcalendareditor.h"

#include "kptcommand.h"
#include "kptitemmodelbase.h"
#include "kptcalendar.h"
#include "kptduration.h"
#include "kptfactory.h"
#include "kptview.h"
#include "kptnode.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptdatetime.h"
#include "kptcontext.h"

#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QMenu>
#include <QPainter>
#include <QStyle>
#include <QList>
#include <QHeaderView>
#include <QObject>
#include <QTreeWidget>
#include <QStringList>
#include <QSplitter>
#include <QVBoxLayout>

#include <kicon.h>
#include <kglobal.h>
#include <klocale.h>
#include <kcalendarsystem.h>
#include <kprinter.h>
#include <kactioncollection.h>
#include <kxmlguifactory.h>
#include <ksystemtimezone.h>
#include <ktimezones.h>
#include <kabc/addressee.h>
#include <kabc/vcardconverter.h>

#include <kdebug.h>

namespace KPlato
{


//-----------------------------------------
CalendarDayItemModelBase::CalendarDayItemModelBase( Part *part, QObject *parent )
    : ItemModelBase( part, parent ),
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
CalendarItemModel::CalendarItemModel( Part *part, QObject *parent )
    : ItemModelBase( part, parent ),
    m_calendar( 0 )
{
}

CalendarItemModel::~CalendarItemModel()
{
}

void CalendarItemModel::slotCalendarToBeInserted( const Calendar *parent, int row )
{
    //kDebug()<<k_funcinfo<<(parent?parent->name():"Top level")<<", "<<row<<endl;
    Q_ASSERT( m_calendar == 0 );
    m_calendar = const_cast<Calendar *>(parent);
    beginInsertRows( index( parent ), row, row );
}

void CalendarItemModel::slotCalendarInserted( const Calendar *calendar )
{
    //kDebug()<<k_funcinfo<<calendar->name()<<endl;
    Q_ASSERT( calendar->parentCal() == m_calendar );
    endInsertRows();
    m_calendar = 0;
}

void CalendarItemModel::slotCalendarToBeRemoved( const Calendar *calendar )
{
    //kDebug()<<k_funcinfo<<calendar->name()<<endl;
    int row = index( calendar ).row();
    beginRemoveRows( index( calendar->parentCal() ), row, row );
}

void CalendarItemModel::slotCalendarRemoved( const Calendar *calendar )
{
    //kDebug()<<k_funcinfo<<calendar->name()<<endl;
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
    //kDebug()<<k_funcinfo<<index.internalPointer()<<": "<<index.row()<<", "<<index.column()<<endl;
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
        //kDebug()<<k_funcinfo<<par->name()<<": "<<row<<endl;
        return createIndex( row, 0, par );
    }
    return QModelIndex();
}

bool CalendarItemModel::hasChildren( const QModelIndex &parent ) const
{
    //kDebug()<<k_funcinfo<<parent.internalPointer()<<": "<<parent.row()<<", "<<parent.column()<<endl;
    if ( m_project == 0 ) {
        return false;
    }
    Calendar *par = calendar( parent );
    if ( par == 0 ) {
        return ! m_project->calendars().isEmpty();
    }
    return ! par->calendars().isEmpty();
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

int CalendarItemModel::columnCount( const QModelIndex &parent ) const
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

bool CalendarItemModel::insertRows( int row, int count, const QModelIndex &parent )
{
//TODO
    return false;
}

bool CalendarItemModel::removeRows( int row, int count, const QModelIndex &parent )
{
//TODO
    return false;
}

QVariant CalendarItemModel::name( const Calendar *a, int role ) const
{
    //kDebug()<<k_funcinfo<<res->name()<<", "<<role<<endl;
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
                m_part->addCommand( new CalendarModifyNameCmd( m_part, a, value.toString(), "Modify Calendar Name" ) );
            }
            return true;
    }
    return false;
}

QVariant CalendarItemModel::timeZone( const Calendar *a, int role ) const
{
    //kDebug()<<k_funcinfo<<res->name()<<", "<<role<<endl;
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return i18n( a->timeZone()->name().toUtf8() );
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
            return lst.indexOf( i18n ( a->timeZone()->name().toUtf8() ) );
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
            const KTimeZone *tz = 0;
            foreach ( QString s, KSystemTimeZones::timeZones()->zones().keys() ) {
                if ( name == i18n( s.toUtf8() ) ) {
                    tz = KSystemTimeZones::zone( s );
                    break;
                }
            }
            if ( tz == 0 ) {
                return false;
            }
            m_part->addCommand( new CalendarModifyTimeZoneCmd( m_part, a, tz, "Modify Calendar Timezone" ) );
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
            kDebug()<<k_funcinfo<<"data: invalid display value column "<<index.column()<<endl;;
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
       /*     case 0: return ToolTip::Calendar Name;*/
            default: return QVariant();
        }
    }
    return ItemModelBase::headerData(section, orientation, role);
}

void CalendarItemModel::sort( int column, Qt::SortOrder order )
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
            kDebug()<<k_funcinfo<<index.row()<<endl;
            Calendar *c = calendar( index );
            if ( c ) {
                stream << c->id();
            }
        }
    }
    m->setData("application/x-vnd.kde.kplato.calendarid.internal", encodedData);
    return m;
}

bool CalendarItemModel::dropMimeData( const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent )
{
    kDebug()<<k_funcinfo<<action<<endl;
    if (action == Qt::IgnoreAction) {
        return true;
    }
    if ( !data->hasFormat( "application/x-vnd.kde.kplato.calendarid.internal" ) ) {
        return false;
    }
    if ( action == Qt::MoveAction ) {
        kDebug()<<k_funcinfo<<"MoveAction"<<endl;
        
        QByteArray encodedData = data->data( "application/x-vnd.kde.kplato.calendarid.internal" );
        QDataStream stream(&encodedData, QIODevice::ReadOnly);
        Calendar *par = 0;
        if ( parent.isValid() ) {
            par = calendar( parent );
        }
        K3MacroCommand *cmd = 0;
        QList<Calendar*> lst = calendarList( stream );
        foreach ( Calendar *c, lst ) {
            if ( cmd == 0 ) cmd = new K3MacroCommand( i18n( "Re-parent Calendar" ) );
            cmd->addCommand( new CalendarModifyParentCmd( m_part, m_project, c, par ) );
        }
        if ( cmd ) {
            m_part->addCommand( cmd );
        }
        //kDebug()<<k_funcinfo<<row<<", "<<column<<" parent="<<parent.row()<<", "<<parent.column()<<": "<<par->name()<<endl;
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
    //kDebug()<<k_funcinfo<<endl;
    m_part->addCommand( new CalendarAddCmd( m_part, m_project, calendar, parent, i18n( "Add Calendar" ) ) );
    int row = -1;
    if ( parent ) {
        row = parent->indexOf( calendar );
    } else {
        row = m_project->indexOf( calendar );
    }
    if ( row != -1 ) {
        //kDebug()<<k_funcinfo<<"Inserted: "<<calendar->name()<<endl;
        return createIndex( row, 0, calendar );
    }
    return QModelIndex();
}

void CalendarItemModel::removeCalendar( QList<Calendar *> /*lst*/ )
{
/*    K3MacroCommand *cmd = 0;
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
            if ( cmd == 0 ) cmd = new K3MacroCommand( s );
            cmd->addCommand( new CalendarRemoveCmd( m_part, m_project, c ) );
        }
    }
    if ( cmd )
        m_part->addCommand( cmd );*/
}

void CalendarItemModel::removeCalendar( Calendar *calendar )
{
    if ( calendar == 0 ) {
        return;
    }
    m_part->addCommand( new CalendarRemoveCmd( m_part, m_project, calendar, i18n( "Delete Calendar" ) ) );
}


//------------------------------------------
CalendarDayItemModel::CalendarDayItemModel( Part *part, QObject *parent )
    : CalendarDayItemModelBase( part, parent )
{
    typeWeekday = new TopLevelType( i18n( "Weekdays" ) );
    typeDate = new TopLevelType(  i18n( "Days" ) );
}

CalendarDayItemModel::~CalendarDayItemModel()
{
}

void CalendarDayItemModel::slotDayToBeAdded( CalendarDay *day, int row )
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
    //kDebug()<<k_funcinfo<<day->date()<<", "<<row<<endl;
}

void CalendarDayItemModel::slotDayRemoved( CalendarDay *day )
{
    //kDebug()<<k_funcinfo<<day->date()<<endl;
    endRemoveRows();
}

void CalendarDayItemModel::slotWorkIntervalToBeAdded( CalendarDay *day, TimeInterval *ti, int row )
{
    int r = m_calendar->indexOfWeekday( day );
    if ( r == -1 ) {
        r = m_calendar->indexOf( day );
    }
    //kDebug()<<k_funcinfo<<day<<", "<<ti<<", "<<row<<endl;
    beginInsertRows( createIndex( r, 0, day ), row, row );
}

void CalendarDayItemModel::slotWorkIntervalAdded( CalendarDay *day, TimeInterval *ti )
{
    //kDebug()<<k_funcinfo<<day<<", "<<ti<<endl;
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
    kDebug()<<k_funcinfo<<day<<", "<<row<<endl;
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
    //kDebug()<<k_funcinfo<<m_calendar<<" --> "<<calendar<<endl;
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
        kError()<<k_funcinfo<<1<<endl;
        return QModelIndex();
    }
    TimeInterval *ti = interval( index );
    if ( ti == 0 ) {
        kError()<<k_funcinfo<<2<<endl;
        return QModelIndex();
    }
    d = parentDay( ti );
    if ( d == 0 ) {
        kError()<<k_funcinfo<<3<<endl;
        return QModelIndex();
    }
    int row = -1;
    if ( isWeekday( d ) ) {
        row = m_calendar->indexOfWeekday( d );
    } else if ( isDate( d ) ) {
        row = m_calendar->indexOf( d );
    } else {
        kError()<<k_funcinfo<<4<<endl;
        return QModelIndex();
    }
    return createIndex( row, 0, d );
}

bool CalendarDayItemModel::hasChildren( const QModelIndex &parent ) const
{
    //kDebug()<<k_funcinfo<<parent.internalPointer()<<": "<<parent.row()<<", "<<parent.column()<<endl;
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
    //kDebug()<<k_funcinfo<<res->name()<<", "<<role<<endl;
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
            //kDebug()<<k_funcinfo<<d<<endl;
            m_part->addCommand( new CalendarModifyDateCmd( m_part, m_calendar, d, date,  "Modify Calendar Date" ) );
            return true;
        }
    }
    return false;
}

QVariant CalendarDayItemModel::name( int weekday, int role ) const
{
    //kDebug()<<k_funcinfo<<res->name()<<", "<<role<<endl;
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
    //kDebug()<<k_funcinfo<<endl;
    switch ( role ) {
        case Qt::EditRole:
            int v = value.toInt();
            if ( isDate ( d ) ) {
                if ( v >= CalendarDay::Undefined )
                    v++; // Undefined not in list
            }
            m_part->addCommand( new CalendarModifyStateCmd( m_part, m_calendar, d, static_cast<CalendarDay::State>( v ), "Modify Calendar State" ) );
            return true;
    }
    return false;
}

QVariant CalendarDayItemModel::intervalStart( const TimeInterval *ti, int role ) const
{
    //kDebug()<<k_funcinfo<<res->name()<<", "<<role<<endl;
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
            if ( start > t.second ) {
                t.second = start;
            }
            CalendarDay *d = parentDay( ti );
            m_part->addCommand( new CalendarModifyTimeIntervalCmd( m_part, m_calendar, t, ti,  "Modify Calendar Working Interval" ) );
            return true;
        }
    }
    return false;
}

QVariant CalendarDayItemModel::intervalEnd( const TimeInterval *ti, int role ) const
{
    //kDebug()<<k_funcinfo<<res->name()<<", "<<role<<endl;
    switch ( role ) {
        case Qt::DisplayRole:
            return ti->second;
        case Qt::ToolTipRole:
        case Qt::EditRole:
            return ti->second;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
        case Role::Minimum: {
            return QTime();
/*            CalendarDay *d = m_days.value( const_cast<TimeInterval*>( ti ) );
            if ( d == 0 ) {
                return QTime();
            }
            return d->minEndTime( ti );*/
        }
        case Role::Maximum: {
            return QTime();
/*            CalendarDay *d = m_days.value( const_cast<TimeInterval*>( ti ) );
            if ( d == 0 ) {
                return QTime();
            }
            return d->maxEndTime( ti );*/
        }
        case Role::EditorType:
            return Delegate::TimeEditor;
    }
    return QVariant();
}

bool CalendarDayItemModel::setIntervalEnd( TimeInterval *ti, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole: {
            QTime end = value.toTime();
            TimeInterval t = TimeInterval( ti->first, end );
            if ( end < t.first ) {
                t.first = end;
            }
            CalendarDay *d = parentDay( ti );
            m_part->addCommand( new CalendarModifyTimeIntervalCmd( m_part, m_calendar, t, ti,  "Modify Calendar Working Interval" ) );
            return true;
        }
    }
    return false;
}

QVariant CalendarDayItemModel::workDuration( const CalendarDay *day, int role ) const
{
    //kDebug()<<k_funcinfo<<res->name()<<", "<<role<<endl;
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
    //kDebug()<<k_funcinfo<<res->name()<<", "<<role<<endl;
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole: {
            Duration d( Duration( ti->first.msecsTo( ti->second ) ) );
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
                } else kDebug()<<k_funcinfo<<"data: invalid top level item "<<index.row()<<endl;
                break;
            }
            case 1:
            case 2:
                result = QVariant();
                break;
            default:
                result = QVariant();
                kDebug()<<k_funcinfo<<"data: invalid display value column "<<index.column()<<endl;;
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
                    kDebug()<<k_funcinfo<<"data: invalid display value column "<<index.column()<<endl;
            }
        } else {
            TimeInterval *ti = interval( index );
            if ( ti ) {
                switch ( index.column() ) {
                    case 0: result = intervalStart( ti, role ); break;
                    case 1: result = intervalEnd( ti, role ); break;
                    case 2: result = intervalDuration( ti, role ); break;
                    default:
                        kDebug()<<k_funcinfo<<"data: invalid display value column "<<index.column()<<endl;;
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

void CalendarDayItemModel::sort( int column, Qt::SortOrder order )
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
    //kDebug()<<k_funcinfo<<endl;
    if ( day->state() == CalendarDay::Working ) {
        m_part->addCommand( new CalendarAddTimeIntervalCmd( m_part, m_calendar, day, ti, i18n( "Add Work Interval" ) ) );
        int row = day->indexOf( ti );
        if ( row != -1 ) {
            return createIndex( row, 0, ti );
        }
    }
    return QModelIndex();
}

void CalendarDayItemModel::removeInterval( TimeInterval *ti )
{
    //kDebug()<<k_funcinfo<<endl;
    CalendarDay *d = parentDay( ti );
    if ( d == 0 ) {
        return;
    }
    m_part->addCommand( new CalendarRemoveTimeIntervalCmd( m_part, m_calendar, d, ti, i18n( "Remove Work Interval" ) ) );
}

QModelIndex CalendarDayItemModel::insertDay( CalendarDay *day )
{
    //kDebug()<<k_funcinfo<<endl;
    m_part->addCommand( new CalendarAddDayCmd( m_part, m_calendar, day, i18n( "Add Calendar Day" ) ) );
    int row = m_calendar->indexOf( day );
    if ( row != -1 ) {
        return createIndex( row, 0, day );
    }
    return QModelIndex();
}

void CalendarDayItemModel::removeDay( CalendarDay *day )
{
    //kDebug()<<k_funcinfo<<endl;
    m_part->addCommand( new CalendarRemoveDayCmd( m_part, m_calendar, day, i18n( "Remove Calendar Day" ) ) );
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
            //kDebug()<<k_funcinfo<<day->date()<<i<<endl;
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
        //kDebug()<<k_funcinfo<<day->date()<<k<<endl;
        m_days.remove( k );
    }
}


//--------------------
CalendarTreeView::CalendarTreeView( Part *part, QWidget *parent )
    : TreeViewBase( parent )
{
    header()->setContextMenuPolicy( Qt::CustomContextMenu );
    setModel( new CalendarItemModel( part ) );
    
    setSelectionBehavior( QAbstractItemView::SelectRows );
    setSelectionMode( QAbstractItemView::SingleSelection );
    setSelectionModel( new QItemSelectionModel( model() ) );

    setItemDelegateForColumn( 1, new EnumDelegate( this ) ); // timezone
    
    connect( header(), SIGNAL( customContextMenuRequested ( const QPoint& ) ), this, SLOT( headerContextMenuRequested( const QPoint& ) ) );
    connect( this, SIGNAL( activated ( const QModelIndex ) ), this, SLOT( slotActivated( const QModelIndex ) ) );


}

void CalendarTreeView::slotActivated( const QModelIndex index )
{
    kDebug()<<k_funcinfo<<index.column()<<endl;
}

void CalendarTreeView::headerContextMenuRequested( const QPoint &pos )
{
    kDebug()<<k_funcinfo<<header()->logicalIndexAt(pos)<<" at "<<pos<<endl;
}

void CalendarTreeView::contextMenuEvent ( QContextMenuEvent *event )
{
    //kDebug()<<k_funcinfo<<endl;
    emit contextMenuRequested( indexAt(event->pos()), event->globalPos() );
}

void CalendarTreeView::focusInEvent ( QFocusEvent *event )
{
    //kDebug()<<k_funcinfo<<endl;
    TreeViewBase::focusInEvent( event );
    emit focusChanged();
}

void CalendarTreeView::focusOutEvent ( QFocusEvent * event )
{
    //kDebug()<<k_funcinfo<<endl;
    TreeViewBase::focusInEvent( event );
    emit focusChanged();
}

void CalendarTreeView::selectionChanged( const QItemSelection &sel, const QItemSelection &desel )
{
    //kDebug()<<k_funcinfo<<sel.indexes().count()<<endl;
    //foreach( QModelIndex i, selectionModel()->selectedIndexes() ) { kDebug()<<k_funcinfo<<i.row()<<", "<<i.column()<<endl; }
    TreeViewBase::selectionChanged( sel, desel );
    emit selectionChanged( selectionModel()->selectedIndexes() );
}

void CalendarTreeView::currentChanged( const QModelIndex & current, const QModelIndex & previous )
{
    //kDebug()<<k_funcinfo<<endl;
    TreeViewBase::currentChanged( current, previous );
    selectionModel()->select( current, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows );
    emit currentChanged( current );
}

Calendar *CalendarTreeView::currentCalendar() const
{
    return itemModel()->calendar( currentIndex() );
}

Calendar *CalendarTreeView::selectedCalendar() const
{
    QModelIndexList lst = selectionModel()->selectedRows();
    if ( lst.count() == 1 ) {
        return itemModel()->calendar( lst.first() );
    }
    return 0;
}

QList<Calendar*> CalendarTreeView::selectedCalendars() const
{
    QList<Calendar *> lst;
    foreach ( QModelIndex i, selectionModel()->selectedRows() ) {
        Calendar *a = itemModel()->calendar( i );
        if ( a ) {
            lst << a;
        }
    }
    return lst;
}

void CalendarTreeView::dragMoveEvent(QDragMoveEvent *event)
{
    if (dragDropMode() == InternalMove && (event->source() != this || !(event->possibleActions() & Qt::MoveAction))) {
        return;
    }
    TreeViewBase::dragMoveEvent( event );
    if ( ! event->isAccepted() ) {
        return;
    }
    // QTreeView thinks it's ok to drop, but it might not be...
    event->ignore();
    QModelIndex index = indexAt( event->pos() );
    if ( ! index.isValid() ) {
        event->accept();
        return; // always ok to drop on main project
    }
    Calendar *c = itemModel()->calendar( index );
    if ( c == 0 ) {
        kError()<<k_funcinfo<<"no calendar to drop on!"<<endl;
        return; // hmmm
    }
    switch ( dropIndicatorPosition() ) {
        case AboveItem:
        case BelowItem:
            // c == sibling
            // if siblings parent is me or child of me: illegal
            if ( itemModel()->dropAllowed( c->parentCal(), event->mimeData() ) ) {
                event->accept();
            }
            break;
        case OnItem:
            // c == new parent
            if ( itemModel()->dropAllowed( c, event->mimeData() ) ) {
                event->accept();
            }
            break;
        default:
            break;
    }
}


//--------------------
CalendarDayView::CalendarDayView( Part *part, QWidget *parent )
    : TreeViewBase( parent )
{
    header()->setContextMenuPolicy( Qt::CustomContextMenu );
    m_model = new CalendarDayItemModel( part, this );
    setModel(m_model);
    // TODO: sort on date & weekday number
    for ( int c = 0; c < m_model->columnCount(); ++c ) {
        QItemDelegate *delegate = m_model->createDelegate( c, this );
        if ( delegate ) {
            setItemDelegateForColumn( c, delegate );
        }
    }
    setSelectionBehavior( QAbstractItemView::SelectRows );
    setSelectionMode( QAbstractItemView::SingleSelection );
    setItemsExpandable( true );
    
    setAcceptDrops( false );
    setDropIndicatorShown( false );
    
    connect( header(), SIGNAL( customContextMenuRequested ( const QPoint& ) ), this, SLOT( headerContextMenuRequested( const QPoint& ) ) );
    connect( this, SIGNAL( activated ( const QModelIndex ) ), this, SLOT( slotActivated( const QModelIndex ) ) );
}

void CalendarDayView::setCurrentCalendar( Calendar *calendar )
{
    itemModel()->setCalendar( calendar );
}

void CalendarDayView::slotActivated( const QModelIndex index )
{
    kDebug()<<k_funcinfo<<index.column()<<endl;
}

void CalendarDayView::headerContextMenuRequested( const QPoint &pos )
{
    kDebug()<<k_funcinfo<<header()->logicalIndexAt(pos)<<" at "<<pos<<endl;
}

void CalendarDayView::contextMenuEvent ( QContextMenuEvent *event )
{
    //kDebug()<<k_funcinfo<<endl;
    emit contextMenuRequested( indexAt(event->pos()), event->globalPos() );
}

void CalendarDayView::focusInEvent ( QFocusEvent *event )
{
    //kDebug()<<k_funcinfo<<endl;
    TreeViewBase::focusInEvent( event );
    emit focusChanged();
}

void CalendarDayView::focusOutEvent ( QFocusEvent * event )
{
    //kDebug()<<k_funcinfo<<endl;
    TreeViewBase::focusInEvent( event );
    emit focusChanged();
}

void CalendarDayView::selectionChanged( const QItemSelection &sel, const QItemSelection &desel )
{
    //kDebug()<<k_funcinfo<<sel.indexes().count()<<endl;
    //foreach( QModelIndex i, selectionModel()->selectedIndexes() ) { kDebug()<<k_funcinfo<<i.row()<<", "<<i.column()<<endl; }
    TreeViewBase::selectionChanged( sel, desel );
    emit selectionChanged( selectionModel()->selectedIndexes() );
}

void CalendarDayView::currentChanged( const QModelIndex & current, const QModelIndex & previous )
{
    //kDebug()<<k_funcinfo<<endl;
    TreeViewBase::currentChanged( current, previous );
    selectionModel()->select( current, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows );
    emit currentChanged( current );
}

CalendarDay *CalendarDayView::selectedDay() const
{
    QModelIndexList lst = selectionModel()->selectedRows();
    if ( lst.count() == 1 ) {
        return itemModel()->day( lst.first() );
    }
    return 0;
}

TimeInterval *CalendarDayView::selectedInterval() const
{
    QModelIndexList lst = selectionModel()->selectedRows();
    if ( lst.count() == 1 ) {
        return itemModel()->interval( lst.first() );
    }
    return 0;
}

//-----------------------------------
CalendarEditor::CalendarEditor( Part *part, QWidget *parent )
    : ViewBase( part, parent )
{
    setupGui();
    
    QVBoxLayout * l = new QVBoxLayout( this );
    l->setMargin( 0 );
    QSplitter *sp = new QSplitter( this );
    l->addWidget( sp );

    m_calendarview = new CalendarTreeView( part, sp );
    m_dayview = new CalendarDayView( part, sp );
    
    m_calendarview->setEditTriggers( m_calendarview->editTriggers() | QAbstractItemView::EditKeyPressed );
    
    m_dayview->setEditTriggers( m_dayview->editTriggers() | QAbstractItemView::EditKeyPressed );

    m_calendarview->setDragDropMode( QAbstractItemView::InternalMove );
    m_calendarview->setDropIndicatorShown ( true );
    m_calendarview->setDragEnabled ( true );
    m_calendarview->setAcceptDrops( true );
    

    connect( m_calendarview, SIGNAL( currentChanged( QModelIndex ) ), this, SLOT( slotCurrentCalendarChanged( QModelIndex ) ) );
    connect( m_calendarview, SIGNAL( selectionChanged( const QModelIndexList ) ), this, SLOT( slotCalendarSelectionChanged( const QModelIndexList ) ) );
    connect( m_calendarview, SIGNAL( contextMenuRequested( QModelIndex, const QPoint& ) ), this, SLOT( slotContextMenuCalendar( QModelIndex, const QPoint& ) ) );
    
    connect( m_dayview, SIGNAL( currentChanged( QModelIndex ) ), this, SLOT( slotCurrentDayChanged( QModelIndex ) ) );
    connect( m_dayview, SIGNAL( selectionChanged( const QModelIndexList ) ), this, SLOT( slotDaySelectionChanged( const QModelIndexList ) ) );
    connect( m_dayview, SIGNAL( contextMenuRequested( QModelIndex, const QPoint& ) ), this, SLOT( slotContextMenuDay( QModelIndex, const QPoint& ) ) );

    connect( m_dayview->itemModel(), SIGNAL( dataChanged( const QModelIndex&, const QModelIndex& ) ), this, SLOT( slotEnableActions() ) );
    
    connect( m_calendarview, SIGNAL( focusChanged() ), this, SLOT( slotEnableActions() ) );
    connect( m_dayview, SIGNAL( focusChanged() ), this, SLOT( slotEnableActions() ) );
    
}

void CalendarEditor::draw( Project &project )
{
    m_calendarview->setProject( &project );
    m_dayview->setProject( &project );
}

void CalendarEditor::draw()
{
}

void CalendarEditor::setGuiActive( bool activate )
{
    //kDebug()<<k_funcinfo<<activate<<endl;
    updateActionsEnabled( true );
    ViewBase::setGuiActive( activate );
    if ( activate ) {
        if ( !m_calendarview->currentIndex().isValid() ) {
            m_calendarview->selectionModel()->setCurrentIndex(m_calendarview->model()->index( 0, 0 ), QItemSelectionModel::NoUpdate);
        }
        //slotSelectionChanged( m_calendarview->selectionModel()->selectedRows() );
    }
}

void CalendarEditor::slotContextMenuCalendar( QModelIndex index, const QPoint& pos )
{
    //kDebug()<<k_funcinfo<<index.row()<<", "<<index.column()<<": "<<pos<<endl;
    QString name;
    if ( index.isValid() ) {
        Calendar *a = m_calendarview->itemModel()->calendar( index );
        if ( a ) {
            name = "calendareditor_calendar_popup";
        } else if ( m_dayview->itemModel()->day( index ) ) {
            name = "calendareditor_day_popup";
        }
    }
    //kDebug()<<k_funcinfo<<name<<endl;
    if ( name.isEmpty() ) {
        return;
    }
    emit requestPopupMenu( name, pos );
}

void CalendarEditor::slotContextMenuDay( QModelIndex index, const QPoint& pos )
{
    kDebug()<<k_funcinfo<<index.row()<<", "<<index.column()<<": "<<pos<<endl;
/*    QString name;
    if ( index.isValid() ) {
        if ( m_dayview->itemModel()->day( index ) ) {
            name = "calendareditor_day_popup";
        }
    }
    kDebug()<<k_funcinfo<<name<<endl;
    if ( name.isEmpty() ) {
        return;
    }
    emit requestPopupMenu( name, pos );*/
}

Calendar *CalendarEditor::currentCalendar() const
{
    return m_calendarview->currentCalendar();
}

void CalendarEditor::slotCurrentCalendarChanged(  const QModelIndex &curr )
{
    //kDebug()<<k_funcinfo<<curr.row()<<", "<<curr.column()<<endl;
    m_dayview->setCurrentCalendar( currentCalendar() );
}

void CalendarEditor::slotCalendarSelectionChanged( const QModelIndexList list)
{
    //kDebug()<<k_funcinfo<<list.count()<<endl;
    updateActionsEnabled( true );
}

void CalendarEditor::slotCurrentDayChanged(  const QModelIndex &curr )
{
    //kDebug()<<k_funcinfo<<curr.row()<<", "<<curr.column()<<endl;
}

void CalendarEditor::slotDaySelectionChanged( const QModelIndexList list)
{
    //kDebug()<<k_funcinfo<<list.count()<<endl;
    updateActionsEnabled( true );
}

void CalendarEditor::slotEnableActions()
{
    updateActionsEnabled( true );
}

void CalendarEditor::updateActionsEnabled(  bool on )
{
    QList<Calendar *> lst = m_calendarview->selectedCalendars();
    bool one = lst.count() == 1;
    bool more = lst.count() > 1;
    bool cal = m_calendarview->hasFocus();
    bool day = m_dayview->hasFocus();
    actionAddCalendar ->setEnabled( on && !more && cal );
    actionAddSubCalendar ->setEnabled( on && one && cal );
    actionDeleteSelection->setEnabled( on && ( one || more ) && cal );

    bool o = false;
    TimeInterval *ti = 0;
    CalendarDay *d = m_dayview->selectedDay();
    if ( on ) {
        o = d == 0 ? false : d->state() == CalendarDay::Working;
        if ( !o ) {
            ti = m_dayview->selectedInterval();
            if ( ti ) {
                d = m_dayview->itemModel()->parentDay( ti );
                o = d->state() == CalendarDay::Working;
            }
        }
    }
    actionAddDay->setEnabled( on && day && !ti && ( d == 0 || m_dayview->itemModel()->isDate( d ) ) );
    actionAddWorkInterval->setEnabled( on && o && day );
    
    bool act = on && day && ( ti || ( d && m_dayview->itemModel()->isDate( d ) ) );
    actionDeleteDaySelection->setEnabled( act );
}

void CalendarEditor::setupGui()
{
    KActionCollection *coll = actionCollection();
    QString name = "calendareditor_calendar_list";
    
    actionAddCalendar   = new KAction(KIcon( "document-new" ), i18n("Add Calendar"), this);
    coll->addAction("add_calendar", actionAddCalendar  );
    actionAddCalendar ->setShortcut( KShortcut( Qt::CTRL + Qt::Key_I ) );
    connect( actionAddCalendar , SIGNAL( triggered( bool ) ), SLOT( slotAddCalendar () ) );
    
    actionAddSubCalendar   = new KAction(KIcon( "document-new" ), i18n("Add Subcalendar"), this);
    coll->addAction("add_subcalendar", actionAddSubCalendar  );
    actionAddSubCalendar ->setShortcut( KShortcut( Qt::SHIFT + Qt::CTRL + Qt::Key_I ) );
    connect( actionAddSubCalendar , SIGNAL( triggered( bool ) ), SLOT( slotAddSubCalendar () ) );
    
    actionDeleteSelection  = new KAction(KIcon( "edit-delete" ), i18n("Delete Selected Calendar"), this);
    coll->addAction("delete_calendar_selection", actionDeleteSelection );
    actionDeleteSelection->setShortcut( KShortcut( Qt::Key_Delete ) );
    connect( actionDeleteSelection, SIGNAL( triggered( bool ) ), SLOT( slotDeleteCalendar() ) );
    
    addAction( name, actionAddSubCalendar  );
    addAction( name, actionAddCalendar  );
    addAction( name, actionDeleteSelection );
    
    name = "calendareditor_day_list";
    
    actionAddDay   = new KAction(KIcon( "document-new" ), i18n("Add Calendar Day"), this);
    coll->addAction("add_calendarday", actionAddDay  );
    connect( actionAddDay , SIGNAL( triggered( bool ) ), SLOT( slotAddDay() ) );
    
    actionAddWorkInterval   = new KAction(KIcon( "document-new" ), i18n("Add Work Interval"), this);
    coll->addAction("add_workinterval", actionAddWorkInterval  );
    connect( actionAddWorkInterval , SIGNAL( triggered( bool ) ), SLOT( slotAddInterval() ) );
    
    actionDeleteDaySelection  = new KAction(KIcon( "edit-delete" ), i18n("Delete Selected Item"), this);
    coll->addAction("delete_day_selection", actionDeleteDaySelection );
    connect( actionDeleteDaySelection, SIGNAL( triggered( bool ) ), SLOT( slotDeleteDaySelection() ) );
    
    addAction( name, actionAddWorkInterval  );
    addAction( name, actionAddDay  );
    addAction( name, actionDeleteDaySelection );
    
}

void CalendarEditor::slotAddCalendar ()
{
    //kDebug()<<k_funcinfo<<endl;
    // get parent through sibling
    Calendar *parent = m_calendarview->selectedCalendar ();
    if ( parent ) {
        parent = parent->parentCal();
    }
    insertCalendar ( new Calendar (), parent );
}

void CalendarEditor::slotAddSubCalendar ()
{
    //kDebug()<<k_funcinfo<<endl;
    insertCalendar ( new Calendar (), m_calendarview->selectedCalendar () );
}

void CalendarEditor::insertCalendar ( Calendar *calendar, Calendar *parent )
{
    QModelIndex i = m_calendarview->itemModel()->insertCalendar ( calendar, parent );
    if ( i.isValid() ) {
        QModelIndex p = m_calendarview->model()->parent( i );
        //if (parent) kDebug()<<k_funcinfo<<" parent="<<parent->name()<<": "<<p.row()<<", "<<p.column()<<endl;
        //kDebug()<<k_funcinfo<<i.row()<<", "<<i.column()<<endl;
        m_calendarview->setExpanded( p, true );
        m_calendarview->setCurrentIndex( i );
        m_calendarview->edit( i );
    }
}

void CalendarEditor::slotDeleteCalendar()
{
    //kDebug()<<k_funcinfo<<endl;
    m_calendarview->itemModel()->removeCalendar( m_calendarview->selectedCalendar() );
}

void CalendarEditor::slotAddInterval ()
{
    //kDebug()<<k_funcinfo<<endl;
    CalendarDay *parent = m_dayview->selectedDay ();
    if ( parent == 0 ) {
        TimeInterval *ti = m_dayview->selectedInterval();
        if ( ti == 0 ) {
            return;
        }
        parent = m_dayview->itemModel()->parentDay( ti );
        if ( parent == 0 ) {
            return;
        }
    }
    QModelIndex i = m_dayview->itemModel()->insertInterval( new TimeInterval(), parent );
    if ( i.isValid() ) {
        QModelIndex p = m_dayview->itemModel()->index( parent );
        m_dayview->setExpanded( p, true );
        m_dayview->setCurrentIndex( i );
        m_dayview->edit( i );
    }
}

void CalendarEditor::slotDeleteDaySelection()
{
    //kDebug()<<k_funcinfo<<endl;
    TimeInterval *ti = m_dayview->selectedInterval();
    if ( ti != 0 ) {
        m_dayview->itemModel()->removeInterval( ti );
        return;
    }
    CalendarDay *day = m_dayview->selectedDay();
    if ( day != 0 ) {
        m_dayview->itemModel()->removeDay( day );
    }
}

void CalendarEditor::slotAddDay ()
{
    //kDebug()<<k_funcinfo<<endl;
    Calendar *c = currentCalendar();
    if ( c == 0 ) {
        return;
    }
    QDate date = QDate::currentDate();
    while ( c->day( date ) ) {
        date = date.addDays( 1 );
    }
    QModelIndex i = m_dayview->itemModel()->insertDay( new CalendarDay(date,  CalendarDay::NonWorking ) );
    if ( i.isValid() ) {
        QModelIndex p = m_dayview->itemModel()->parent( i );
        m_dayview->setExpanded( p, true );
        m_dayview->setCurrentIndex( i );
        m_dayview->edit( i );
    }
}


} // namespace KPlato

#include "kptcalendareditor.moc"
