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
    }
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
    return QStringList() << "costbreakdown";
}

QStringList ReportData::dataSourceNames() const
{
    //TODO
    return QStringList() << i18n( "Cost Breakdown" );
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
    KoReportData *rd = 0;
    emit createReportData( source, rd );
    return rd;
}

void ReportData::setModel( QAbstractItemModel *model )
{
    m_model.setSourceModel( model );
    ItemModelBase *m = itemModel();
    if ( m ) {
        m->setProject( m_project );
        m->setScheduleManager( m_schedulemanager );
    }
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
    : ReportData()
{
}

bool ChartReportData::moveNext()
{
    if ( recordCount() <= m_row + 1 ) {
        return false;
    }
    ++m_row;
    return true;
}

bool ChartReportData::movePrevious()
{
    if ( m_row == 0  ) {
        return false;
    }
    --m_row;
    return true;
}

bool ChartReportData::moveFirst()
{
    if ( recordCount() <= 0  ) {
        return false;
    }
    m_row = 0;
    return true;
}

bool ChartReportData::moveLast()
{
    if ( recordCount() == 0  ) {
        return false;
    }
    m_row = recordCount() - 1;
    return true;
}

qint64 ChartReportData::recordCount() const
{
    return m_model.columnCount() - 4;
}

QVariant ChartReportData::value ( unsigned int i ) const
{
    if ( recordCount() == 0 ) {
        return QVariant();
    }
    QVariant value;
    if ( i == 0 ) {
        // x-axis labels
        value = m_model.headerData( m_row + 3, Qt::Horizontal );
    } else {
        // data
        value = m_model.index( i - 1, m_row + 2 ).data( Role::Planned );
    }
    return value;
}

QStringList ChartReportData::fieldNames() const
{
    // Legends
    QStringList names;
    names << "";
    int count = m_model.rowCount();
    for ( int i = 0; i < count; ++i ) {
        names << m_model.index( i, 0 ).data().toString();
    }
    return names;
}


} //namespace KPlato

#include "reportdata.moc"
