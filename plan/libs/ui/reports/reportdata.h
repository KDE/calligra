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
#include "kptnodechartmodel.h"
#include "kptproject.h"

#include <QSortFilterProxyModel>
#include <QPointer>
#include <QString>
#include <QStringList>

namespace KPlato
{

class ScheduleManager;
class ReportData;
class ChartItemModel;

namespace Report
{
    KPLATOUI_EXPORT QList<ReportData*> createBaseReportDataModels( QObject *parent = 0 );
    KPLATOUI_EXPORT ReportData *findReportData( const QList<ReportData*> &lst, const QString &type );
}

class KPLATOUI_EXPORT ReportData : public QObject, public KoReportData
{
    Q_OBJECT
public:
    explicit ReportData( QObject *parent = 0 );
    ReportData( const ReportData &other );
    virtual ~ReportData();

    /// Re-implement this to create a clone of your report data object
    /// Returns 0 by default
    virtual ReportData *clone() const { return 0; }

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
    virtual int fieldNumber(const QString &field) const;

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

    virtual bool loadXml( const KoXmlElement &/*element*/ ) { return true; }
    virtual void saveXml( QDomElement &/*element*/ ) const {}

    bool isMainDataSource() const { return m_maindatasource; }
    bool isSubDataSource() const { return m_subdatasource; }
    void setSubDataSources( QList<ReportData*> &lst ) { m_subdatasources = lst; }

public slots:
    virtual void setProject( Project *project );
    void setScheduleManager( ScheduleManager *sm );

signals:
    void scheduleManagerChanged( ScheduleManager *sm );
    void createReportData( const QString &type, ReportData *rd );

protected:
    /// Re-implement this to create data models
    virtual void createModels() {}

protected:
    QSortFilterProxyModel m_model;
    long m_row;
    Project *m_project;
    ScheduleManager *m_schedulemanager;

    QMap<QString, QVariant> m_expressions;

    QMap<int, int> m_columnroles;
    QString m_name;
    QList<SortedField> m_sortlist;
    QList<QAbstractItemModel*> m_sortmodels;
    bool m_maindatasource;
    bool m_subdatasource;
    QList<ReportData*> m_subdatasources;
};

class KPLATOUI_EXPORT TaskReportData : public ReportData
{
    Q_OBJECT
public:
    explicit TaskReportData( QObject *parent = 0 );
    TaskReportData( const TaskReportData &other );

    bool loadXml( const KoXmlElement &element );
    void saveXml( QDomElement &element ) const;

    ReportData *clone() const;

protected:
    void createModels();
};

class KPLATOUI_EXPORT TaskStatusReportData : public ReportData
{
    Q_OBJECT
public:
    explicit TaskStatusReportData( QObject *parent = 0 );
    TaskStatusReportData( const TaskStatusReportData &other );

    bool loadXml( const KoXmlElement &element );
    void saveXml( QDomElement &element ) const;

    ReportData *clone() const;

protected:
    void createModels();
};

class KPLATOUI_EXPORT ResourceReportData : public ReportData
{
    Q_OBJECT
public:
    explicit ResourceReportData( QObject *parent = 0 );
    ResourceReportData( const ResourceReportData &other );

    bool loadXml( const KoXmlElement &element );
    void saveXml( QDomElement &element ) const;

    ReportData *clone() const;

protected:
    void createModels();
};

class KPLATOUI_EXPORT ResourceAssignmentReportData : public ReportData
{
    Q_OBJECT
public:
    explicit ResourceAssignmentReportData( QObject *parent = 0 );
    ResourceAssignmentReportData( const ResourceAssignmentReportData &other );

    bool loadXml( const KoXmlElement &element );
    void saveXml( QDomElement &element ) const;

    ReportData *clone() const;

protected:
    void createModels();
};

class KPLATOUI_EXPORT ChartReportData : public ReportData
{
    Q_OBJECT
public:
    explicit ChartReportData( QObject *parent = 0 );
    ChartReportData( const ChartReportData &other );

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

    bool loadXml( const KoXmlElement &element );
    void saveXml( QDomElement &element ) const;

protected:
    int firstRow();
    int lastRow() const;

    int m_firstrow;
    int m_lastrow;
    QDate m_startdate;
    QStringList m_keywords;
    bool m_fakedata;
};

class KPLATOUI_EXPORT CostPerformanceReportData : public ChartReportData
{
    Q_OBJECT
public:
    explicit CostPerformanceReportData( QObject *parent = 0 );
    CostPerformanceReportData( const CostPerformanceReportData &other );

    ReportData *clone() const;

    virtual bool open();

protected:
    void createModels();

private:
    ChartItemModel *m_chartmodel;
};

class KPLATOUI_EXPORT EffortPerformanceReportData : public ChartReportData
{
    Q_OBJECT
public:
    explicit EffortPerformanceReportData( QObject *parent = 0 );
    EffortPerformanceReportData( const EffortPerformanceReportData &other );

    ReportData *clone() const;

    virtual bool open();

protected:
    void createModels();

private:
    ChartItemModel *m_chartmodel;
};

class KPLATOUI_EXPORT CostBreakdownReportData : public ChartReportData
{
    Q_OBJECT
public:
    explicit CostBreakdownReportData( QObject *parent = 0 );
    CostBreakdownReportData( const CostBreakdownReportData &other );

    ReportData *clone() const;

    bool open();

protected:
    void createModels();
};


} //namespace KPlato

#endif
