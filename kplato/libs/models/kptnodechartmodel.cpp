/* This file is part of the Calligra project
 * Copyright (c) 2008 Dag Andersen <danders@get2net.dk>
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
#include "kptresource.h"

#include <QPointF>
#include <QVariant>
#include <QColor>
#include <QPen>

#include <kdebug.h>

#include "KDChartGlobal"

namespace KPlato
{

ChartItemModel::ChartItemModel( QObject *parent )
    : ItemModelBase( parent )
{
}

QModelIndex ChartItemModel::parent( const QModelIndex &index ) const
{
    Q_UNUSED(index);
    return QModelIndex();
}

int ChartItemModel::columnCount( const QModelIndex &/*parent*/ ) const
{
    return 6;
}

int ChartItemModel::rowCount( const QModelIndex &/*parent */) const
{
    return startDate().daysTo( endDate() ) + 1;
}

QModelIndex ChartItemModel::index( int row, int column, const QModelIndex &parent ) const
{
    if ( m_project == 0 || row < 0 || column < 0 ) {
        //kDebug()<<"No project"<<m_project<<" or illegal row, column"<<row<<column;
        return QModelIndex();
    }
    if ( parent.isValid() ) {
        return QModelIndex();
    }
    return createIndex( row, column );
}

double ChartItemModel::bcwsEffort( int day ) const
{
    return m_bcws.hoursTo( startDate().addDays( day ) );
}

QVariant ChartItemModel::bcwpEffort( int day ) const
{
    QVariant res;
    QDate date = startDate().addDays( day );
    if ( m_bcws.days().contains( date ) ) {
        res = m_bcws.bcwpEffort( date );
    }
    return res;
}

double ChartItemModel::acwpEffort( int day ) const
{
    return m_acwp.hoursTo( startDate().addDays( day ) );
}

double ChartItemModel::bcwsCost( int day ) const
{
    return m_bcws.costTo( startDate().addDays( day ) );
}

QVariant ChartItemModel::bcwpCost( int day ) const
{
    QVariant res;
    QDate date = startDate().addDays( day );
    if ( m_bcws.days().contains( date ) ) {
        res = m_bcws.bcwpCost( date );
    }
    return res;
}

double ChartItemModel::acwpCost( int day ) const
{
    return m_acwp.costTo( startDate().addDays( day ) );
}

QVariant ChartItemModel::data( const QModelIndex &index, int role ) const
{
    QString r;
    if ( role == KDChart::DatasetPenRole ) r = "DatasetPenRole";
    if ( role == KDChart::DatasetBrushRole ) r = "DatasetBrushRole";
    if ( role == KDChart::DataValueLabelAttributesRole ) r = "DataValueLabelAttributesRole";
    if ( role == KDChart::ThreeDAttributesRole ) r = "ThreeDAttributesRole";
    if ( role == KDChart::LineAttributesRole ) r = "LineAttributesRole";
    if ( role == KDChart::ThreeDLineAttributesRole ) r = "ThreeDLineAttributesRole";
    if ( role == KDChart::BarAttributesRole ) r = "BarAttributesRole";
    if ( role == KDChart::StockBarAttributesRole ) r = "StockBarAttributesRole";
    if ( role == KDChart::ThreeDBarAttributesRole ) r = "ThreeDBarAttributesRole";
    if ( role == KDChart::PieAttributesRole ) r = "PieAttributesRole";
    if ( role == KDChart::ThreeDPieAttributesRole ) r = "ThreeDPieAttributesRole";
    if ( role == KDChart::DataHiddenRole ) r = "DataHiddenRole";
    if ( role == KDChart::ValueTrackerAttributesRole ) r = "ValueTrackerAttributesRole";
    if ( role == KDChart::CommentRole ) r = "CommentRole";
    QVariant result;
    if ( role == Qt::DisplayRole ) {
        r = "DisplayRole";
        switch ( index.column() ) {
            case 0: result = bcwsCost( index.row() ); break;
            case 1: result = bcwpCost( index.row() ); break;
            case 2: result = acwpCost( index.row() ); break;
            case 3: result = bcwsEffort( index.row() ); break;
            case 4: result = bcwpEffort( index.row() ); break;
            case 5: result = acwpEffort( index.row() ); break;
            default: break;
        }
        //kDebug()<<index<<r<<result;
        return result;
    }  else if ( role == KDChart::DatasetBrushRole ) {
        return headerData( index.column(), Qt::Horizontal, role );
    }  else if ( role == KDChart::DatasetPenRole ) {
        return headerData( index.column(), Qt::Horizontal, role );
    }
    //kDebug()<<index<<r<<result;
    return result;
}

QVariant ChartItemModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    QVariant result;
    if ( role == Qt::DisplayRole ) {
        if ( orientation == Qt::Horizontal ) {
            switch ( section ) {
                case 0: return i18nc( "Cost based Budgeted Cost of Work Scheduled", "BCWS Cost" );
                case 1: return i18nc( "Cost based Budgeted Cost of Work Performed", "BCWP Cost" );
                case 2: return i18nc( "Cost based Actual Cost of Work Performed", "ACWP Cost" );
                case 3: return i18nc( "Effort based Budgeted Cost of Work Scheduled", "BCWS Effort" );
                case 4: return i18nc( "Effort based Budgeted Cost of Work Performed", "BCWP Effort" );
                case 5: return i18nc( "Effort based Actual Cost of Work Performed", "ACWP Effort" );
                default: return QVariant();
            }
        } else {
            return startDate().addDays( section ).toString( i18nc( "Date format used as chart axis labels. Must follow QDate specification.", "MM.dd" ) );
        }
    }  else if ( role == KDChart::DatasetBrushRole ) {
        if ( orientation == Qt::Horizontal ) {
            switch ( section ) {
                case 0: result = QBrush( Qt::red ); break;
                case 1: result = QBrush( Qt::green ); break;
                case 2: result = QBrush( Qt::blue ); break;
                case 3: result = QBrush( Qt::cyan ); break;
                case 4: result = QBrush( Qt::magenta ); break;
                case 5: result = QBrush( Qt::darkYellow ); break;
                default: break;
            }
            //kDebug()<<this<<orientation<<section<<"DatasetBrushRole"<<result;
            return result;
        }
    }  else if ( role == KDChart::DatasetPenRole ) {
        QPen p;
        p.setBrush( headerData( section, orientation, KDChart::DatasetBrushRole ).value<QBrush>() );
        result = p;
        //kDebug()<<section<<"DatasetPenRole"<<result;
        return result;
    }
    return ItemModelBase::headerData(section, orientation, role);
}

void ChartItemModel::setProject( Project *project )
{
    m_bcws.clear();
    m_acwp.clear();
    if ( m_project ) {
        disconnect( m_project, SIGNAL( projectCalculated( ScheduleManager* ) ), this, SLOT( setScheduleManager( ScheduleManager* ) ) );
        disconnect( m_project, SIGNAL( nodeRemoved( Node* ) ), this, SLOT( slotNodeRemoved( Node* ) ) );
        disconnect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotNodeChanged( Node* ) ) );
        disconnect( m_project, SIGNAL( resourceRemoved( const Resource* ) ), this, SLOT( slotResourceChanged( const Resource* ) ) );
        disconnect( m_project, SIGNAL( resourceChanged( Resource* ) ), this, SLOT( slotResourceChanged( Resource* ) ) );
    }
    m_project = project;
    if ( m_project ) {
        connect( m_project, SIGNAL( projectCalculated( ScheduleManager* ) ), this, SLOT( setScheduleManager( ScheduleManager* ) ) );
        connect( m_project, SIGNAL( nodeRemoved( Node* ) ), this, SLOT( slotNodeRemoved( Node* ) ) );
        connect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotNodeChanged( Node* ) ) );
        connect( m_project, SIGNAL( resourceRemoved( const Resource* ) ), this, SLOT( slotResourceChanged( const Resource* ) ) );
        connect( m_project, SIGNAL( resourceChanged( Resource* ) ), this, SLOT( slotResourceChanged( Resource* ) ) );
    }
    reset();
}

void ChartItemModel::setScheduleManager( ScheduleManager *sm )
{
    m_manager = sm;
    calculate();
    reset();
}

void ChartItemModel::setNodes( const QList<Node*> &nodes )
{
    kDebug()<<nodes;
    m_nodes = nodes;
    calculate();
    reset();
}

void ChartItemModel::addNode( Node *node )
{
    m_nodes.append( node );
    calculate();
    reset();
}

void ChartItemModel::clearNodes()
{
    m_nodes.clear();
    calculate();
    reset();
}

void ChartItemModel::slotNodeRemoved( Node *node )
{
    if ( m_nodes.contains( node ) ) {
        m_nodes.removeAt( m_nodes.indexOf( node ) );
        calculate();
        reset();
        return;
    }
}

void ChartItemModel::slotNodeChanged( Node *node )
{
    if ( m_nodes.contains( node ) ) {
        calculate();
        reset();
        return;
    }
    foreach ( Node *n, m_nodes ) {
        if ( node->isChildOf( n ) ) {
            calculate();
            reset();
            return;
        }
    }
}

void ChartItemModel::slotResourceChanged( Resource* )
{
    calculate();
    reset();
}

void ChartItemModel::slotResourceChanged( const Resource* )
{
    calculate();
    reset();
}

QDate ChartItemModel::startDate() const
{
    QDate d = m_bcws.startDate();
    if ( m_acwp.startDate().isValid() ) {
        if ( ! d.isValid() || d > m_acwp.startDate() ) {
            d = m_acwp.startDate();
        }
    }
    return d;
}

QDate ChartItemModel::endDate() const
{
    return qMax( m_bcws.endDate(), m_acwp.endDate() );
}

void ChartItemModel::calculate()
{
    //kDebug()<<m_project<<m_manager<<m_nodes;
    m_bcws.clear();
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
                    m_bcws += n->bcwpPrDay( m_manager->scheduleId(), ECCT_EffortWork );
                    m_acwp += n->acwp( m_manager->scheduleId() );
                }
            }
        }
    }
    //kDebug()<<"bcwp"<<m_bcws;
    //kDebug()<<"acwp"<<m_acwp;
}


} //namespace KPlato

#include "kptnodechartmodel.moc"
