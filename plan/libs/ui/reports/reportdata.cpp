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
#include "kptnodeitemmodel.h"
#include "kptflatproxymodel.h"
#include "kpttaskstatusmodel.h"
#include "kptresourcemodel.h"
#include "kptnodechartmodel.h"
#include "kptaccountsmodel.h"
#include "kptresourcemodel.h"
#include "kptresourceallocationmodel.h"
#include "kptresourceappointmentsmodel.h"

#include <kdebug.h>

#include <QSortFilterProxyModel>
#include <QString>
#include <QStringList>

extern int planDbg();

namespace KPlato
{

KPLATOUI_EXPORT QList<ReportData*> Report::createBaseReportDataModels( QObject *parent )
{
    QList<ReportData*> lst;

    ReportData *data = new TaskReportData( parent );
    lst << data;

    data = new TaskStatusReportData( parent );
    lst << data;

    data = new ResourceAssignmentReportData( parent );
    lst << data;

    data = new ResourceReportData( parent );
    lst << data;

    data = new CostPerformanceReportData( parent );
    lst << data;

    data = new EffortPerformanceReportData( parent );
    lst << data;

    data = new CostBreakdownReportData( parent );
    lst << data;

    foreach ( ReportData *r, lst ) {
        QList<ReportData*> sub;
        foreach ( ReportData *d, lst ) {
            if ( d->isSubDataSource() ) {
                sub << d;
            }
        }
        r->setSubDataSources( sub );
    }
    return lst;
}

KPLATOUI_EXPORT ReportData *Report::findReportData( const QList<ReportData*> &lst, const QString &type )
{
    foreach( ReportData *r, lst ) {
        if ( r->objectName() == type ) {
            return r;
        }
    }
    return 0;
}

//------------------
ReportData::ReportData( QObject *parent )
    : QObject( parent ),
    m_row( 0 ),
    m_project( 0 ),
    m_schedulemanager( 0 ),
    m_maindatasource( false ),
    m_subdatasource( false )
{
}

ReportData::ReportData( const ReportData &other )
    : QObject(),
    m_project( 0 ),
    m_schedulemanager( 0 )
{
    setObjectName( other.objectName() );
    m_name = other.m_name;
    m_columnroles = other.m_columnroles;
    m_sortlist = other.m_sortlist;
    m_maindatasource = other.m_maindatasource;
    m_subdatasource = other.m_subdatasource;
    m_subdatasources = other.m_subdatasources;
}

ReportData::~ReportData()
{
}

void ReportData::setColumnRole( int column, int role )
{
    m_columnroles[ column ] = role;
}

bool ReportData::open()
{
    close();
    ItemModelBase *basemodel = itemModel();
    if ( basemodel ) {
        basemodel->setProject( m_project );
        basemodel->setScheduleManager( m_schedulemanager );
    } else kError()<<"No item model";

    if ( ! m_sortlist.isEmpty() ) {
        QAbstractItemModel *sourcemodel = m_model.sourceModel();
        foreach ( const SortedField &sort, m_sortlist ) {
            int col = fieldNumber( sort.field );
            QSortFilterProxyModel *sf = new QSortFilterProxyModel( &m_model );
            sf->setSourceModel( sourcemodel );
            if ( basemodel ) {
                sf->setSortRole( basemodel->sortRole( col ) );
            }
            sf->sort( col, sort.order );
            sourcemodel = sf;
            m_sortmodels << sf;
        }
        m_model.setSourceModel( sourcemodel );
    }
    return true;
}

bool ReportData::close()
{
    while ( ! m_sortmodels.isEmpty() ) {
        QAbstractProxyModel *m = qobject_cast<QAbstractProxyModel*>( m_sortmodels.takeLast() );
        for ( QAbstractProxyModel *p = &m_model; p != 0; p = qobject_cast<QAbstractProxyModel*>( p->sourceModel() ) ) {
            if ( p->sourceModel() == m ) {
                p->setSourceModel( m->sourceModel() );
                delete m;
                break;
            }
        }
    }
    return true;
}

QString ReportData::sourceName() const {
    return m_name;
}

int ReportData::fieldNumber ( const QString &fld ) const
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
    int role = m_columnroles.contains( i ) ? m_columnroles[ i ] : Qt::DisplayRole;
    QVariant value = m_model.index( at(), i ).data( role );
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
     QStringList lst;
     foreach ( ReportData *r, m_subdatasources ) {
         if ( r->isSubDataSource() ) {
             lst << r->objectName();
         }
     }
     return lst;
}

QStringList ReportData::dataSourceNames() const
{
    QStringList lst;
    foreach ( ReportData *r, m_subdatasources ) {
        if ( r->isSubDataSource() ) {
            lst << r->sourceName();
        }
    }
    return lst;
}

void ReportData::setSorting(const QList<SortedField>& lst )
{
    m_sortlist = lst;
}

KoReportData* ReportData::data(const QString &source)
{
    ReportData *r = Report::findReportData( m_subdatasources, source );
    if ( r ) {
        r = r->clone();
        r->setParent( this );
        r->setProject( m_project );
        r->setScheduleManager( m_schedulemanager );
    }
    kDebug(planDbg())<<this<<m_subdatasources<<r;
    return r;
}

void ReportData::setModel( QAbstractItemModel *model )
{
    m_model.setSourceModel( model );
    ItemModelBase *m = itemModel();
    if ( m ) {
        m->setProject( m_project );
        m->setScheduleManager( m_schedulemanager );
    }
    kDebug(planDbg())<<this<<model<<m<<m_project<<m_schedulemanager;
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
}

void ReportData::setScheduleManager( ScheduleManager *sm )
{
    m_schedulemanager = sm;
}

//---------------------------
TaskReportData::TaskReportData( QObject *parent )
    : ReportData( parent )
{
    m_maindatasource = true;
    m_subdatasource = false;
    setObjectName( "tasks" );
    m_name = i18n( "Tasks" );
    setColumnRole( NodeModel::NodeDescription, Qt::EditRole );
    createModels();
}

TaskReportData::TaskReportData( const TaskReportData &other )
    : ReportData( other )
{
    createModels();
}

bool TaskReportData::loadXml( const KoXmlElement &element )
{
    return true;
}

void TaskReportData::saveXml( QDomElement &element ) const
{
}

ReportData *TaskReportData::clone() const
{
    return new TaskReportData( *this );
}

void TaskReportData::createModels()
{
    QRegExp rex( QString( "^(%1|%2)$" ).arg( (int)Node::Type_Task ).arg( (int)Node::Type_Milestone ) );
    QSortFilterProxyModel *sf = new QSortFilterProxyModel( &m_model );
    m_model.setSourceModel( sf );
    sf->setFilterKeyColumn( NodeModel::NodeType );
    sf->setFilterRole( Qt::EditRole );
    sf->setFilterRegExp( rex );
    sf->setDynamicSortFilter( true );
    FlatProxyModel *fm = new FlatProxyModel( sf );
    sf->setSourceModel( fm );
    GeneralNodeItemModel *m = new GeneralNodeItemModel( fm );
    fm->setSourceModel( m );
}

//---------------------------
TaskStatusReportData::TaskStatusReportData( QObject *parent )
    : ReportData( parent )
{
    m_maindatasource = true;
    m_subdatasource = false;
    setObjectName( "taskstatus" );
    m_name = i18n( "Task status" );

    setColumnRole( NodeModel::NodeDescription, Qt::EditRole );
    createModels();
}

TaskStatusReportData::TaskStatusReportData( const TaskStatusReportData &other )
    : ReportData( other )
{
    createModels();
}

bool TaskStatusReportData::loadXml( const KoXmlElement &element )
{
    return true;
}

void TaskStatusReportData::saveXml( QDomElement &element ) const
{
}

ReportData *TaskStatusReportData::clone() const
{
    return new TaskStatusReportData( *this );
}

void TaskStatusReportData::createModels()
{
    QRegExp rex( QString( "^(%1|%2)$" ).arg( (int)Node::Type_Task ).arg( (int)Node::Type_Milestone ) );
    QSortFilterProxyModel *sf = new QSortFilterProxyModel( &m_model );
    m_model.setSourceModel( sf );
    sf->setFilterKeyColumn( NodeModel::NodeType );
    sf->setFilterRole( Qt::EditRole );
    sf->setFilterRegExp( rex );
    sf->setDynamicSortFilter( true );
    FlatProxyModel *fm = new FlatProxyModel( sf );
    sf->setSourceModel( fm );
    TaskStatusItemModel *m = new TaskStatusItemModel( fm );
    fm->setSourceModel( m );
}

//---------------------------
ResourceReportData::ResourceReportData( QObject *parent )
    : ReportData( parent )
{
    m_maindatasource = true;
    m_subdatasource = false;
    setObjectName( "resources" );
    m_name = i18n( "Resource assignments" );

    createModels();
}

ResourceReportData::ResourceReportData( const ResourceReportData &other )
    : ReportData( other )
{
    createModels();
}

bool ResourceReportData::loadXml( const KoXmlElement &element )
{
    return true;
}

void ResourceReportData::saveXml( QDomElement &element ) const
{
}

ReportData *ResourceReportData::clone() const
{
    return new ResourceReportData( *this );
}

void ResourceReportData::createModels()
{
    ItemModelBase *m = 0;

    QRegExp rex( QString( "^(%1)$" ).arg( (int)OT_Resource ) );
    QSortFilterProxyModel *sf = new QSortFilterProxyModel( &m_model );
    m_model.setSourceModel( sf );
    sf->setFilterKeyColumn( 0 );
    sf->setFilterRole( Role::ObjectType );
    sf->setFilterRegExp( rex );
    sf->setDynamicSortFilter( true );
    FlatProxyModel *fm = new FlatProxyModel( sf );
    sf->setSourceModel( fm );
    m = new ResourceItemModel( fm );
    fm->setSourceModel( m );
}

//---------------------------
ResourceAssignmentReportData::ResourceAssignmentReportData( QObject *parent )
    : ReportData( parent )
{
    m_maindatasource = true;
    m_subdatasource = false;
    setObjectName( "resourceassignments" );
    m_name = i18n( "Resources" );

    createModels();
}

ResourceAssignmentReportData::ResourceAssignmentReportData( const ResourceAssignmentReportData &other )
    : ReportData( other )
{
    createModels();
}

bool ResourceAssignmentReportData::loadXml( const KoXmlElement &element )
{
    return true;
}

void ResourceAssignmentReportData::saveXml( QDomElement &element ) const
{
}

ReportData *ResourceAssignmentReportData::clone() const
{
    return new ResourceAssignmentReportData( *this );
}

void ResourceAssignmentReportData::createModels()
{
    QSortFilterProxyModel *sf = 0;
    ItemModelBase *m = 0;

    QRegExp rex( QString( "^(%1)$" ).arg( (int)OT_Appointment ) );
    sf = new QSortFilterProxyModel( &m_model );
    sf->setFilterKeyColumn( 0 );
    sf->setFilterRole( Role::ObjectType );
    sf->setFilterRegExp( rex );
    sf->setDynamicSortFilter( true );
    FlatProxyModel *fm = new FlatProxyModel( sf );
    sf->setSourceModel( fm );
    m = new ResourceAppointmentsRowModel( fm );
    fm->setSourceModel( m );
    m_model.setSourceModel( sf );
}

//---------------------------
ChartReportData::ChartReportData( QObject *parent )
    : ReportData( parent ),
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

ChartReportData::ChartReportData( const ChartReportData &other )
    : ReportData( other ),
    m_fakedata( true )
{
}

bool ChartReportData::open()
{
    return ReportData::open();
}

int ChartReportData::firstRow()
{
    if ( m_fakedata ) {
        return 0;
    }

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
        kDebug(planDbg())<<s<<row;
    }
    return row;
}

int ChartReportData::lastRow() const
{
    if ( m_fakedata ) {
        return 3;
    }
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
        kDebug(planDbg())<<last<<e<<row;
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
    if ( m_fakedata ) {
        kDebug(planDbg())<<m_row<<i;
        return QVariant( ( int )( m_row * i ) );
    }
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
            kDebug(planDbg())<<this<<row<<m_model.headerData( row, Qt::Vertical, Qt::EditRole )<<i<<"="<<value;
        }
    }
    return value;
}

QVariant ChartReportData::value( const QString &name ) const
{
    kDebug(planDbg())<<name;
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
//             kDebug(planDbg())<<this<<i<<"("<<count<<"):"<<m_model.headerData( i, Qt::Horizontal ).toString();
            names << m_model.headerData( i, Qt::Horizontal ).toString();
        }
    }
//     kDebug(planDbg())<<this<<names;
    return names;
}

void ChartReportData::addExpression( const QString &field, const QVariant &/*value*/, int /*relation*/ )
{
//     kDebug(planDbg())<<field<<value<<relation;
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

bool ChartReportData::loadXml( const KoXmlElement &element )
{
    return true;
}

void ChartReportData::saveXml( QDomElement &element ) const
{
}

//-----------------
CostPerformanceReportData::CostPerformanceReportData( QObject *parent )
    : ChartReportData( parent ),
    m_chartmodel( 0 )
{
    m_maindatasource = false;
    m_subdatasource = true;
    setObjectName( "costperformance" );
    m_name = i18n( "Cost Performance" );
    cbs = false;
    createModels();
}

CostPerformanceReportData::CostPerformanceReportData( const CostPerformanceReportData &other )
    : ChartReportData( other ),
    m_chartmodel( 0 )
{
    m_fakedata = false;
    cbs = other.cbs;
    createModels();
}

bool CostPerformanceReportData::open()
{
    if ( ! ChartReportData::open() ) {
        return false;
    }
    if ( m_chartmodel ) {
        m_chartmodel->setNodes( m_project ? QList<Node*>() << m_project : QList<Node*>() );
    }
    m_startdate = m_model.headerData( 0, Qt::Vertical, Qt::EditRole ).toDate();

    m_firstrow = firstRow();
    m_lastrow = lastRow();

    return true;
}

ReportData *CostPerformanceReportData::clone() const
{
    return new CostPerformanceReportData( *this );
}

void CostPerformanceReportData::createModels()
{
    ChartProxyModel *cpm = new ChartProxyModel( &m_model );
    m_model.setSourceModel( cpm );
    // hide effort
    cpm->setRejectColumns( QList<int>() << 3 << 4 << 5 );
    cpm->setZeroColumns( QList<int>() << 3 << 4 << 5 );
    m_chartmodel = new ChartItemModel( cpm );
    cpm->setSourceModel( m_chartmodel );
}

//-----------------
EffortPerformanceReportData::EffortPerformanceReportData( QObject *parent )
    : ChartReportData( parent ),
    m_chartmodel( 0 )
{
    m_maindatasource = false;
    m_subdatasource = true;
    setObjectName( "effortperformance" );
    m_name = i18n( "Effort Performance" );
    cbs = false;
    createModels();
}

EffortPerformanceReportData::EffortPerformanceReportData( const EffortPerformanceReportData &other )
    : ChartReportData( other ),
    m_chartmodel( 0 )
{
    m_fakedata = false;
    cbs = other.cbs;
    createModels();
}

bool EffortPerformanceReportData::open()
{
    if ( ! ChartReportData::open() ) {
        return false;
    }
    if ( m_chartmodel ) {
        m_chartmodel->setNodes( m_project ? QList<Node*>() << m_project : QList<Node*>() );
    }
    m_startdate = m_model.headerData( 0, Qt::Vertical, Qt::EditRole ).toDate();

    m_firstrow = firstRow();
    m_lastrow = lastRow();

    return true;
}

ReportData *EffortPerformanceReportData::clone() const
{
    return new EffortPerformanceReportData( *this );
}

void EffortPerformanceReportData::createModels()
{
    ChartProxyModel *cpm = new ChartProxyModel( &m_model );
    // hide cost
    cpm->setRejectColumns( QList<int>() << 0 << 1 << 2 );
    cpm->setZeroColumns( QList<int>() << 0 << 1 << 2 );
    m_chartmodel = new ChartItemModel( cpm );
    cpm->setSourceModel( m_chartmodel );
    m_model.setSourceModel( cpm );
}

//-----------------
CostBreakdownReportData::CostBreakdownReportData( QObject *parent )
    : ChartReportData( parent )
{
    m_maindatasource = false;
    m_subdatasource = true;
    setObjectName( "costbreakdown" );
    m_name = i18n( "Cost Breakdown" );

    cbs = true;
    createModels();
}

CostBreakdownReportData::CostBreakdownReportData( const CostBreakdownReportData &other )
    : ChartReportData( other )
{
    m_fakedata = false;
    cbs = other.cbs;
    createModels();
}

bool CostBreakdownReportData::open()
{
    if ( ! ChartReportData::open() ) {
        return false;
    }
    m_startdate = m_model.headerData( 3, Qt::Horizontal, Qt::EditRole ).toDate();

    m_firstrow = firstRow();
    m_lastrow = lastRow();

    return true;
}

ReportData *CostBreakdownReportData::clone() const
{
    return new CostBreakdownReportData( *this );
}

void CostBreakdownReportData::createModels()
{
    FlatProxyModel *fm = new FlatProxyModel( &m_model );
    ItemModelBase *m = new CostBreakdownItemModel( fm );
    fm->setSourceModel( m );
    m_model.setSourceModel( fm );
}

} //namespace KPlato

#include "reportdata.moc"
