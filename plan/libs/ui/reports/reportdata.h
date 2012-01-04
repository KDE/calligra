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

#ifndef KPLATO_REPORTDATA_H
#define KPLATO_REPORTDATA_H

#include "kplatoui_export.h"

#include <KoReportData.h>

#include "kptitemmodelbase.h"
#include "kptproject.h"

#include <QSortFilterProxyModel>
#include <QPointer>
#include <QString>
#include <QStringList>

namespace KPlato
{

class ScheduleManager;

class KPLATOUI_EXPORT ReportData : public QObject, public KoReportData
{
    Q_OBJECT
public:
    explicit ReportData();
    virtual ~ReportData();

    /// Set the @p role that shall be used when fetching data for @p column
    /// Default is Qt::DisplayRole
    void setColumnRole( int column, int role );

    //!Open the dataset
    virtual bool open();

    //!Close the dataset
    virtual bool close();

    //!Move to the next record
    virtual bool moveNext();

    //!Move to the previous record
    virtual bool movePrevious();

    //!Move to the first record
    virtual bool moveFirst();

    //!Move to the last record
    virtual bool moveLast();

    //!Return the current position in the dataset
    virtual qint64 at() const;

    //!Return the total number of records
    virtual qint64 recordCount() const;

    //!Return the index number of the field given by nane field
    virtual unsigned int fieldNumber(const QString &field) const;

    //!Return the list of field names
    virtual QStringList fieldNames() const;
    //!Return the list of field keys
    virtual QStringList fieldKeys() const;

    //!Return the value of the field at the given position for the current record
    virtual QVariant value(unsigned int) const;

    //!Return the value of the field fir the given name for the current record
    virtual QVariant value(const QString &field) const;

    //!Return the name of this source
    virtual QString sourceName() const;

    //!Sets the sorting for the data
    //!Should be called before open() so that the data source can be edited accordingly
    //!Default impl does nothing
    virtual void setSorting( const QList<SortedField>& sorting );

    //!Return a list of data sources possible for advanced controls
    virtual QStringList dataSources() const;
    //!Return a list of data source names possible for advanced controls
    virtual QStringList dataSourceNames() const;

    //!Allow a driver to create a new instance with a new data source
    //!source is a driver specific identifier
    //!Owner of the returned pointer is the caller
    virtual KoReportData* data(const QString &source);

    void setModel( QAbstractItemModel *model );
    QAbstractItemModel *model() const;
    ItemModelBase *itemModel() const;
    
    Project *project() const { return m_project; }
    ScheduleManager *scheduleManager() const { return m_schedulemanager; }
    
public slots:
    void setProject( Project *project );
    void setScheduleManager( ScheduleManager *sm );
    void setDataSource( ReportData *source );

signals:
    void scheduleManagerChanged( ScheduleManager *sm );
    void createReportData( const QString &type, ReportData *rd );

protected:
    QSortFilterProxyModel m_model;
    long m_row;
    Project *m_project;
    ScheduleManager *m_schedulemanager;
    
    ReportData *m_datasource;
    QMap<QString, QVariant> m_expressions;

    QMap<int, int> m_columnroles;
};

class KPLATOUI_EXPORT ChartReportData : public ReportData
{
public:
    explicit ChartReportData();

    /// Prepare the data for access
    virtual bool open();

    //!Move to the next record
    virtual bool moveNext();

    //!Move to the previous record
    virtual bool movePrevious();

    //!Move to the first record
    virtual bool moveFirst();

    //!Move to the last record
    virtual bool moveLast();

    //!Return the total number of records
    virtual qint64 recordCount() const;

    //!Return the value of the field at the given position for the current record
    virtual QVariant value(unsigned int) const;
    //!Return the value of the field named @p name
    QVariant value( const QString &name ) const;
    
    //!Return the list of field names, used for legends in a chart
    virtual QStringList fieldNames() const;

    void addExpression( const QString &field, const QVariant &value, int relation = '=' );

    bool cbs;

protected:
    int firstRow();
    int lastRow() const;

    int m_firstrow;
    int m_lastrow;
    QDate m_startdate;
    QStringList m_keywords;
};

} //namespace KPlato

#endif
