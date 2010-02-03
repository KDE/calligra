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

#include "kptglobal.h"

#include <kdebug.h>


namespace KPlato
{

ReportData::ReportData()
    : m_row( 0 )
{
}

ReportData::~ReportData()
{
}

bool ReportData::open()
{
    //qDebug()<<"ReportData::open:"<<m_model;
    return true;
}

bool ReportData::close()
{
    //qDebug()<<"ReportData::close:";
    return true;
}

QString ReportData::source() const {
    qDebug()<<"ReportData::source:";
    return "";
}

uint ReportData::fieldNumber ( const QString &fld )
{
    //qDebug()<<"ReportData::fieldNumber:"<<fld;
    QStringList names = fieldNames();
    return names.indexOf( fld );
}

QString ReportData::fieldTag( int index ) const
{
    return fieldTags().value( index );
}

QStringList ReportData::fieldTags() const
{
    //qDebug()<<"ReportData::fieldNames:";
    QStringList names;
    int count = m_model.columnCount();
    for ( int i = 0; i < count; ++i ) {
        names << m_model.headerData( i, Qt::Horizontal, Role::ColumnTag ).toString();
    }
    return names;
}

QStringList ReportData::fieldNames()
{
    //qDebug()<<"ReportData::fieldNames:";
    QStringList names;
    int count = m_model.columnCount();
    for ( int i = 0; i < count; ++i ) {
        names << m_model.headerData( i, Qt::Horizontal ).toString();
    }
    return names;
}

QVariant ReportData::value ( unsigned int i ) {
    if ( m_model.rowCount() == 0 ) {
        return QVariant();
    }
    QVariant value = m_model.index( at(), i ).data();
    //qDebug()<<"ReportData::value"<<i<<value;
    return value;
}

QVariant ReportData::value ( const QString &fld )
{
    qDebug()<<"ReportData::value:"<<fld;
    if ( m_model.rowCount() == 0 ) {
        return QVariant();
    }
    int i = fieldNumber ( fld );
    return value( i );
}

bool ReportData::moveNext()
{
    //qDebug()<<"ReportData::moveNext:";
    if ( m_model.rowCount() <= m_row + 1 ) {
        qDebug()<<"ReportData::moveNext: failed"<<m_row;
        return false;
    }
    ++m_row;
    return true;
}

bool ReportData::movePrevious()
{
    //qDebug()<<"ReportData::movePrevious:";
    if ( m_row <= 0  ) {
        qDebug()<<"ReportData::movePrevious: failed";
        return false;
    }
    --m_row;
    return true;
}

bool ReportData::moveFirst()
{
    if ( m_model.rowCount() == 0  ) {
        qDebug()<<"ReportData::moveFirst: failed";
        return false;
    }
    m_row = 0;
    return true;
}

bool ReportData::moveLast()
{
    //qDebug()<<"ReportData::moveLast:";
    if ( m_model.rowCount() == 0  ) {
        return false;
    }
    m_row =  m_model.rowCount() - 1;
    return true;
}

long ReportData::at() const
{
    //qDebug()<<"ReportData::at:"<<m_row;
    return m_row;
}

long ReportData::recordCount() const {
    //qDebug()<<"ReportData::recordCount:";
    return m_model.rowCount();
}

void ReportData::setModel( QAbstractItemModel *model )
{
    m_model.setSourceModel( model );
}

} //namespace KPlato
