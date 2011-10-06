/*
* KPlato Report Plugin
* Copyright (C) 2007-2009 by Adam Pigg (adam@piggz.co.uk)
* Copyright (C) 2010 by Dag Andersen <danders@get2net.dk>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#include "reportdata.h"

#include "kptproject.h"
#include "kptschedule.h"

#include <kdebug.h>

#include <QSortFilterProxyModel>
#include <QString>
#include <QStringList>

namespace KPlato
{

ReportData::ReportData()
    : m_row( 0 ),
    m_project( 0 ),
    m_schedulemanager( 0 )
{
}

ReportData::~ReportData()
{
}

bool ReportData::open()
{
    ItemModelBase *m = itemModel();
    if ( m ) {
        m->setScheduleManager( m_schedulemanager );
        kDebug()<<this<<m_schedulemanager;
    } else kError()<<"No item model";
    return true;
}

bool ReportData::close()
{
    return true;
}

QString ReportData::sourceName() const {
    return QString();
}

unsigned int ReportData::fieldNumber ( const QString &fld ) const
{
    QStringList names = fieldKeys();
    int idx = names.indexOf( fld );
    return idx;
}

QStringList ReportData::fieldNames() const
{
    QStringList names;
    int count = m_model.columnCount();
    for ( int i = 0; i < count; ++i ) {
        names << m_model.headerData( i, Qt::Horizontal ).toString();
    }
    return names;
}

QStringList ReportData::fieldKeys() const
{
    QStringList keys;
    int count = m_model.columnCount();
    for ( int i = 0; i < count; ++i ) {
        keys << m_model.headerData( i, Qt::Horizontal, Role::ColumnTag ).toString();
    }
    return keys;
}

QVariant ReportData::value ( unsigned int i ) const {
    if ( m_model.rowCount() == 0 ) {
        return QVariant();
    }
    QVariant value = m_model.index( at(), i ).data();
    return value;
}

QVariant ReportData::value ( const QString &fld ) const
{
    if ( m_model.rowCount() == 0 ) {
        return QVariant();
    }
    int i = fieldNumber ( fld );
    return value( i );
}

bool ReportData::moveNext()
{
    if ( m_model.rowCount() <= m_row + 1 ) {
        return false;
    }
    ++m_row;
    return true;
}

bool ReportData::movePrevious()
{
    if ( m_row <= 0  ) {
        return false;
    }
    --m_row;
    return true;
}

bool ReportData::moveFirst()
{
    if ( m_model.rowCount() == 0  ) {
        return false;
    }
    m_row = 0;
    return true;
}

bool ReportData::moveLast()
{
    if ( m_model.rowCount() == 0  ) {
        return false;
    }
    m_row =  m_model.rowCount() - 1;
    return true;
}

qint64 ReportData::at() const
{
    return m_row;
}

qint64 ReportData::recordCount() const {
    return m_model.rowCount();
}

QStringList ReportData::dataSources() const
{
    //TODO
    return QStringList() << "costbreakdown" << "costperformance" << "effortperformance";
}

QStringList ReportData::dataSourceNames() const
{
    //TODO
    return QStringList() << i18n( "Cost Breakdown" ) << i18n( "Cost Performance" ) << i18n( "Effort Performance" );
}

void ReportData::setSorting(const QList<SortedField>& lst )
{
    //FIXME the actual sorting should prob be in open(), but I don't think it matters for now
    if ( lst.isEmpty() ) {
        return;
    }
    QSortFilterProxyModel *sf = 0;
    QAbstractItemModel *source_model = m_model.sourceModel();

    foreach ( const SortedField &sort, lst ) {
        int col = fieldNumber( sort.field );
        sf = new QSortFilterProxyModel( &m_model );
        sf->setSourceModel( source_model );
        sf->sort( col, sort.order );
        source_model = sf;
    }
    m_model.setSourceModel( sf );
}

KoReportData* ReportData::data(const QString &source)
{
    emit createReportData( source, this );
    return m_datasource;
}

void ReportData::setDataSource( ReportData *source )
{
    m_datasource = source;
}

void ReportData::setModel( QAbstractItemModel *model )
{
    m_model.setSourceModel( model );
    ItemModelBase *m = itemModel();
    if ( m ) {
        m->setProject( m_project );
        m->setScheduleManager( m_schedulemanager );
    }
    kDebug()<<this<<model<<m<<m_project<<m_schedulemanager;
}

QAbstractItemModel *ReportData::model() const
{
    return const_cast<QSortFilterProxyModel*>( &m_model );
}

ItemModelBase *ReportData::itemModel() const
{
    QAbstractItemModel *m = m_model.sourceModel();
    QAbstractProxyModel *p = 0;
    do {
        p = qobject_cast<QAbstractProxyModel*>( m );
        if ( p ) {
            m = p->sourceModel();
        }
    } while ( p );
    return qobject_cast<ItemModelBase*>( m );
}

void ReportData::setProject( Project *project )
{
    m_project = project;
    ItemModelBase *m = itemModel();
    if ( m ) {
        m->setProject( m_project );
    }
}

void ReportData::setScheduleManager( ScheduleManager *sm )
{
    m_schedulemanager = sm;
    ItemModelBase *m = itemModel();
    if ( m ) {
        m->setScheduleManager( m_schedulemanager );
    }
    emit scheduleManagerChanged( sm );
}


//---------------------------
ChartReportData::ChartReportData()
    : ReportData(),
    cbs( false ),
    m_firstrow( 0 ),
    m_lastrow( -1 )
{
    // these controls the amount of data (days) to include in a chart
    m_keywords << "start"
                << "end"
                << "first"
                << "days";
}

bool ChartReportData::open()
{
    if ( ! ReportData::open() ) {
        return false;
    }
    if ( cbs ) {
        m_startdate = m_model.headerData( 3, Qt::Horizontal, Qt::EditRole ).toDate();
    } else {
        m_startdate = m_model.headerData( 0, Qt::Vertical, Qt::EditRole ).toDate();
    }
    kDebug()<<m_expressions;
    m_firstrow = firstRow();
    m_lastrow = lastRow();
    kDebug()<<m_firstrow<<"-"<<m_lastrow<<":"<<recordCount();
    return true;
}

int ChartReportData::firstRow()
{
    int row = 0;
    QDate s;
    if ( m_expressions.contains( "start" ) ) {
        s = m_expressions[ "start" ].toDate();
    } else if ( m_expressions.contains( "first" ) ) {
        s = QDate::currentDate().addDays( m_expressions[ "first" ].toInt() );
    }
    if ( s.isValid() ) {
        if ( m_startdate.isValid() && s > m_startdate ) {
            row = m_startdate.daysTo( s );
            m_startdate = s;
        }
        kDebug()<<s<<row;
    }
    return row;
}

int ChartReportData::lastRow() const
{
    int row = cbs
            ? m_model.columnCount() - 5 // cbs has data as columns + name, description, total (0-2) and parent (last)
            : m_model.rowCount() - 1;
    if ( row < 0 ) {
        return -1;
    }
    QDate e;
    if ( m_expressions.contains( "end" ) ) {
        e = m_expressions[ "end" ].toDate();
    } else if ( m_expressions.contains( "days" ) ) {
        e = m_startdate.addDays( m_expressions[ "days" ].toInt() - 1 );
    }
    if ( e.isValid() ) {
        QDate last;
        if ( cbs ) {
            last = m_model.headerData( row + 3, Qt::Horizontal, Qt::EditRole ).toDate();
        } else {
            last = m_model.headerData( row, Qt::Vertical, Qt::EditRole ).toDate();
        }
        if ( last.isValid() && e < last ) {
            row -= ( e.daysTo( last ) );
        }
        kDebug()<<last<<e<<row;
    }
    return row > m_firstrow ? row : m_firstrow;
}

bool ChartReportData::moveNext()
{
    if ( m_row >= recordCount() - 1 ) {
        return false;
    }
    ++m_row;
    return true;
}

bool ChartReportData::movePrevious()
{
    if ( m_row <= 0 ) {
        return false;
    }
    --m_row;
    return true;
}

bool ChartReportData::moveFirst()
{
    m_row = 0;
    return true;
}

bool ChartReportData::moveLast()
{
    m_row = recordCount() - 1;
    return true;
}

qint64 ChartReportData::recordCount() const
{
    return  m_lastrow < 0 ? 0 : m_lastrow - m_firstrow + 1;
}

QVariant ChartReportData::value ( unsigned int i ) const
{
    QVariant value;
    int row = m_row + m_firstrow;
    if ( cbs ) {
        if ( i == 0 ) {
            // x-axis labels
            value = m_model.headerData( row + 3, Qt::Horizontal );
        } else {
            // data
            value = m_model.index( i - 1, row + 2 ).data( Role::Planned );
        }
    } else {
        if ( i == 0 ) {
            // x-axis labels
            value = m_model.headerData( row, Qt::Vertical );
        } else {
            // data
            value = m_model.index( row, i - 1 ).data();
            kDebug()<<this<<row<<m_model.headerData( row, Qt::Vertical, Qt::EditRole )<<i<<"="<<value;
        }
    }
    return value;
}

QVariant ChartReportData::value( const QString &name ) const
{
    kDebug()<<name;
    if ( m_expressions.contains( name ) ) {
        return m_expressions[ name ];
    }
    return ReportData::value( name );
}

QStringList ChartReportData::fieldNames() const
{
    // Legends
    QStringList names;
    names << ""; // first row/column not used
    if ( cbs ) {
        int count = m_model.rowCount();
        for ( int i = 0; i < count; ++i ) {
            names << m_model.index( i, 0 ).data().toString();
        }
    } else {
        int count = m_model.columnCount();
        for ( int i = 0; i < count; ++i ) {
//             kDebug()<<this<<i<<"("<<count<<"):"<<m_model.headerData( i, Qt::Horizontal ).toString();
            names << m_model.headerData( i, Qt::Horizontal ).toString();
        }
    }
//     kDebug()<<this<<names;
    return names;
}

void ChartReportData::addExpression( const QString &field, const QVariant &value, int relation )
{
//     kDebug()<<field<<value<<relation;
    QStringList lst = field.split( '=', QString::SkipEmptyParts );
    if ( lst.count() == 2 ) {
        QString key = lst[ 0 ].trimmed().toLower();
        if ( m_keywords.contains( key ) ) {
            m_expressions.insert( key, lst[ 1 ].trimmed() );
        } else {
            kWarning()<<"unknown key:"<<key;
        }
    } else {
        kWarning()<<"Invalid key or data:"<<field;
    }
}

} //namespace KPlato

#include "reportdata.moc"
