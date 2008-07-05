/* This file is part of the KOffice project
 * Copyright (c) 2008 Dag Andersen <kplato@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "kptnodechartmodel.h"
#include "kptnode.h"
#include "kptproject.h"
#include "kptschedule.h"

#include <QPointF>
#include <QVariant>
#include <QColor>

#include <kdebug.h>

namespace KPlato
{
    
NodeChartModel::NodeChartModel( QObject *parent )
    : AbstractChartModel( parent )
{
}

void NodeChartModel::setProject( Project *project )
{
    if ( m_project ) {
        m_bcwp.clear();
        m_acwp.clear();
    }
    m_project = project;
    if ( m_project ) {
    }
    emit reset();
}

void NodeChartModel::setScheduleManager( ScheduleManager *sm )
{
    if ( m_manager ) {
        m_bcwp.clear();
        m_acwp.clear();
    }
    m_manager = sm;
    if ( m_manager ) {
        if ( m_project ) {
            m_bcwp = m_project->bcwpPrDay( sm->id() );
            m_acwp = m_project->acwp( sm->id() );
        }
    }
    emit reset();
}

int NodeChartModel::dataSetCount( const ChartAxisIndex &index ) const
{
    if ( m_project == 0 || m_manager == 0 ) {
        return 0;
    }
    if ( index.isValid() && index.number() == 0 ) {
        return 3;
    }
    return 0;
}

int NodeChartModel::childCount( const ChartDataIndex &index ) const
{
    return 0;
}

QVariant NodeChartModel::data( const ChartDataIndex &idx, int role ) const
{
    kDebug()<<idx.number()<<idx.userData<<role;
    if ( role == Qt::ForegroundRole ) {
        switch ( idx.userData ) {
            case BCWS: return QColor( Qt::green );
            case BCWP: return QColor( Qt::red );
            case ACWP: return QColor( Qt::yellow );
        }
    }
    return QVariant();
}

QVariant NodeChartModel::data( const ChartDataIndex &idx, const ChartAxisIndex &axis, int role ) const
{
    //NOTE: Just return a "day number" for x-axis as the test PlotWidget doesn't handle dates
    kDebug()<<idx.number()<<idx.userData<<role;
    if ( role == Qt::DisplayRole ) {
        int axisType = axis.userData;
        switch ( idx.userData ) {
            case BCWS: {
                QVariantList lst;
                if ( axisType == Axis_X ) {
                    QDate s = qMin( m_bcwp.startDate(), m_acwp.startDate() );
                    foreach ( QDate d, m_bcwp.days().keys() ) {
                        lst << s.daysTo( d );
                    }
                } else if ( axisType == Axis_Y ) {
                    double c = 0.0;
                    foreach ( const EffortCost &v, m_bcwp.days().values() ) {
                        c += v.cost();
                        lst << c;
                    }
                }
                return lst;
                break; }
            case BCWP: {
                QVariantList lst;
                if ( axisType == Axis_X ) {
                    QDate s = qMin( m_bcwp.startDate(), m_acwp.startDate() );
                    foreach ( QDate d, m_bcwp.days().keys() ) {
                        lst << s.daysTo( d );
                    }
                } else if ( axisType == Axis_Y ) {
                    foreach ( const EffortCost &v, m_bcwp.days().values() ) {
                        lst << v.bcwpCost();
                    }
                }
                return lst;
                break; }
            case ACWP: {
                QVariantList lst;
                if ( axisType == Axis_X ) {
                    QDate s = qMin( m_bcwp.startDate(), m_acwp.startDate() );
                    foreach ( QDate d, m_bcwp.days().keys() ) {
                        lst << s.daysTo( d );
                    }
                } else if ( axisType == Axis_Y ) {
                    double c = 0.0;
                    foreach ( const EffortCost &v, m_acwp.days().values() ) {
                        c +=  v.cost();
                        lst << c;
                    }
                }
                return lst;
                break; }
            default:
                kDebug()<<"Invalid userData"<<idx.userData;
                break;
        }
    }
    return QVariant();
}


ChartDataIndex NodeChartModel::index( int number, const ChartAxisIndex &idx ) const
{
    if ( ! idx.isValid() ) {
        return ChartDataIndex();
    }
    if ( number == 0 ) {
        kDebug()<<number<<"BCWS";
        return createDataIndex( number, idx, BCWS );
    }
    if ( number == 1 ) {
        kDebug()<<number<<"BCWP";
        return createDataIndex( number, idx, BCWP );
    }
    if ( number == 2 ) {
        kDebug()<<number<<"ACWP";
        return createDataIndex( number, idx, ACWP );
    }
    return ChartDataIndex();
}
    
ChartDataIndex NodeChartModel::index( int number, const ChartDataIndex &idx ) const
{
    if ( ! idx.isValid() ) {
        return ChartDataIndex();
    }
    return ChartDataIndex();
}

int NodeChartModel::axisCount( const ChartAxisIndex &index ) const
{
    if ( m_project == 0 || m_manager == 0 ) {
        return 0;
    }
    if ( ! index.isValid() ) {
        return 1; // we have one x,y axis pair
    }
    if ( index.number() == 0 && index.userData == 0 ) {
        return 2; // x- and y axis
    } else kDebug()<<"Invalid index";
    
    return 0;
}

QVariant NodeChartModel::axisData( const ChartAxisIndex &index, int role ) const
{
    if ( ! index.isValid() ) {
        // Return common data for the widget, like header, footer, legend...
        return QVariant();
    }
    if ( role == Qt::DisplayRole ) {
        if ( index.userData == Axis_X ) {
            return QVariant(); // x-axis
        }
        if ( index.userData == Axis_Y ) {
            return QVariant(); // y-axis
        }
    }
    if ( role == AbstractChartModel::AxisTypeRole ) {
        return index.userData;
    }
    if ( role == AbstractChartModel::AxisMinRole ) {
        if ( index.userData == Axis_X ) {
            return 0;
        }
        if ( index.userData == Axis_Y ) {
            return 0.0;
        }
    }
    if ( role == AbstractChartModel::AxisMaxRole ) {
        if ( index.userData == Axis_X ) {
            QDate d = qMin( m_bcwp.startDate(), m_acwp.startDate() );
            QDate e = qMax( m_bcwp.endDate(), m_acwp.endDate() );
            return d.daysTo( e );
        }
        if ( index.userData == Axis_Y ) {
            return qMax( m_bcwp.totalCost(), m_acwp.totalCost() ) * 1.1;
        }
    }
    if ( role == AbstractChartModel::AxisDataTypeRole ) {
        if ( index.userData == Axis_X ) {
            return QVariant::Date; // x-axis
        }
        if ( index.userData == Axis_Y ) {
            return QVariant::Double; // y-axis
        }
    }
    return QVariant();
}

ChartAxisIndex NodeChartModel::axisIndex( int number, const ChartAxisIndex &parent ) const
{
    if ( ! parent.isValid() ) {
        if ( number == 0 ) {
            return createAxisIndex( number, parent, AbstractChartModel::Axis_None );
        }
    } else {
        if ( number == 0 ) {
            return createAxisIndex( number, parent, AbstractChartModel::Axis_X );
        } else if ( number == 1 ) {
            return createAxisIndex( number, parent, AbstractChartModel::Axis_Y );
        }
    }
    return ChartAxisIndex();
}


} //namespace KPlato

#include "kptnodechartmodel.moc"
