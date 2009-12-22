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
    : AbstractChartModel( parent ),
    m_project( 0 ),
    m_manager( 0 )
{
    m_dataShown.showCost = true;
    m_dataShown.showBCWSCost = true;
    m_dataShown.showBCWPCost = true;
    m_dataShown.showACWPCost = true;
    m_dataShown.showEffort = true;
    m_dataShown.showBCWSEffort = true;
    m_dataShown.showBCWPEffort = true;
    m_dataShown.showACWPEffort = true;
}


void NodeChartModel::setProject( Project *project )
{
    m_bcwp.clear();
    m_acwp.clear();
    if ( m_project ) {
        disconnect( m_project, SIGNAL( projectCalculated( ScheduleManager* ) ), this, SLOT( setScheduleManager( ScheduleManager* ) ) );
        disconnect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotNodeChanged( Node* ) ) );
    }
    m_project = project;
    if ( m_project ) {
        connect( m_project, SIGNAL( projectCalculated( ScheduleManager* ) ), this, SLOT( setScheduleManager( ScheduleManager* ) ) );
        connect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotNodeChanged( Node* ) ) );
    }
    emit reset();
}

void NodeChartModel::slotNodeChanged( Node *node )
{
    if ( m_nodes.contains( node ) ) {
        calculate();
        emit reset();
        return;
    }
    foreach ( Node *n, m_nodes ) {
        if ( node->isChildOf( n ) ) {
            calculate();
            emit reset();
            return;
        }
    }
}

void NodeChartModel::setScheduleManager( ScheduleManager *sm )
{
    m_manager = sm;
    calculate();
    emit reset();
}

void NodeChartModel::setDataShown( const NodeChartModel::DataShown &show )
{
    m_dataShown = show;
    emit reset();
}

void NodeChartModel::calculate()
{
    //kDebug()<<m_project<<m_manager<<m_nodes;
    m_bcwp.clear();
    m_acwp.clear();
    if ( m_manager ) {
        if ( m_project ) {
            foreach ( Node *n, m_nodes ) {
                bool skip = false;
                foreach ( Node *p, m_nodes ) {
                    if ( n->isChildOf( p ) ) {
                        skip = true;
                        break;
                    }
                }
                if ( ! skip ) {
                    m_bcwp += n->bcwpPrDay( m_manager->id() );
                    m_acwp += n->acwp( m_manager->id() );
                }
            }
        }
    }
/* debug
    EffortCostDayMap::ConstIterator it;
    EffortCostDayMap::ConstIterator end = m_bcwp.days().constEnd();
    for ( it = m_bcwp.days().constBegin(); it != end; ++it ) {
        //kDebug()<<"Plan:"<<it.key()
                <<it.value().effort().toString()
                <<it.value().cost()
                <<it.value().bcwpEffort()
                <<it.value().bcwpCost();
    }
    end = m_acwp.days().constEnd();
    for ( it = m_acwp.days().constBegin(); it != end; ++it ) {
        //kDebug()<<"Actual:"<<it.key()
                <<it.value().effort().toString()
                <<it.value().cost();
    }
*/

}

void NodeChartModel::setNodes( const QList<Node*> &nodes )
{
    //kDebug()<<nodes;
    m_nodes = nodes;
    calculate();
    emit reset();
}

void NodeChartModel::addNode( Node *node )
{
    m_nodes.append( node );
    calculate();
    emit reset();
}

void NodeChartModel::clearNodes()
{
    m_nodes.clear();
    calculate();
    emit reset();
}

int NodeChartModel::dataSetCount( const ChartAxisIndex &index ) const
{
    if ( m_project == 0 || m_manager == 0 ) {
        return 0;
    }
    if ( index.isValid() && index.userData == AxisSet ) {
        int count = 0;
        if ( m_dataShown.showCost && index.number() == 0 ) {
            if ( m_dataShown.showBCWSCost ) ++count;
            if ( m_dataShown.showBCWPCost ) ++count;
            if ( m_dataShown.showACWPCost ) ++count;
        } else { // always effort
            if ( m_dataShown.showBCWSEffort ) ++count;
            if ( m_dataShown.showBCWPEffort ) ++count;
            if ( m_dataShown.showACWPEffort ) ++count;
        }
        //qDebug()<<"dataSetCount:"<<index<<count;
        return count;
    }
    return 0;
}

int NodeChartModel::childCount( const ChartDataIndex &index ) const
{
    return 0;
}

QVariant NodeChartModel::data( const ChartDataIndex &idx, int role ) const
{
    //kDebug()<<idx.number()<<idx.userData<<role;
    if ( role == Qt::ForegroundRole ) {
        if ( idx.axisIndex().number() == 0 ) {
            switch ( idx.userData ) {
                case BCWS: return QColor( Qt::green );
                case BCWP: return QColor( Qt::red );
                case ACWP: return QColor( Qt::blue );
            }
        } else {
            switch ( idx.userData ) {
                case BCWS: return QColor( Qt::black );
                case BCWP: return QColor( Qt::gray );
                case ACWP: return QColor( Qt::yellow );
            }
        }
    }
    return QVariant();
}

QVariant NodeChartModel::data( const ChartDataIndex &idx, const ChartAxisIndex &axis, int role ) const
{
    //NOTE: Just return a "day number" for x-axis as the test PlotWidget doesn't handle dates
    //kDebug()<<idx.number()<<idx.userData<<role<<", "<<axis.number()<<axis.userData;
    if ( role == Qt::DisplayRole ) {
        int axisType = axis.userData;
        bool cost_Y = m_dataShown.showCost && axis.parentId == 0;
        bool effort_Y = m_dataShown.showEffort && ( m_dataShown.showCost ? axis.parentId == 1 : axis.parentId == 0 );
        switch ( idx.userData ) {
            case BCWS: {
                QVariantList lst;
                if ( axisType == X_Axis ) {
                    //kDebug()<<"X-axis"<<idx<<axis;
                    QDate s = startDate();
                    foreach ( const QDate &d, m_bcwp.days().keys() ) {
                        lst << s.daysTo( d );
                    }
                } else if ( axisType == Y_Axis ) {
                    //kDebug()<<"Y-axis"<<idx.number()<<axis.number();
                    if ( cost_Y ) {
                        //kDebug()<<"Y-axis, Cost"<<idx;
                        double c = 0.0;
                        foreach ( const EffortCost &v, m_bcwp.days() ) {
                            c += v.cost();
                            lst << c;
                        }
                    } else if ( effort_Y ) {
                        //kDebug()<<"Y-axis, Hours"<<idx;
                        double c = 0.0;
                        foreach ( const EffortCost &v, m_bcwp.days() ) {
                            c += v.effort().toDouble( Duration::Unit_h );
                            lst << c;
                        }
                    }
                }
                //kDebug()<<"BCWS"<<idx<<lst;
                return lst;
                break; }
            case BCWP: {
                QVariantList lst;
                if ( axisType == X_Axis ) {
                    //kDebug()<<"X-axis"<<idx<<axis;
                    QDate s = startDate();
                    foreach ( const QDate &d, m_bcwp.days().keys() ) {
                        lst << s.daysTo( d );
                    }
                } else if ( axisType == Y_Axis ) {
                    //kDebug()<<"Y-axis"<<idx<<axis;
                    if ( cost_Y ) {
                        //kDebug()<<"Y-axis, Cost"<<idx;
                        foreach ( const EffortCost &v, m_bcwp.days() ) {
                            lst << v.bcwpCost();
                        }
                    } else if ( effort_Y ) {
                        //kDebug()<<"Y-axis, Hours"<<idx;
                        foreach ( const EffortCost &v, m_bcwp.days() ) {
                            lst << v.bcwpEffort();
                        }
                    }

                }
                //kDebug()<<"BCWP"<<idx<<lst;
                return lst;
                break; }
            case ACWP: {
                QVariantList lst;
                if ( axisType == X_Axis ) {
                    //kDebug()<<"X-axis"<<idx;
                    QDate s = startDate();
                    foreach ( const QDate &d, m_acwp.days().keys() ) {
                        lst << s.daysTo( d );
                    }
                } else if ( axisType == Y_Axis ) {
                    //kDebug()<<"Y-axis"<<idx;
                    if ( cost_Y ) {
                        //kDebug()<<"Y-axis, Cost"<<idx;
                        double c = 0.0;
                        foreach ( const EffortCost &v, m_acwp.days() ) {
                            c +=  v.cost();
                            lst << c;
                        }
                    } else if ( effort_Y ) {
                        //kDebug()<<"Y-axis, Hours"<<idx;
                        double c = 0.0;
                        foreach ( const EffortCost &v, m_acwp.days() ) {
                            c +=  v.effort().toDouble( Duration::Unit_h );
                            lst << c;
                        }
                    }
                }
                //kDebug()<<"ACWP"<<idx<<lst;
                return lst;
                break; }
            default:
                kDebug()<<"Invalid userData"<<idx;
                break;
        }
    }
    if ( role == Qt::ForegroundRole ) {
        if ( m_dataShown.showCost && axis.number() == 0 ) {
            //kDebug()<<"cost"<<axis;
            switch ( idx.userData ) {
                case BCWS: return QColor( Qt::darkGreen );
                case BCWP: return QColor( Qt::red );
                case ACWP: return QColor( Qt::darkRed );
            }
        } else {
            //kDebug()<<"effort"<<axis;
            switch ( idx.userData ) {
                case BCWS: return QColor( Qt::black );
                case BCWP: return QColor( Qt::darkGray );
                case ACWP: return QColor( Qt::darkBlue );
            }
        }
        return QVariant();
    }
    if ( role == AbstractChartModel::DataLabelRole ) {
        if ( m_dataShown.showCost && axis.number() == 0 ) {
            //kDebug()<<"cost"<<axis;
            switch ( idx.userData ) {
                case BCWS: return i18nc( "Budgeted Cost of Work Scheduled", "BCWS (Cost)" );
                case BCWP: return i18nc( "Budgeted Cost of Work Performed", "BCWP (Cost)" );
                case ACWP: return i18nc( "Actual Cost of Work Performed", "ACWP (Cost)" );
            }
        } else {
            //kDebug()<<"effort"<<axis;
            switch ( idx.userData ) {
                case BCWS: return i18nc( "Budgeted Cost of Work Scheduled", "BCWS (Effort)" );
                case BCWP: return i18nc( "Budgeted Cost of Work Performed", "BCWP (Effort)" );
                case ACWP: return i18nc( "Actual Cost of Work Performed", "ACWP (Effort)" );
            }
        }
        return QVariant();
    }
    return QVariant();
}


ChartDataIndex NodeChartModel::index( int number, const ChartAxisIndex &idx ) const
{
    if ( ! idx.isValid() ) {
        return ChartDataIndex();
    }
    //kDebug()<<idx;
    int type = 0;
    if ( m_dataShown.showCost ) {
        if ( m_dataShown.showBCWSCost ) type = 1;
        if ( m_dataShown.showBCWPCost ) type += 2;
        if ( m_dataShown.showACWPCost ) type += 4;
    }
    if ( m_dataShown.showEffort ) {
        if ( m_dataShown.showBCWSEffort ) type += 8;
        if ( m_dataShown.showBCWPEffort ) type += 16;
        if ( m_dataShown.showACWPEffort ) type += 32;
    }
    //qDebug()<<"index"<<number<<":"<<idx<<type;
    if ( m_dataShown.showCost && idx.number() == 0 ) {
        switch ( number ) {
            case 0:
                if ( type & 1 ) return createDataIndex( 0, idx, BCWS );
                if ( type & 2 ) return createDataIndex( 1, idx, BCWP );
                if ( type & 4 ) return createDataIndex( 2, idx, ACWP );
                break;
            case 1: {
                if ( (type & 1) && (type & 2) ) return createDataIndex( 1, idx, BCWP );
                if ( type & 4 ) return createDataIndex( 2, idx, ACWP );
                break; }
            case 2:
                if ( type & 4 ) return createDataIndex( 2, idx, ACWP );
                break;
        }
    }
    if ( m_dataShown.showEffort && idx.number() == (m_dataShown.showCost ? 1 : 0) ) {
        switch ( number ) {
            case 0:
                if ( type & 8 ) return createDataIndex( 0, idx, BCWS );
                if ( type & 16 ) return createDataIndex( 1, idx, BCWP );
                if ( type & 32 ) return createDataIndex( 2, idx, ACWP );
                break;
            case 1:
                if ( (type & 8) && (type & 16) ) return createDataIndex( 1, idx, BCWP );
                if ( type & 32 ) return createDataIndex( 2, idx, ACWP );
                break;
            case 2:
                if ( type & 32 ) return createDataIndex( 2, idx, ACWP );
                break;
        }
    }
    return ChartDataIndex();
}
    
ChartDataIndex NodeChartModel::index( int number, const ChartDataIndex &idx ) const
{
    if ( ! idx.isValid() ) {
        return ChartDataIndex();
    }
    // if sections, return valid index
    return ChartDataIndex();
}

int NodeChartModel::axisCount( const ChartAxisIndex &index ) const
{
    if ( m_project == 0 || m_manager == 0 ) {
        return 0;
    }
    if ( ! index.isValid() ) {
        //kDebug()<<index<<"we have max 2; x,y axis pairs";
        int c = 0;
        if ( m_dataShown.showCost ) ++c;
        if ( m_dataShown.showEffort ) ++c;
        return c; // we have one x,y axis pair
    }
    if ( index.userData == AxisSet ) {
        //kDebug()<<index<<"x- and y axis";
        return 2; // x- and y axis
    }
    //kDebug()<<index<<"Axis, no children";
    return 0;
}

ChartAxisIndex NodeChartModel::parent( const ChartAxisIndex &index ) const
{
    //kDebug()<<index;
    if ( ! index.isValid() || index.userData == AxisSet ) {
        return ChartAxisIndex();
    }
    return createAxisIndex( index.parentId, ChartAxisIndex(), AxisSet );
}

QVariant NodeChartModel::axisData( const ChartAxisIndex &index, int role ) const
{
    if ( ! index.isValid() ) {
        // Return common data for the widget, like header, footer, legend...
        return QVariant();
    }
    if ( role == Qt::DisplayRole ) {
        //kDebug()<<"Display:"<<index;
        if ( index.userData == AxisSet ) {
            //kDebug()<<index<<"Axis set";
            return QVariant();
        }
        if ( index.userData == X_Axis ) {
            if ( parent( index ).number() == 0 ) {
                return i18n( "Time" ); // left y-axis
            }
            if ( parent( index ).number() == 1 ) {
                // Top axis label
                return startDate().toString( Qt::ISODate ) + " - " + endDate().toString( Qt::ISODate );
            }
        }
        if ( index.userData == Y_Axis ) {
            //kDebug()<<"Y axis:"<<index;
            if ( m_dataShown.showCost && parent( index ).number() == 0 ) {
                //kDebug()<<"Y-axis Cost";
                return i18n( "Cost" ); // left y-axis
            }
            if ( m_dataShown.showEffort && parent( index ).number() == ( m_dataShown.showCost ? 1 : 0 ) ) {
                //kDebug()<<"Y-axis Hours";
                return i18n( "Hours" ); // right y-axis
            }
        }
        //kDebug()<<index<<"No data";
        return QVariant();
    }
    if ( role == AbstractChartModel::AxisTypeRole ) {
        if ( index.userData == X_Axis ) {
            return AbstractChartModel::Axis_X;
        }
        if ( index.userData == Y_Axis || index.userData == Y_AxisSet ) {
            return AbstractChartModel::Axis_Y;
        }
        return QVariant();
    }
    if ( role == AbstractChartModel::AxisMinRole ) {
        if ( index.userData == X_Axis ) {
            return 0;
        }
        if ( index.userData == Y_Axis ) {
            return 0.0; // both cost and effort
        }
        return QVariant();
    }
    if ( role == AbstractChartModel::AxisMaxRole ) {
        if ( index.userData == X_Axis ) {
            QDate d = startDate();
            QDate e = endDate();
            return d.daysTo( e );
        }
        if ( index.userData == Y_Axis ) {
            if ( m_dataShown.showCost && parent( index ).number() == 0 ) {
                //kDebug()<<"Cost"<<index;
                return qMax( m_bcwp.totalCost(), m_acwp.totalCost() ) * 1.1;
            }
            if ( m_dataShown.showEffort && parent( index ).number() == ( m_dataShown.showCost ? 1 : 0 ) ) {
                //kDebug()<<"Effort"<<index;
                return qMax( m_bcwp.totalEffort().toDouble( Duration::Unit_h ), m_acwp.totalEffort().toDouble( Duration::Unit_h ) ) * 1.1;
            }
        }
        return QVariant();
    }
    if ( role == AbstractChartModel::AxisDataTypeRole ) {
        if ( index.userData == X_Axis ) {
            return QVariant::Date; // x-axis
        }
        if ( index.userData == Y_Axis ) {
            return QVariant::Double; // y-axis
        }
        return QVariant();
    }
    if ( role == AbstractChartModel::AxisStartDateRole ) {
        //kDebug()<<"Display:"<<index;
        if ( index.userData == X_Axis ) {
            return startDate();
        }
        return QDate();
    }
    
    return QVariant();
}

ChartAxisIndex NodeChartModel::axisIndex( int number, const ChartAxisIndex &parent ) const
{
    if ( ! parent.isValid() ) {
        return createAxisIndex( number, parent, AxisSet );
    }
    if ( parent.userData == AxisSet ) {
        if ( number == 0 ) {
            return createAxisIndex( number, parent, X_Axis, parent.number() );
        }
        if ( number == 1 ) {
            return createAxisIndex( number, parent, Y_Axis, parent.number() );
        }
    }
    return ChartAxisIndex();
}

QDate NodeChartModel::startDate() const
{
    QDate d = m_bcwp.startDate();
    if ( m_acwp.startDate().isValid() ) {
        if ( ! d.isValid() || d > m_acwp.startDate() ) {
            d = m_acwp.startDate();
        }
    }
    return d;
}

QDate NodeChartModel::endDate() const
{
    return qMax( m_bcwp.endDate(), m_acwp.endDate() );
}

} //namespace KPlato

#include "kptnodechartmodel.moc"
