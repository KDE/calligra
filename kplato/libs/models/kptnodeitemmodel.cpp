/* This file is part of the KDE project
  Copyright (C) 2007 Dag Andersen <danders@get2net.dk>

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

#include "kptnodeitemmodel.h"

#include "kptglobal.h"
#include "kptcommand.h"
#include "kptduration.h"
#include "kptproject.h"
#include "kptnode.h"
#include "kpttaskcompletedelegate.h"

#include <QAbstractItemModel>
#include <QMimeData>
#include <QModelIndex>
#include <QWidget>

#include <kicon.h>
#include <kaction.h>
#include <kglobal.h>
#include <klocale.h>
#include <ktoggleaction.h>
#include <kactionmenu.h>
#include <kstandardaction.h>
#include <kstandardshortcut.h>
#include <kaccelgen.h>
#include <kactioncollection.h>
#include <KRichTextWidget>

#include <kdganttglobal.h>
#include <QtGui>
#include <math.h>

namespace KPlato
{


//--------------------------------------
NodeModel::NodeModel()
    : QObject(),
    m_project( 0 ),
    m_manager( 0 ),
    m_now( QDate::currentDate() ),
    m_prec( 1 )
{
}

const QMetaEnum NodeModel::columnMap() const
{
    return metaObject()->enumerator( metaObject()->indexOfEnumerator("Properties") );
}

void NodeModel::setProject( Project *project )
{
    kDebug()<<m_project<<"->"<<project;
    m_project = project;
}

void NodeModel::setManager( ScheduleManager *sm )
{
    kDebug()<<m_manager<<"->"<<sm;
    m_manager = sm;
}

QVariant NodeModel::name( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return node->name();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::leader( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return node->leader();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::allocation( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return node->requestNameList().join(",");
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::description( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole: {
            KRichTextWidget w( node->description(), 0 );
            w.switchToPlainText();
            QString s = w.textOrHtml();
            int i = s.indexOf( '\n' );
            s = s.left( i );
            if ( i > 0 ) {
                s += "...";
            }
            return s;
        }
        case Qt::ToolTipRole: {
            KRichTextWidget w( node->description(), 0 );
            w.switchToPlainText();
            QString s = w.textOrHtml();
            if ( s.length() > 300 ) {
                s = s.left( 300 ) + "...";
            }
            return s;
        }
        case Qt::EditRole:
            return node->description();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::type( const Node *node, int role ) const
{
    //kDebug()<<node->name()<<", "<<role;
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            return node->typeToString( true );
        case Qt::EditRole:
            return node->type();
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::constraint( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            return node->constraintToString( true );
        case Role::EnumList: 
            return Node::constraintList( true );
        case Qt::EditRole: 
            return node->constraint();
        case Role::EnumListValue: 
            return (int)node->constraint();
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::constraintStartTime( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole: {
            int c = node->constraint();
            if ( ! ( c == Node::MustStartOn || c == Node::StartNotEarlier || c == Node::FixedInterval  ) ) {
                return QVariant();
            }
            return KGlobal::locale()->formatDateTime( node->constraintStartTime() );
        }
        case Qt::ToolTipRole: {
            int c = node->constraint();
            if ( c == Node::MustStartOn || c == Node::StartNotEarlier || c == Node::FixedInterval  ) {
                return KGlobal::locale()->formatDateTime( node->constraintStartTime(), KLocale::LongDate, KLocale::TimeZone );
            }
            break;
        }
        case Qt::EditRole:
            return node->constraintStartTime().dateTime();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::constraintEndTime( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole: {
            int c = node->constraint();
            if ( ! ( c == Node::FinishNotLater || c == Node::MustFinishOn || c == Node::FixedInterval ) ) {
                return QVariant();
            }
            return KGlobal::locale()->formatDateTime( node->constraintEndTime() );
        }
        case Qt::ToolTipRole: {
            int c = node->constraint();
            if ( c == Node::FinishNotLater || c == Node::MustFinishOn || c == Node::FixedInterval ) {
                return KGlobal::locale()->formatDateTime( node->constraintEndTime().dateTime() );
            }
            break;
        }
        case Qt::EditRole:
            return node->constraintEndTime().dateTime();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::estimateType( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            if ( node->type() == Node::Type_Task ) {
                return node->estimate()->typeToString( true );
            }
            return QString();
        case Role::EnumList: 
            return Estimate::typeToStringList( true );
        case Qt::EditRole:
            if ( node->type() == Node::Type_Task ) {
                return node->estimate()->typeToString();
            }
            return QString();
        case Role::EnumListValue: 
            return (int)node->estimate()->type();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::estimateCalendar( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            if ( node->type() == Node::Type_Task ) {
                if ( node->estimate()->calendar() ) {
                    return node->estimate()->calendar()->name();
                }
                return i18n( "None" );
            }
            return QString();
        case Role::EnumList: 
        {
            QStringList lst; lst << i18n( "None" );
            const Node *n = const_cast<Node*>( node )->projectNode();
            if ( n ) {
                lst += static_cast<const Project*>( n )->calendarNames();
            }
            return lst;
        }
        case Qt::EditRole:
            if ( node->type() == Node::Type_Task ) {
                if ( node->estimate()->calendar() == 0 ) {
                    return i18n( "None" );
                }
                return node->estimate()->calendar()->name();
            }
            return QString();
        case Role::EnumListValue:
        {
            if ( node->estimate()->calendar() == 0 ) {
                return 0;
            }
            QStringList lst;
            const Node *n = const_cast<Node*>( node )->projectNode();
            if ( n ) {
                lst = static_cast<const Project*>( n )->calendarNames();
            }
            return lst.indexOf( node->estimate()->calendar()->name() ) + 1;
        }
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::estimate( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            if ( node->type() == Node::Type_Task ) {
                Duration::Unit unit = node->estimate()->unit();
                return KGlobal::locale()->formatNumber( node->estimate()->expectedEstimate(), m_prec ) +  Duration::unitToString( unit, true );
            }
            break;
        case Qt::ToolTipRole:
            if ( node->type() == Node::Type_Task ) {
                Duration::Unit unit = node->estimate()->unit();
                QString s = KGlobal::locale()->formatNumber( node->estimate()->expectedEstimate(), m_prec ) +  Duration::unitToString( unit, true );
                Estimate::Type t = node->estimate()->type();
                if ( t == Estimate::Type_Effort ) {
                    s = i18n( "Estimated effort: %1", s );
                } else {
                    s = i18n( "Estimated duration: %1", s );
                }
                return s;
            }
            break;
        case Qt::EditRole:
            return node->estimate()->expectedEstimate();
        case Role::DurationUnit:
            return static_cast<int>( node->estimate()->unit() );
        case Role::Minimum:
            return m_project->config().minimumDurationUnit();
        case Role::Maximum:
            return m_project->config().maximumDurationUnit();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::optimisticRatio( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
            if ( node->type() == Node::Type_Task ) {
                return node->estimate()->optimisticRatio();
            }
            return QString();
        case Qt::ToolTipRole:
            if ( node->type() == Node::Type_Task ) {
                Duration::Unit unit = node->estimate()->unit();
                QString s = KGlobal::locale()->formatNumber( node->estimate()->optimisticEstimate(), m_prec ) +  Duration::unitToString( unit, true );
                Estimate::Type t = node->estimate()->type();
                if ( t == Estimate::Type_Effort ) {
                    s = i18n( "Optimistic effort: %1", s );
                } else {
                    s = i18n( "Optimistic duration: %1", s );
                }
                return s;
            }
            break;
        case Role::Minimum:
            return -99;
        case Role::Maximum:
            return 0;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::pessimisticRatio( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
            if ( node->type() == Node::Type_Task ) {
                return node->estimate()->pessimisticRatio();
            }
            return QString();
        case Qt::ToolTipRole:
            if ( node->type() == Node::Type_Task ) {
                Duration::Unit unit = node->estimate()->unit();
                QString s = KGlobal::locale()->formatNumber( node->estimate()->pessimisticEstimate(), m_prec ) +  Duration::unitToString( unit, true );
                Estimate::Type t = node->estimate()->type();
                if ( t == Estimate::Type_Effort ) {
                    s = i18n( "Pessimistic effort: %1", s );
                } else {
                    s = i18n( "Pessimistic duration: %1", s );
                }
                return s;
            }
            break;
        case Role::Minimum:
            return 0;
        case Role::Maximum:
            return INT_MAX;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::riskType( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            if ( node->type() == Node::Type_Task ) {
                return node->estimate()->risktypeToString( true );
            }
            return QString();
        case Role::EnumList: 
            return Estimate::risktypeToStringList( true );
        case Qt::EditRole:
            if ( node->type() == Node::Type_Task ) {
                return node->estimate()->risktypeToString();
            }
            return QString();
        case Role::EnumListValue: 
            return (int)node->estimate()->risktype();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::runningAccount( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            if ( node->type() == Node::Type_Task ) {
                Account *a = node->runningAccount();
                return a == 0 ? i18n( "None" ) : a->name();
            }
            break;
        case Qt::ToolTipRole:
            if ( node->type() == Node::Type_Task ) {
                Account *a = node->runningAccount();
                return i18n( "Running account: %1", (a == 0 ? i18n( "None" ) : a->name() ) );
            }
            break;
        case Role::EnumListValue:
        case Qt::EditRole: {
            Account *a = node->runningAccount();
            return a == 0 ? 0 : ( m_project->accounts().costElements().indexOf( a->name() ) + 1 );
        }
        case Role::EnumList: {
            QStringList lst;
            lst << i18n("None");
            lst += m_project->accounts().costElements();
            return lst;
        }
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::startupAccount( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            if ( node->type() == Node::Type_Task  || node->type() == Node::Type_Milestone ) {
                Account *a = node->startupAccount();
                //kDebug()<<node->name()<<": "<<a;
                return a == 0 ? i18n( "None" ) : a->name();
            }
            break;
        case Qt::ToolTipRole:
            if ( node->type() == Node::Type_Task  || node->type() == Node::Type_Milestone ) {
                Account *a = node->startupAccount();
                //kDebug()<<node->name()<<": "<<a;
                return i18n( "Startup account: %1", ( a == 0 ? i18n( "None" ) : a->name() ) );
            }
            break;
        case Role::EnumListValue:
        case Qt::EditRole: {
            Account *a = node->startupAccount();
            return a == 0 ? 0 : ( m_project->accounts().costElements().indexOf( a->name() ) + 1 );
        }
        case Role::EnumList: {
            QStringList lst;
            lst << i18n("None");
            lst += m_project->accounts().costElements();
            return lst;
        }
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::startupCost( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            if ( node->type() == Node::Type_Task || node->type() == Node::Type_Milestone ) {
                return m_project->locale()->formatMoney( node->startupCost() );
            }
            break;
        case Qt::EditRole:
            return m_project->locale()->formatMoney( node->startupCost() );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::shutdownAccount( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            if ( node->type() == Node::Type_Task || node->type() == Node::Type_Milestone ) {
                Account *a = node->shutdownAccount();
                return a == 0 ? i18n( "None" ) : a->name();
            }
            break;
        case Qt::ToolTipRole:
            if ( node->type() == Node::Type_Task || node->type() == Node::Type_Milestone ) {
                Account *a = node->shutdownAccount();
                return i18n( "Shutdown account: %1", ( a == 0 ? i18n( "None" ) : a->name() ) );
            }
            break;
        case Role::EnumListValue:
        case Qt::EditRole: {
            Account *a = node->shutdownAccount();
            return a == 0 ? 0 : ( m_project->accounts().costElements().indexOf( a->name() ) + 1 );
        }
        case Role::EnumList: {
            QStringList lst;
            lst << i18n("None");
            lst += m_project->accounts().costElements();
            return lst;
        }
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::shutdownCost( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            if ( node->type() == Node::Type_Task || node->type() == Node::Type_Milestone ) {
                return m_project->locale()->formatMoney( node->shutdownCost() );
            }
            break;
        case Qt::EditRole:
            return m_project->locale()->formatMoney( node->shutdownCost() );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::startTime( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return KGlobal::locale()->formatDateTime( node->startTime( id() ) );
        case Qt::ToolTipRole:
            //kDebug()<<node->name()<<", "<<role;
            return i18n( "Scheduled start: %1", KGlobal::locale()->formatDateTime( node->startTime( id() ), KLocale::LongDate, KLocale::TimeZone ) );
        case Qt::EditRole:
            return node->startTime( id() ).dateTime();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::endTime( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return KGlobal::locale()->formatDateTime( node->endTime( id() ) );
        case Qt::ToolTipRole:
            //kDebug()<<node->name()<<", "<<role;
            return i18n( "Scheduled finish: %1", KGlobal::locale()->formatDateTime( node->endTime( id() ), KLocale::LongDate, KLocale::TimeZone ) );
        case Qt::EditRole:
            return node->endTime( id() ).dateTime();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::duration( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            if ( node->type() == Node::Type_Task ) {
                Duration::Unit unit = node->estimate()->unit();
                double v = node->duration( id() ).toDouble( unit );
                return KGlobal::locale()->formatNumber( v, m_prec ) +  Duration::unitToString( unit, true );
            } else if ( node->type() == Node::Type_Project ) {
                Duration::Unit unit = Duration::Unit_d;
                double v = node->duration( id() ).toDouble( unit );
                return KGlobal::locale()->formatNumber( v, m_prec ) +  Duration::unitToString( unit, true );
            }
            break;
        case Qt::ToolTipRole:
            if ( node->type() == Node::Type_Task ) {
                Duration::Unit unit = node->estimate()->unit();
                double v = node->duration( id() ).toDouble( unit );
                return i18n( "Scheduled duration: %1", KGlobal::locale()->formatNumber( v, m_prec ) +  Duration::unitToString( unit, true ) );
            } else if ( node->type() == Node::Type_Project ) {
                Duration::Unit unit = Duration::Unit_d;
                double v = node->duration( id() ).toDouble( unit );
                return i18n( "Scheduled duration: %1", KGlobal::locale()->formatNumber( v, m_prec ) +  Duration::unitToString( unit, true ) );
            }
            break;
        case Qt::EditRole: {
            return node->duration( id() ).toDouble( Duration::Unit_h );
        }
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::varianceDuration( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            if ( node->type() == Node::Type_Task ) {
                Duration::Unit unit = node->estimate()->unit();
                double v = node->variance( id(), unit );
                return KGlobal::locale()->formatNumber( v );
            }
            break;
        case Qt::ToolTipRole:
            if ( node->type() == Node::Type_Task ) {
                Duration::Unit unit = node->estimate()->unit();
                double v = node->variance( id(), unit );
                return i18n( "PERT duration variance: %1", KGlobal::locale()->formatNumber( v ) );
            }
            break;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::varianceEstimate( const Estimate *est, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole: {
            Duration::Unit unit = est->unit();
            double v = est->variance( unit );
            //kDebug()<<node->name()<<": "<<v<<" "<<unit<<" : "<<scales;
            return KGlobal::locale()->formatNumber( v );
        }
        case Qt::ToolTipRole: {
            Duration::Unit unit = est->unit();
            double v = est->variance( unit );
            return i18n( "PERT estimate variance: %1", KGlobal::locale()->formatNumber( v ) + Duration::unitToString( unit, true ) );
        }
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::optimisticDuration( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole: {
            Duration d = node->duration( id() );
            d = ( d * ( 100 + node->estimate()->optimisticRatio() ) ) / 100;
            Duration::Unit unit = node->estimate()->unit();
            double v = d.toDouble( unit );
                //kDebug()<<node->name()<<": "<<v<<" "<<unit<<" : "<<scales;
            return KGlobal::locale()->formatNumber( v, m_prec ) +  Duration::unitToString( unit, true );
            break;
        }
        case Qt::ToolTipRole: {
            Duration d = node->duration( id() );
            d = ( d * ( 100 + node->estimate()->optimisticRatio() ) ) / 100;
            Duration::Unit unit = node->estimate()->unit();
            double v = d.toDouble( unit );
            //kDebug()<<node->name()<<": "<<v<<" "<<unit<<" : "<<scales;
            return i18n( "PERT optimistic duration: %1", KGlobal::locale()->formatNumber( v, m_prec ) +  Duration::unitToString( unit, true ) );
            break;
        }
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::optimisticEstimate( const Estimate *est, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole: {
            Duration::Unit unit = est->unit();
            return KGlobal::locale()->formatNumber( est->optimisticEstimate(), m_prec ) +  Duration::unitToString( unit, true );
            break;
        }
        case Qt::ToolTipRole: {
            Duration::Unit unit = est->unit();
            return i18n( "Optimistic estimate: %1", KGlobal::locale()->formatNumber( est->optimisticEstimate(), m_prec ) +  Duration::unitToString( unit, true ) );
            break;
        }
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::pertExpected( const Estimate *est, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole: {
            Duration::Unit unit = est->unit();
            double v = Estimate::scale( est->pertExpected(), unit, est->scales() );
            return KGlobal::locale()->formatNumber( v, m_prec ) +  Duration::unitToString( unit, true );
        }
        case Qt::ToolTipRole: {
            Duration::Unit unit = est->unit();
            double v = Estimate::scale( est->pertExpected(), unit, est->scales() );
            return i18n( "PERT expected estimate: %1", KGlobal::locale()->formatNumber( v, m_prec ) +  Duration::unitToString( unit, true ) );
        }
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::pessimisticDuration( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole: {
            Duration d = node->duration( id() );
            d = ( d * ( 100 + node->estimate()->pessimisticRatio() ) ) / 100;
            Duration::Unit unit = node->estimate()->unit();
            double v = d.toDouble( unit );
            //kDebug()<<node->name()<<": "<<v<<" "<<unit<<" : "<<scales;
            return KGlobal::locale()->formatNumber( v, m_prec ) +  Duration::unitToString( unit, true );
            break;
        }
        case Qt::ToolTipRole: {
            Duration d = node->duration( id() );
            d = ( d * ( 100 + node->estimate()->pessimisticRatio() ) ) / 100;
            Duration::Unit unit = node->estimate()->unit();
            double v = d.toDouble( unit );
            //kDebug()<<node->name()<<": "<<v<<" "<<unit<<" : "<<scales;
            return i18n( "PERT pessimistic duration: %1", KGlobal::locale()->formatNumber( v, m_prec ) +  Duration::unitToString( unit, true ) );
            break;
        }
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::pessimisticEstimate( const Estimate *est, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole: {
            Duration::Unit unit = est->unit();
            return KGlobal::locale()->formatNumber( est->pessimisticEstimate(), m_prec ) +  Duration::unitToString( unit, true );
            break;
        }
        case Qt::ToolTipRole: {
            Duration::Unit unit = est->unit();
            return i18n( "Pessimistic estimate: %1", KGlobal::locale()->formatNumber( est->pessimisticEstimate(), m_prec ) +  Duration::unitToString( unit, true ) );
            break;
        }
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::earlyStart( const Node *node, int role ) const
{
    if ( ! ( node->type() == Node::Type_Task || node->type() == Node::Type_Milestone ) ) {
        return QVariant();
    }
    const Task *t = static_cast<const Task*>( node );
    switch ( role ) {
        case Qt::DisplayRole:
            return KGlobal::locale()->formatDateTime( t->earlyStart( id() ) );
        case Qt::ToolTipRole:
            return KGlobal::locale()->formatDate( t->earlyStart( id() ).date() );
        case Qt::EditRole:
            return t->earlyStart( id() ).dateTime();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::earlyFinish( const Node *node, int role ) const
{
    if ( ! ( node->type() == Node::Type_Task || node->type() == Node::Type_Milestone ) ) {
        return QVariant();
    }
    const Task *t = static_cast<const Task*>( node );
    switch ( role ) {
        case Qt::DisplayRole:
            return KGlobal::locale()->formatDateTime( t->earlyFinish( id() ) );
        case Qt::ToolTipRole:
            return KGlobal::locale()->formatDate( t->earlyFinish( id() ).date() );
        case Qt::EditRole:
            return t->earlyFinish( id() ).dateTime();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::lateStart( const Node *node, int role ) const
{
    if ( ! ( node->type() == Node::Type_Task || node->type() == Node::Type_Milestone ) ) {
        return QVariant();
    }
    const Task *t = static_cast<const Task*>( node );
    switch ( role ) {
        case Qt::DisplayRole:
            return KGlobal::locale()->formatDateTime( t->lateStart( id() ) );
        case Qt::ToolTipRole:
            return KGlobal::locale()->formatDate( t->lateStart( id() ).date() );
        case Qt::EditRole:
            return t->lateStart( id() ).dateTime();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::lateFinish( const Node *node, int role ) const
{
    if ( ! ( node->type() == Node::Type_Task || node->type() == Node::Type_Milestone ) ) {
        return QVariant();
    }
    const Task *t = static_cast<const Task*>( node );
    switch ( role ) {
        case Qt::DisplayRole:
            return KGlobal::locale()->formatDateTime( t->lateFinish( id() ) );
        case Qt::ToolTipRole:
            return KGlobal::locale()->formatDate( t->lateFinish( id() ).date() );
        case Qt::EditRole:
            return t->lateFinish( id() ).dateTime();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::positiveFloat( const Node *node, int role ) const
{
    if ( ! ( node->type() == Node::Type_Task || node->type() == Node::Type_Milestone ) ) {
        return QVariant();
    }
    const Task *t = static_cast<const Task*>( node );
    switch ( role ) {
        case Qt::DisplayRole:
            return t->positiveFloat( id() ).toString( Duration::Format_i18nHourFraction );
        case Qt::ToolTipRole:
            return t->positiveFloat( id() ).toString( Duration::Format_i18nDayTime );
        case Qt::EditRole:
            return t->positiveFloat( id() ).toDouble( Duration::Unit_h );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::freeFloat( const Node *node, int role ) const
{
    if ( ! ( node->type() == Node::Type_Task || node->type() == Node::Type_Milestone ) ) {
        return QVariant();
    }
    const Task *t = static_cast<const Task*>( node );
    switch ( role ) {
        case Qt::DisplayRole:
            return t->freeFloat( id() ).toString( Duration::Format_i18nHourFraction );
        case Qt::ToolTipRole:
            return t->freeFloat( id() ).toString( Duration::Format_i18nDayTime );
        case Qt::EditRole:
            return t->freeFloat( id() ).toDouble( Duration::Unit_h );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::negativeFloat( const Node *node, int role ) const
{
    if ( ! ( node->type() == Node::Type_Task || node->type() == Node::Type_Milestone ) ) {
        return QVariant();
    }
    const Task *t = static_cast<const Task*>( node );
    switch ( role ) {
        case Qt::DisplayRole:
            return t->negativeFloat( id() ).toString( Duration::Format_i18nHourFraction );
        case Qt::ToolTipRole:
            return t->negativeFloat( id() ).toString( Duration::Format_i18nDayTime );
        case Qt::EditRole:
            return t->negativeFloat( id() ).toDouble( Duration::Unit_h );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::startFloat( const Node *node, int role ) const
{
    if ( ! ( node->type() == Node::Type_Task || node->type() == Node::Type_Milestone ) ) {
        return QVariant();
    }
    const Task *t = static_cast<const Task*>( node );
    switch ( role ) {
        case Qt::DisplayRole:
            return t->startFloat( id() ).toString( Duration::Format_i18nHourFraction );
        case Qt::ToolTipRole:
            return t->startFloat( id() ).toString( Duration::Format_i18nDayTime );
        case Qt::EditRole:
            return t->startFloat( id() ).toDouble( Duration::Unit_h );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::finishFloat( const Node *node, int role ) const
{
    if ( ! ( node->type() == Node::Type_Task || node->type() == Node::Type_Milestone ) ) {
        return QVariant();
    }
    const Task *t = static_cast<const Task*>( node );
    switch ( role ) {
        case Qt::DisplayRole:
            return t->finishFloat( id() ).toString( Duration::Format_i18nHourFraction );
        case Qt::ToolTipRole:
            return t->finishFloat( id() ).toString( Duration::Format_i18nDayTime );
        case Qt::EditRole:
            t->finishFloat( id() ).toDouble( Duration::Unit_h );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::assignedResources( const Node *node, int role ) const
{
    if ( node->type() != Node::Type_Task ) {
        return QVariant();
    }
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return node->assignedNameList( id() ).join(",");
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}


QVariant NodeModel::completed( const Node *node, int role ) const
{
    if ( ! ( node->type() == Node::Type_Task || node->type() == Node::Type_Milestone ) ) {
        return QVariant();
    }
    const Task *t = static_cast<const Task*>( node );
    switch ( role ) {
        case Qt::DisplayRole:
            return t->completion().percentFinished();
        case Qt::EditRole:
            return t->completion().percentFinished();
        case Qt::ToolTipRole:
            return i18n( "Task is %1% completed", t->completion().percentFinished() );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::status( const Node *node, int role ) const
{
    if ( ! ( node->type() == Node::Type_Task || node->type() == Node::Type_Milestone ) ) {
        return QVariant();
    }
    const Task *t = static_cast<const Task*>( node );
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole: {
            int st = t->state( id() );
            if ( st & Node::State_NotScheduled ) {
                return SchedulingState::notScheduled();
            }
            if ( st & Node::State_Finished ) {
                if ( st & Node::State_FinishedLate ) {
                    return i18n( "Finished late" );
                }
                if ( st & Node::State_FinishedEarly ) {
                    return i18n( "Finished early" );
                }
                return i18n( "Finished" );
            }
            if ( st & Node::State_Running ) {
                return i18n( "Running" );
            }
            if ( st & Node::State_Started ) {
                if ( st & Node::State_StartedLate ) {
                    return i18n( "Started late" );
                }
                if ( st & Node::State_StartedEarly ) {
                    return i18n( "Started early" );
                }
                return i18n( "Started" );
            }
            if ( st & Node::State_ReadyToStart ) {
                return i18n( "Can start" );
            }
            if ( st & Node::State_NotReadyToStart ) {
                return i18n( "Cannot start" );
            }
            return i18n( "Not started" );
            break;
        }
        case Qt::EditRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::startedTime( const Node *node, int role ) const
{
    if ( ! ( node->type() == Node::Type_Task || node->type() == Node::Type_Milestone ) ) {
        return QVariant();
    }
    const Task *t = static_cast<const Task*>( node );
    switch ( role ) {
        case Qt::DisplayRole:
            if ( t->completion().isStarted() ) {
                return KGlobal::locale()->formatDateTime( t->completion().startTime() );
            }
            break;
        case Qt::ToolTipRole:
            if ( t->completion().isStarted() ) {
                return i18n( "Actual start: %1", KGlobal::locale()->formatDate( t->completion().startTime().date(), KLocale::LongDate ) );
            }
            break;
        case Qt::EditRole:
            if ( t->completion().isStarted() ) {
                return t->completion().startTime().dateTime();
            }
            return QDateTime::currentDateTime();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::isStarted( const Node *node, int role ) const
{
    if ( ! ( node->type() == Node::Type_Task || node->type() == Node::Type_Milestone ) ) {
        return QVariant();
    }
    const Task *t = static_cast<const Task*>( node );
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
            return t->completion().isStarted();
        case Qt::ToolTipRole:
            if ( t->completion().isStarted() ) {
                return i18n( "The task started at: %1", KGlobal::locale()->formatDate( t->completion().startTime().date(), KLocale::LongDate ) );
            }
            return i18n( "The task is not started" );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::finishedTime( const Node *node, int role ) const
{
    if ( ! ( node->type() == Node::Type_Task || node->type() == Node::Type_Milestone ) ) {
        return QVariant();
    }
    const Task *t = static_cast<const Task*>( node );
    switch ( role ) {
        case Qt::DisplayRole:
            if ( t->completion().isFinished() ) {
                return KGlobal::locale()->formatDateTime( t->completion().finishTime() );
            }
            break;
        case Qt::ToolTipRole:
            if ( t->completion().isFinished() ) {
                return i18n( "Actual finish: %1", KGlobal::locale()->formatDateTime( t->completion().finishTime(), KLocale::LongDate, KLocale::TimeZone ) );
            }
            break;
        case Qt::EditRole:
            if ( t->completion().isFinished() ) {
                return t->completion().finishTime().dateTime();
            }
            break;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::isFinished( const Node *node, int role ) const
{
    if ( ! ( node->type() == Node::Type_Task || node->type() == Node::Type_Milestone ) ) {
        return QVariant();
    }
    const Task *t = static_cast<const Task*>( node );
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
            return t->completion().isFinished();
        case Qt::ToolTipRole:
            if ( t->completion().isFinished() ) {
                return i18n( "The task finished at: %1", KGlobal::locale()->formatDate( t->completion().finishTime().date(), KLocale::LongDate ) );
            }
            return i18n( "The task is not finished" );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::plannedEffortTo( const Node *node, int role ) const
{
    KLocale *l = KGlobal::locale();
    switch ( role ) {
        case Qt::DisplayRole:
            return node->plannedEffortTo( m_now, id() ).format();
        case Qt::ToolTipRole:
            return i18n( "Planned effort until %1: %2", l->formatDate( m_now ), node->plannedEffortTo( m_now, id() ).toString( Duration::Format_i18nHour ) );
        case Qt::EditRole:
            return node->plannedEffortTo( m_now, id() ).toDouble( Duration::Unit_h );
        case Role::DurationUnit:
            return static_cast<int>( Duration::Unit_h );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::actualEffortTo( const Node *node, int role ) const
{
    KLocale *l = KGlobal::locale();
    switch ( role ) {
        case Qt::DisplayRole:
            return node->actualEffortTo( m_now ).format();
        case Qt::ToolTipRole:
            //kDebug()<<m_now<<node;
            return i18n( "Actual effort used up to %1: %2", l->formatDate( m_now ), node->actualEffortTo( m_now ).toString( Duration::Format_i18nHour ) );
        case Qt::EditRole:
            return node->actualEffortTo( m_now ).toDouble( Duration::Unit_h );
        case Role::DurationUnit:
            return static_cast<int>( Duration::Unit_h );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::remainingEffort( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole: {
            const Task *t = dynamic_cast<const Task*>( node );
            if ( t ) {
                return t->completion().remainingEffort().format();
            }
            break;
        }
        case Qt::ToolTipRole: {
            const Task *t = dynamic_cast<const Task*>( node );
            if ( t ) {
                return i18n( "Remaining effort: %1", t->completion().remainingEffort().toString( Duration::Format_i18nHour ) );
            }
            break;
        }
        case Qt::EditRole: {
            const Task *t = dynamic_cast<const Task*>( node );
            return t->completion().remainingEffort().toDouble( Duration::Unit_h );
        }
        case Role::DurationUnit:
            return static_cast<int>( Duration::Unit_h );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::plannedCostTo( const Node *node, int role ) const
{
    KLocale *l = m_project->locale();
    switch ( role ) {
        case Qt::DisplayRole:
            return l->formatMoney( node->plannedCostTo( m_now ) );
        case Qt::ToolTipRole:
            return i18n( "Planned cost until %1: %2", l->formatDate( m_now ), l->formatMoney( node->plannedCostTo( m_now ) ) );
        case Qt::EditRole:
            return node->plannedCostTo( m_now );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::actualCostTo( const Node *node, int role ) const
{
    KLocale *l = m_project->locale();
    switch ( role ) {
        case Qt::DisplayRole:
            return l->formatMoney( node->actualCostTo( m_now ).cost() );
        case Qt::ToolTipRole:
            return i18n( "Actual cost until %1: %2", l->formatDate( m_now ), l->formatMoney( node->actualCostTo( m_now ).cost() ) );
        case Qt::EditRole:
            return node->actualCostTo( m_now ).cost();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::note( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            if ( node->type() == Node::Type_Task ) {
                Node *n = const_cast<Node*>( node );
                return static_cast<Task*>( n )->completion().note();
            }
            break;
        case Qt::EditRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::resourceIsMissing( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return node->resourceError( id() );
            break;
        case Qt::ToolTipRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::resourceIsOverbooked( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return node->resourceOverbooked( id() );
            break;
        case Qt::ToolTipRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::resourceIsNotAvailable( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return node->resourceNotAvailable( id() );
            break;
        case Qt::ToolTipRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::schedulingConstraintsError( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return node->schedulingError( id() );
            break;
        case Qt::ToolTipRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::nodeIsNotScheduled( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return node->notScheduled( id() );
        case Qt::EditRole:
            return node->notScheduled( id() );
        case Qt::ToolTipRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::effortNotMet( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return node->effortMetError( id() );
            break;
        case Qt::ToolTipRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::wbsCode( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return node->wbsCode();
            break;
        case Qt::ToolTipRole:
            return i18n( "Work breakdown structure code: %1", node->wbsCode() );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::nodeLevel( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return node->level();
        case Qt::ToolTipRole:
            return i18n( "Node level: %1", node->level() );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::nodeBCWS( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return m_project->locale()->formatMoney( node->bcws( m_now, id() ), QString(), 0 );
        case Qt::ToolTipRole:
            return i18n( "Budgeted Cost of Work Scheduled at %1: %2", m_now.toString(), m_project->locale()->formatMoney( node->bcws( m_now, id() ), QString(), 0 ) );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::nodeBCWP( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return m_project->locale()->formatMoney( node->bcwp( id() ), QString(), 0 );
        case Qt::ToolTipRole:
            return i18n( "Budgeted Cost of Work Performed at %1: %2", m_now.toString(), m_project->locale()->formatMoney( node->bcwp( id() ), QString(), 0 ) );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::nodeACWP( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return m_project->locale()->formatMoney( node->acwp( m_now, id() ).cost(), QString(), 0 );
        case Qt::ToolTipRole:
            return i18n( "Actual Cost of Work Performed at %1: %2", m_now.toString(), m_project->locale()->formatMoney( node->acwp( m_now, id() ).cost() ) );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::nodePerformanceIndex( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return KGlobal::locale()->formatNumber( node->schedulePerformanceIndex( m_now, id() ), 2 );
        case Qt::ToolTipRole:
            return i18n( "Schedule Performance Index at %1: %2", m_now.toString(), KGlobal::locale()->formatNumber( node->schedulePerformanceIndex( m_now, id() ), 2 ) );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::nodeIsCritical( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return node->isCritical( id() );
        case Qt::ToolTipRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::nodeInCriticalPath( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return node->inCriticalPath( id() );
        case Qt::ToolTipRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::wpOwnerName( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            if ( static_cast<const Task*>( node )->wpTransmitionStatus() == WorkPackage::TS_None ) {
                return i18nc( "Not available", "NA" );
            }
            return static_cast<const Task*>( node )->wpOwnerName();
        case Qt::ToolTipRole: {
            int sts = wpTransmitionStatus( node, Qt::EditRole ).toInt();
            QString t = wpTransmitionTime( node, Qt::DisplayRole ).toString();
            if ( sts == WorkPackage::TS_Send ) {
                return i18n( "Latest work package sent to %1 at %2", static_cast<const Task*>( node )->wpOwnerName(), t );
            }
            if ( sts == WorkPackage::TS_Receive ) {
                return i18n( "Latest work package received from %1 at %2", static_cast<const Task*>( node )->wpOwnerName(), t );
            }
            return i18n( "Not available" );
        }
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::wpTransmitionStatus( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            if ( static_cast<const Task*>( node )->wpTransmitionStatus() == WorkPackage::TS_None ) {
                return i18nc( "Not available", "NA" );
            }
            return WorkPackage::transmitionStatusToString( static_cast<const Task*>( node )->wpTransmitionStatus(), true );
        case Qt::EditRole:
            return static_cast<const Task*>( node )->wpTransmitionStatus();
        case Qt::ToolTipRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::wpTransmitionTime( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            if ( static_cast<const Task*>( node )->wpTransmitionStatus() == WorkPackage::TS_None ) {
                return i18nc( "Not available", "NA" );
            }
            return KGlobal::locale()->formatDateTime( static_cast<const Task*>( node )->wpTransmitionTime() );
        case Qt::ToolTipRole: {
            int sts = wpTransmitionStatus( node, Qt::EditRole ).toInt();
            QString t = wpTransmitionTime( node, Qt::DisplayRole ).toString();
            if ( sts == WorkPackage::TS_Send ) {
                return i18n( "Latest work package sent: %1", t );
            }
            if ( sts == WorkPackage::TS_Receive ) {
                return i18n( "Latest work package received: %1", t );
            }
            return i18n( "Not available" );
        }
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant NodeModel::data( const Node *n, int property, int role ) const
{
    QVariant result;
    switch ( property ) {
        // Edited by user
        case NodeName: result = name( n, role ); break;
        case NodeType: result = type( n, role ); break;
        case NodeResponsible: result = leader( n, role ); break;
        case NodeAllocation: result = allocation( n, role ); break;
        case NodeEstimateType: result = estimateType( n, role ); break;
        case NodeEstimateCalendar: result = estimateCalendar( n, role ); break;
        case NodeEstimate: result = estimate( n, role ); break;
        case NodeOptimisticRatio: result = optimisticRatio( n, role ); break;
        case NodePessimisticRatio: result = pessimisticRatio( n, role ); break;
        case NodeRisk: result = riskType( n, role ); break;
        case NodeConstraint: result = constraint( n, role ); break;
        case NodeConstraintStart: result = constraintStartTime( n, role ); break;
        case NodeConstraintEnd: result = constraintEndTime( n, role ); break;
        case NodeRunningAccount: result = runningAccount( n, role ); break;
        case NodeStartupAccount: result = startupAccount( n, role ); break;
        case NodeStartupCost: result = startupCost( n, role ); break;
        case NodeShutdownAccount: result = shutdownAccount( n, role ); break;
        case NodeShutdownCost: result = shutdownCost( n, role ); break;
        case NodeDescription: result = description( n, role ); break;
        
        // Based on edited values
        case NodeExpected: result = pertExpected( n->estimate(), role ); break;
        case NodeVarianceEstimate: result = varianceEstimate( n->estimate(), role ); break;
        case NodeOptimistic: result = optimisticEstimate( n->estimate(), role ); break;
        case NodePessimistic: result = pessimisticEstimate( n->estimate(), role ); break;

        // After scheduling
        case NodeStartTime: result = startTime( n, role ); break;
        case NodeEndTime: result = endTime( n, role ); break;
        case NodeEarlyStart: result = earlyStart( n, role ); break;
        case NodeEarlyFinish: result = earlyFinish( n, role ); break;
        case NodeLateStart: result = lateStart( n, role ); break;
        case NodeLateFinish: result = lateFinish( n, role ); break;
        case NodePositiveFloat: result = positiveFloat( n, role ); break;
        case NodeFreeFloat: result = freeFloat( n, role ); break;
        case NodeNegativeFloat: result = negativeFloat( n, role ); break;
        case NodeStartFloat: result = startFloat( n, role ); break;
        case NodeFinishFloat: result = finishFloat( n, role ); break;
        case NodeAssignments: result = assignedResources( n, role ); break;

        // Based on scheduled values
        case NodeDuration: result = duration( n, role ); break;
        case NodeVarianceDuration: result = varianceDuration( n, role ); break;
        case NodeOptimisticDuration: result = optimisticDuration( n, role ); break;
        case NodePessimisticDuration: result = pessimisticDuration( n, role ); break;
        
        // Completion
        case NodeStatus: result = status( n, role ); break;
        case NodeCompleted: result = completed( n, role ); break;
        case NodePlannedEffort: result = plannedEffortTo( n, role ); break;
        case NodeActualEffort: result = actualEffortTo( n, role ); break;
        case NodeRemainingEffort: result = remainingEffort( n, role ); break;
        case NodePlannedCost: result = plannedCostTo( n, role ); break;
        case NodeActualCost: result = actualCostTo( n, role ); break;
        case NodeActualStart: result = startedTime( n, role ); break;
        case NodeStarted: result = isStarted( n, role ); break;
        case NodeActualFinish: result = finishedTime( n, role ); break;
        case NodeFinished: result = isFinished( n, role ); break;
        case NodeStatusNote: result = note( n, role ); break;
        
        // Scheduling errors
        case NodeNotScheduled: result = nodeIsNotScheduled( n, role ); break;
        case NodeAssignmentMissing: result = resourceIsMissing( n, role ); break;
        case NodeResourceOverbooked: result = resourceIsOverbooked( n, role ); break;
        case NodeResourceUnavailable: result = resourceIsNotAvailable( n, role ); break;
        case NodeConstraintsError: result = schedulingConstraintsError( n, role ); break;
        case NodeEffortNotMet: result = effortNotMet( n, role ); break;
        
        case NodeWBSCode: result = wbsCode( n, role ); break;
        case NodeLevel: result = nodeLevel( n, role ); break;
        
        // Performance
        case NodeBCWS: result = nodeBCWS( n, role ); break;
        case NodeBCWP: result = nodeBCWP( n, role ); break;
        case NodeACWP: result = nodeACWP( n, role ); break;
        case NodePerformanceIndex: result = nodePerformanceIndex( n, role ); break;
        case NodeCritical: result = nodeIsCritical( n, role ); break;
        case NodeCriticalPath: result = nodeInCriticalPath( n, role ); break;

        case WPOwnerName: result = wpOwnerName( n, role ); break;
        case WPTransmitionStatus: result = wpTransmitionStatus( n, role ); break;
        case WPTransmitionTime: result = wpTransmitionTime( n, role ); break;

        default:
            //kDebug()<<"Invalid property number: "<<property;;
            return result;
    }
    return result;
}

int NodeModel::propertyCount() const
{
    return columnMap().keyCount();
}

bool NodeModel::setData( Node *node, int property, const QVariant & value, int role )
{
    return false;
}

QVariant NodeModel::headerData( int section, int role )
{
    if ( role == Qt::DisplayRole ) {
        switch ( section ) {
            case NodeName: return i18n( "Name" );
            case NodeType: return i18n( "Type" );
            case NodeResponsible: return i18n( "Responsible" );
            case NodeAllocation: return i18n( "Allocation" );
            case NodeEstimateType: return i18n( "Estimate Type" );
            case NodeEstimateCalendar: return i18n( "Calendar" );
            case NodeEstimate: return i18n( "Estimate" );
            case NodeOptimisticRatio: return i18n( "Optimistic" ); // Ratio
            case NodePessimisticRatio: return i18n( "Pessimistic" ); // Ratio
            case NodeRisk: return i18n( "Risk" );
            case NodeConstraint: return i18n( "Constraint" );
            case NodeConstraintStart: return i18n( "Constraint Start" );
            case NodeConstraintEnd: return i18n( "Constraint End" );
            case NodeRunningAccount: return i18n( "Running Account" );
            case NodeStartupAccount: return i18n( "Startup Account" );
            case NodeStartupCost: return i18n( "Startup Cost" );
            case NodeShutdownAccount: return i18n( "Shutdown Account" );
            case NodeShutdownCost: return i18n( "Shutdown Cost" );
            case NodeDescription: return i18n( "Description" );
            
            // Based on edited values
            case NodeExpected: return i18n( "Expected" );
            case NodeVarianceEstimate: return i18n( "Variance (Est)" );
            case NodeOptimistic: return i18n( "Optimistic" );
            case NodePessimistic: return i18n( "Pessimistic" );
            
            // After scheduling
            case NodeStartTime: return i18n( "Start Time" );
            case NodeEndTime: return i18n( "End Time" );
            case NodeEarlyStart: return i18n( "Early Start" );
            case NodeEarlyFinish: return i18n( "Early Finish" );
            case NodeLateStart: return i18n( "Late Start" );
            case NodeLateFinish: return i18n( "Late Finish" );
            case NodePositiveFloat: return i18n( "Positive Float" );
            case NodeFreeFloat: return i18n( "Free Float" );
            case NodeNegativeFloat: return i18n( "Negative Float" );
            case NodeStartFloat: return i18n( "Start Float" );
            case NodeFinishFloat: return i18n( "Finish Float" );
            case NodeAssignments: return i18n( "Assignments" );
            
            // Based on scheduled values
            case NodeDuration: return i18n( "Duration" );
            case NodeVarianceDuration: return i18n( "Variance (Dur)" );
            case NodeOptimisticDuration: return i18n( "Optimistic (Dur)" );
            case NodePessimisticDuration: return i18n( "Pessimistic (Dur)" );

            // Completion
            case NodeStatus: return i18n( "Status" );
            // xgettext: no-c-format
            case NodeCompleted: return i18n( "% Completed" );
            case NodePlannedEffort: return i18n( "Planned Effort" );
            case NodeActualEffort: return i18n( "Actual Effort" );
            case NodeRemainingEffort: return i18n( "Remaining Effort" );
            case NodePlannedCost: return i18n( "Planned Cost" );
            case NodeActualCost: return i18n( "Actual Cost" );
            case NodeActualStart: return i18n( "Actual Start" );
            case NodeStarted: return i18n( "Started" );
            case NodeActualFinish: return i18n( "Actual Finish" );
            case NodeFinished: return i18n( "Finished" );
            case NodeStatusNote: return i18n( "Status Note" );
            
            // Scheduling errors
            case NodeNotScheduled: return i18n( "Not Scheduled" );
            case NodeAssignmentMissing: return i18n( "Assignment Missing" );
            case NodeResourceOverbooked: return i18n( "Resource Overbooked" );
            case NodeResourceUnavailable: return i18n( "Resource Unavailable" );
            case NodeConstraintsError: return i18n( "Constraints Error" );
            case NodeEffortNotMet: return i18n( "Effort Not Met" );
            
            case NodeWBSCode: return i18n( "WBS Code" );
            case NodeLevel: return i18nc( "Node level", "Level" );
            
            // Performance
            case NodeBCWS: return i18nc( "Budgeted Cost of Work Scheduled", "BCWS" );
            case NodeBCWP: return i18nc( "Budgeted Cost of Work Performed", "BCWP" );
            case NodeACWP: return i18nc( "Actual Cost of Work Performed", "ACWP" );
            case NodePerformanceIndex: return i18nc( "Schedule Performance Index", "SPI" );
            case NodeCritical: return i18n( "Critical" );
            case NodeCriticalPath: return i18n( "Critical Path" );
            
            // Work package handling
            case WPOwnerName: return i18n( "Owner" );
            case WPTransmitionStatus: return i18n( "Status" );
            case WPTransmitionTime: return i18n( "Time" );

            default: return QVariant();
        }
    }
    if ( role == Qt::ToolTipRole ) {
        switch ( section ) {
            case NodeName: return ToolTip::nodeName();
            case NodeType: return ToolTip::nodeType();
            case NodeResponsible: return ToolTip::nodeResponsible();
            case NodeAllocation: return ToolTip::allocation();
            case NodeEstimateType: return ToolTip::estimateType();
            case NodeEstimateCalendar: return ToolTip::estimateCalendar();
            case NodeEstimate: return ToolTip::estimate();
            case NodeOptimisticRatio: return ToolTip::optimisticRatio();
            case NodePessimisticRatio: return ToolTip::pessimisticRatio();
            case NodeRisk: return ToolTip::riskType();
            case NodeConstraint: return ToolTip::nodeConstraint();
            case NodeConstraintStart: return ToolTip::nodeConstraintStart();
            case NodeConstraintEnd: return ToolTip::nodeConstraintEnd();
            case NodeRunningAccount: return ToolTip::nodeRunningAccount();
            case NodeStartupAccount: return ToolTip::nodeStartupAccount();
            case NodeStartupCost: return ToolTip::nodeStartupCost();
            case NodeShutdownAccount: return ToolTip::nodeShutdownAccount();
            case NodeShutdownCost: return ToolTip::nodeShutdownCost();
            case NodeDescription: return ToolTip::nodeDescription();

            // Based on edited values
            case NodeExpected: return ToolTip::estimateExpected();
            case NodeVarianceEstimate: return ToolTip::estimateVariance();
            case NodeOptimistic: return ToolTip::estimateOptimistic();
            case NodePessimistic: return ToolTip::estimatePessimistic();
            
            // After scheduling
            case NodeStartTime: return ToolTip::nodeStartTime();
            case NodeEndTime: return ToolTip::nodeEndTime();
            case NodeEarlyStart: return ToolTip::nodeEarlyStart();
            case NodeEarlyFinish: return ToolTip::nodeEarlyFinish();
            case NodeLateStart: return ToolTip::nodeLateStart();
            case NodeLateFinish: return ToolTip::nodeLateFinish();
            case NodePositiveFloat: return ToolTip::nodePositiveFloat();
            case NodeFreeFloat: return ToolTip::nodeFreeFloat();
            case NodeNegativeFloat: return ToolTip::nodeNegativeFloat();
            case NodeStartFloat: return ToolTip::nodeStartFloat();
            case NodeFinishFloat: return ToolTip::nodeFinishFloat();
            case NodeAssignments: return ToolTip::nodeAssignment();

            // Based on scheduled values
            case NodeDuration: return ToolTip::nodeDuration();
            case NodeVarianceDuration: return ToolTip::nodeVarianceDuration();
            case NodeOptimisticDuration: return ToolTip::nodeOptimisticDuration();
            case NodePessimisticDuration: return ToolTip::nodePessimisticDuration();

            // Completion
            case NodeStatus: return ToolTip::nodeStatus();
            case NodeCompleted: return ToolTip::nodeCompletion();
            case NodePlannedEffort: return ToolTip::nodePlannedEffortTo();
            case NodeActualEffort: return ToolTip::nodeActualEffortTo();
            case NodeRemainingEffort: return ToolTip::nodeRemainingEffort();
            case NodePlannedCost: return ToolTip::nodePlannedCostTo();
            case NodeActualCost: return ToolTip::nodeActualCostTo();
            case NodeActualStart: return ToolTip::completionStartedTime();
            case NodeStarted: return ToolTip::completionStarted();
            case NodeActualFinish: return ToolTip::completionFinishedTime();
            case NodeFinished: return ToolTip::completionFinished();
            case NodeStatusNote: return ToolTip::completionStatusNote();
    
            // Scheduling errors
            case NodeNotScheduled: return ToolTip::nodeNotScheduled();
            case NodeAssignmentMissing: return ToolTip::nodeAssignmentMissing();
            case NodeResourceOverbooked: return ToolTip::nodeResourceOverbooked();
            case NodeResourceUnavailable: return ToolTip::nodeResourceUnavailable();
            case NodeConstraintsError: return ToolTip::nodeConstraintsError();
            case NodeEffortNotMet: return ToolTip::nodeEffortNotMet();
            
            case NodeWBSCode: return ToolTip::nodeWBS();
            case NodeLevel: return ToolTip::nodeLevel();
            
            // Performance
            case NodeBCWS: return ToolTip::nodeBCWS();
            case NodeBCWP: return ToolTip::nodeBCWP();
            case NodeACWP: return ToolTip::nodeACWP();
            case NodePerformanceIndex: return ToolTip::nodePerformanceIndex();
            
            // Work package handling FIXME
            case WPOwnerName: return i18n( "Work package owner" );
            case WPTransmitionStatus: return i18n( "Work package status" );
            case WPTransmitionTime: return i18n( "Work package send/receive time" );

            default: return QVariant();
        }
    }
    return QVariant();
}

//----------------------------
NodeItemModel::NodeItemModel( QObject *parent )
    : ItemModelBase( parent ),
    m_node( 0 )
{
}

NodeItemModel::~NodeItemModel()
{
}
    
void NodeItemModel::slotNodeToBeInserted( Node *parent, int row )
{
    //kDebug()<<parent->name()<<"; "<<row;
    Q_ASSERT( m_node == 0 );
    m_node = parent;
    beginInsertRows( index( parent ), row, row );
}

void NodeItemModel::slotNodeInserted( Node *node )
{
    //kDebug()<<node->parentNode()->name()<<"-->"<<node->name();
    Q_ASSERT( node->parentNode() == m_node );
    endInsertRows();
    m_node = 0;
    emit nodeInserted( node );
}

void NodeItemModel::slotNodeToBeRemoved( Node *node )
{
    //kDebug()<<node->name();
    Q_ASSERT( m_node == 0 );
    m_node = node;
    int row = index( node ).row();
    beginRemoveRows( index( node->parentNode() ), row, row );
}

void NodeItemModel::slotNodeRemoved( Node *node )
{
    //kDebug()<<node->name();
    Q_ASSERT( node == m_node );
    endRemoveRows();
    m_node = 0;
}

void NodeItemModel::slotLayoutChanged()
{
    //kDebug()<<node->name();
    emit layoutAboutToBeChanged();
    emit layoutChanged();
}

void NodeItemModel::slotWbsDefinitionChanged()
{
    kDebug();
    if ( m_project == 0 ) {
        return;
    }
    foreach ( Node *n, m_project->allNodes() ) {
        int row = n->parentNode()->indexOf( n );
        emit dataChanged( createIndex( row, NodeModel::NodeWBSCode, n ), createIndex( row, NodeModel::NodeWBSCode, n ) );
    }
}


void NodeItemModel::setProject( Project *project )
{
    if ( m_project ) {
        disconnect( m_project, SIGNAL( localeChanged() ), this, SLOT( slotLayoutChanged() ) );
        disconnect( m_project, SIGNAL( wbsDefinitionChanged() ), this, SLOT( slotWbsDefinitionChanged() ) );
        disconnect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotNodeChanged( Node* ) ) );
        disconnect( m_project, SIGNAL( nodeToBeAdded( Node*, int ) ), this, SLOT( slotNodeToBeInserted(  Node*, int ) ) );
        disconnect( m_project, SIGNAL( nodeToBeRemoved( Node* ) ), this, SLOT( slotNodeToBeRemoved( Node* ) ) );
        
        disconnect( m_project, SIGNAL( nodeToBeMoved( Node* ) ), this, SLOT( slotLayoutToBeChanged() ) );
        disconnect( m_project, SIGNAL( nodeMoved( Node* ) ), this, SLOT( slotLayoutChanged() ) );
    
        disconnect( m_project, SIGNAL( nodeAdded( Node* ) ), this, SLOT( slotNodeInserted( Node* ) ) );
        disconnect( m_project, SIGNAL( nodeRemoved( Node* ) ), this, SLOT( slotNodeRemoved( Node* ) ) );
        //disconnect( m_project, SIGNAL( nodeMoved( Node* ) ), this, SLOT( slotLayoutChanged() ) );
    }
    m_project = project;
    kDebug()<<this<<m_project<<"->"<<project;
    m_nodemodel.setProject( project );
    if ( project ) {
        connect( m_project, SIGNAL( localeChanged() ), this, SLOT( slotLayoutChanged() ) );
        connect( m_project, SIGNAL( wbsDefinitionChanged() ), this, SLOT( slotWbsDefinitionChanged() ) );
        connect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotNodeChanged( Node* ) ) );
        connect( m_project, SIGNAL( nodeToBeAdded( Node*, int ) ), this, SLOT( slotNodeToBeInserted(  Node*, int ) ) );
        connect( m_project, SIGNAL( nodeToBeRemoved( Node* ) ), this, SLOT( slotNodeToBeRemoved( Node* ) ) );

        connect( m_project, SIGNAL( nodeToBeMoved( Node* ) ), this, SLOT( slotLayoutToBeChanged() ) );
        connect( m_project, SIGNAL( nodeMoved( Node* ) ), this, SLOT( slotLayoutChanged() ) );
    
        connect( m_project, SIGNAL( nodeAdded( Node* ) ), this, SLOT( slotNodeInserted( Node* ) ) );
        connect( m_project, SIGNAL( nodeRemoved( Node* ) ), this, SLOT( slotNodeRemoved( Node* ) ) );
        //connect( m_project, SIGNAL( nodeMoved( Node* ) ), this, SLOT( slotLayoutChanged() ) );
    }
    reset();
}

void NodeItemModel::setManager( ScheduleManager *sm )
{
    if ( m_nodemodel.manager() ) {
    }
    m_nodemodel.setManager( sm );
    if ( sm ) {
    }
    kDebug()<<this<<sm;
    reset();
}
    
Qt::ItemFlags NodeItemModel::flags( const QModelIndex &index ) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags( index );
    if ( !index.isValid() ) {
        if ( m_readWrite ) {
            flags |= Qt::ItemIsDropEnabled;
        }
        return flags;
    }
    if ( isColumnReadOnly( index.column() ) ) {
        //kDebug()<<"Column is readonly:"<<index.column();
        return flags;
    }
    Node *n = node( index );
    if ( m_readWrite && n != 0 ) {
        flags |= Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
        switch ( index.column() ) {
            case NodeModel::NodeName: // name
                flags |= Qt::ItemIsEditable;
                break;
            case NodeModel::NodeType: break; // Node type
            case NodeModel::NodeResponsible: // Responsible
                flags |= Qt::ItemIsEditable;
                break;
            case NodeModel::NodeAllocation: // allocation
                if ( n->type() == Node::Type_Task ) {
                    flags |= Qt::ItemIsEditable;
                }
                break;
            case NodeModel::NodeEstimateType: // estimateType
            case NodeModel::NodeEstimate: // estimate
            case NodeModel::NodeOptimisticRatio: // optimisticRatio
            case NodeModel::NodePessimisticRatio: // pessimisticRatio
            {
                if ( n->type() == Node::Type_Task || n->type() == Node::Type_Milestone ) {
                    flags |= Qt::ItemIsEditable;
                }
                break;
            }
            case NodeModel::NodeEstimateCalendar:
            {
                if ( n->type() == Node::Type_Task && n->estimate()->type() == Estimate::Type_Duration )
                {
                    flags |= Qt::ItemIsEditable;
                }
                break;
            }
            case NodeModel::NodeRisk: // risktype
            {
                if ( n->type() == Node::Type_Task ) {
                    flags |= Qt::ItemIsEditable;
                }
                break;
            }
            case NodeModel::NodeConstraint: // constraint type
                if ( n->type() == Node::Type_Task || n->type() == Node::Type_Milestone ) {
                    flags |= Qt::ItemIsEditable;
                }
                break;
            case NodeModel::NodeConstraintStart: { // constraint start
                if ( ! ( n->type() == Node::Type_Task || n->type() == Node::Type_Milestone ) ) {
                    break;
                }
                int c = n->constraint();
                if ( c == Node::MustStartOn || c == Node::StartNotEarlier || c == Node::FixedInterval ) {
                    flags |= Qt::ItemIsEditable;
                }
                break;
            }
            case NodeModel::NodeConstraintEnd: { // constraint end
                if ( ! ( n->type() == Node::Type_Task || n->type() == Node::Type_Milestone ) ) {
                    break;
                }
                int c = n->constraint();
                if ( c == Node::MustFinishOn || c == Node::FinishNotLater || c ==  Node::FixedInterval ) {
                    flags |= Qt::ItemIsEditable;
                }
                break;
            }
            case NodeModel::NodeRunningAccount: // running account
                if ( n->type() == Node::Type_Task ) {
                    flags |= Qt::ItemIsEditable;
                }
                break;
            case NodeModel::NodeStartupAccount: // startup account
            case NodeModel::NodeStartupCost: // startup cost
            case NodeModel::NodeShutdownAccount: // shutdown account
            case NodeModel::NodeShutdownCost: { // shutdown cost
                if ( n->type() == Node::Type_Task || n->type() == Node::Type_Milestone ) {
                    flags |= Qt::ItemIsEditable;
                }
                break;
            }
            case NodeModel::NodeDescription: // description
                break;
            default: 
                break;
        }
        Task *t = static_cast<Task*>( n );
        if ( manager() && t->isScheduled( id() ) ) {
            if ( ! t->completion().isStarted() ) {
                switch ( index.column() ) {
                    case NodeModel::NodeActualStart:
                        flags |= Qt::ItemIsEditable;
                        break;
                    case NodeModel::NodeActualFinish:
                        if ( t->type() == Node::Type_Milestone ) {
                            flags |= Qt::ItemIsEditable;
                        }
                        break;
                    case NodeModel::NodeCompleted:
                        if ( t->state() & Node::State_ReadyToStart ) {
                            flags |= Qt::ItemIsEditable;
                        }
                        break;

                    default: break;
                }
            } else if ( ! t->completion().isFinished() ) {
                switch ( index.column() ) {
                    case NodeModel::NodeActualFinish:
                    case NodeModel::NodeCompleted:
                    case NodeModel::NodeRemainingEffort:
                        flags |= Qt::ItemIsEditable;
                        break;
                    case NodeModel::NodeActualEffort:
                        if ( t->completion().entrymode() == Completion::EnterEffortPerTask || t->completion().entrymode() == Completion::EnterEffortPerResource ) {
                            flags |= Qt::ItemIsEditable;
                        }
                        break;
                    default: break;
                }
            }
        }
    }
    return flags;
}

    
QModelIndex NodeItemModel::parent( const QModelIndex &index ) const
{
    if ( !index.isValid() ) {
        return QModelIndex();
    }
    //kDebug()<<index.internalPointer()<<":"<<index.row()<<","<<index.column();
    Node *p = node( index )->parentNode();
    if ( p == 0 || p->type() == Node::Type_Project ) {
        return QModelIndex();
    }
    int row = p->parentNode()->indexOf( p );
    if ( row == -1 ) {
        return QModelIndex();
    }
    return createIndex( row, 0, p );
}

QModelIndex NodeItemModel::index( int row, int column, const QModelIndex &parent ) const
{
    if ( parent.isValid() ) {
        Q_ASSERT( parent.model() == this );
    }
    //kDebug()<<parent<<row<<column;
    if ( m_project == 0 || column < 0 || column >= columnCount() || row < 0 ) {
        //kDebug()<<m_project<<parent<<"No index for"<<row<<","<<column;
        return QModelIndex();
    }
    Node *p = node( parent );
    if ( row >= p->numChildren() ) {
        kError()<<p->name()<<" row too high"<<row<<","<<column;
        return QModelIndex();
    }
    // now get the internal pointer for the index
    Node *n = p->childNode( row );
    QModelIndex idx = createIndex(row, column, n);
    //kDebug()<<idx;
    return idx;
}

QModelIndex NodeItemModel::index( const Node *node ) const
{
    if ( m_project == 0 || node == 0 ) {
        return QModelIndex();
    }
    Node *par = node->parentNode();
    if ( par ) {
        //kDebug()<<par<<"-->"<<node;
        return createIndex( par->indexOf( node ), 0, const_cast<Node*>(node) );
    }
    //kDebug()<<node;
    return QModelIndex();
}

bool NodeItemModel::setName( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            if ( value.toString() == node->name() ) {
                return false;
            }
            emit executeCommand( new NodeModifyNameCmd( *node, value.toString(), "Modify task name" ) );
            return true;
    }
    return false;
}

bool NodeItemModel::setLeader( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            if ( value.toString() == node->leader() ) {
                return false;
            }
            emit executeCommand( new NodeModifyLeaderCmd( *node, value.toString(), "Modify task responsible" ) );
            return true;
    }
    return false;
}

bool NodeItemModel::setAllocation( Node *node, const QVariant &value, int role )
{
    Task *task = qobject_cast<Task*>( node );
    if ( task == 0 ) {
        return false;
    }
    switch ( role ) {
        case Qt::EditRole:
        {
            MacroCommand *cmd = 0;
            QStringList res = m_project->resourceNameList();
            QStringList req = node->requestNameList();
            QStringList alloc;
            foreach ( const QString &s, value.toString().split( QRegExp(" *, *"), QString::SkipEmptyParts ) ) {
                alloc << s.trimmed();
            }
            // first add all new resources (to "default" group)
            ResourceGroup *pargr = m_project->groupByName( i18n( "Resources" ) );
            foreach ( const QString &s, alloc ) {
                Resource *r = m_project->resourceByName( s.trimmed() );
                if ( r != 0 ) {
                    continue;
                }
                if ( cmd == 0 ) cmd = new MacroCommand( i18n( "Add Resource" ) );
                if ( pargr == 0 ) {
                    pargr = new ResourceGroup();
                    pargr->setName( i18n( "Resources" ) );
                    cmd->addCommand( new AddResourceGroupCmd( m_project, pargr ) );
                    //kDebug()<<"add group:"<<pargr->name();
                }
                r = new Resource();
                r->setName( s.trimmed() );
                cmd->addCommand( new AddResourceCmd( pargr, r ) );
                //kDebug()<<"add resource:"<<r->name();
                emit executeCommand( cmd );
                cmd = 0;
            }
            
            QString c = i18n( "Modify Resource Allocations" );
            // Handle deleted requests
            foreach ( const QString &s, req ) {
                // if a request is not in alloc, it must have been be removed by the user
                if ( alloc.indexOf( s ) == -1 ) {
                    // remove removed resource request
                    ResourceRequest *r = node->resourceRequest( s );
                    if ( r ) {
                        if ( cmd == 0 ) cmd = new MacroCommand( c );
                        //kDebug()<<"delete request:"<<r->resource()->name()<<" group:"<<r->parent()->group()->name();
                        cmd->addCommand( new RemoveResourceRequestCmd( r->parent(), r ) );
                    }
                }
            }
            // Handle new requests
            QMap<ResourceGroup*, ResourceGroupRequest*> groupmap;
            foreach ( const QString &s, alloc ) {
                // if an allocation is not in req, it must be added
                if ( req.indexOf( s ) == -1 ) {
                    ResourceGroup *pargr = 0;
                    Resource *r = m_project->resourceByName( s );
                    if ( r == 0 ) {
                        // Handle request to non exixting resource
                        pargr = m_project->groupByName( i18n( "Resources" ) );
                        if ( pargr == 0 ) {
                            pargr = new ResourceGroup();
                            pargr->setName( i18n( "Resources" ) );
                            cmd->addCommand( new AddResourceGroupCmd( m_project, pargr ) );
                            //kDebug()<<"add group:"<<pargr->name();
                        }
                        r = new Resource();
                        r->setName( s );
                        cmd->addCommand( new AddResourceCmd( pargr, r ) );
                        //kDebug()<<"add resource:"<<r->name();
                        emit executeCommand( cmd );
                        cmd = 0;
                    } else {
                        pargr = r->parentGroup();
                        //kDebug()<<"add '"<<r->name()<<"' to group:"<<pargr;
                    }
                    // add request
                    ResourceGroupRequest *g = node->resourceGroupRequest( pargr );
                    if ( g == 0 ) {
                        g = groupmap.value( pargr );
                    }
                    if ( g == 0 ) {
                        // create a group request
                        if ( cmd == 0 ) cmd = new MacroCommand( c );
                        g = new ResourceGroupRequest( pargr );
                        cmd->addCommand( new AddResourceGroupRequestCmd( *task, g ) );
                        groupmap.insert( pargr, g );
                        //kDebug()<<"add group request:"<<g;
                    }
                    if ( cmd == 0 ) cmd = new MacroCommand( c );
                    cmd->addCommand( new AddResourceRequestCmd( g, new ResourceRequest( r, r->units() ) ) );
                    //kDebug()<<"add request:"<<r->name()<<" group:"<<g;
                }
            }
            if ( cmd ) {
                emit executeCommand( cmd );
            }
            return true;
        }
    }
    return false;
}

bool NodeItemModel::setDescription( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            if ( value.toString() == node->description() ) {
                return false;
            }
            emit executeCommand( new NodeModifyDescriptionCmd( *node, value.toString(), "Modify task description" ) );
            return true;
    }
    return false;
}

bool NodeItemModel::setType( Node *, const QVariant &, int )
{
    return false;
}

bool NodeItemModel::setConstraint( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            Node::ConstraintType v = Node::ConstraintType( value.toInt() );
            //kDebug()<<v;
            if ( v == node->constraint() ) {
                return false;
            }
            emit executeCommand( new NodeModifyConstraintCmd( *node, v, "Modify constraint type" ) );
            return true;
    }
    return false;
}

bool NodeItemModel::setConstraintStartTime( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            if ( value.toDateTime() == node->constraintStartTime().dateTime() ) {
                return false;
            }
            emit executeCommand( new NodeModifyConstraintStartTimeCmd( *node, value.toDateTime(), "Modify constraint start time" ) );
            return true;
    }
    return false;
}

bool NodeItemModel::setConstraintEndTime( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            if ( value.toDateTime() == node->constraintEndTime().dateTime() ) {
                return false;
            }
            emit executeCommand( new NodeModifyConstraintEndTimeCmd( *node, value.toDateTime(), "Modify constraint end time" ) );
            return true;
    }
    return false;
}

bool NodeItemModel::setEstimateType( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            Estimate::Type v = Estimate::Type( value.toInt() );
            if ( v == node->estimate()->type() ) {
                return false;
            }
            emit executeCommand( new ModifyEstimateTypeCmd( *node, node->estimate()->type(), v, "Modify estimate type" ) );
            return true;
    }
    return false;
}

bool NodeItemModel::setEstimateCalendar( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            Calendar *c = 0;
            if ( value.toInt() > 0 ) {
                QStringList lst = m_nodemodel.estimateCalendar( node, Role::EnumList ).toStringList();
                if ( value.toInt() < lst.count() ) {
                    c = m_project->calendarByName( lst.at( value.toInt() ) );
                }
                Calendar *old = node->estimate()->calendar();
                if ( c != old ) {
                    emit executeCommand( new ModifyEstimateCalendarCmd( *node, old, c, "Modify estimate calendar" ) );
                    return true;
                }
            }
            return false;;
    }
    return false;
}

bool NodeItemModel::setEstimate( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            double d( value.toList()[0].toDouble() );
            Duration::Unit unit = static_cast<Duration::Unit>( value.toList()[1].toInt() );
            //kDebug()<<d<<","<<unit<<" ->"<<value.toList()[1].toInt();
            MacroCommand *cmd = 0;
            if ( d != node->estimate()->expectedEstimate() ) {
                if ( cmd == 0 ) cmd = new MacroCommand( i18n( "Modify estimate" ) );
                cmd->addCommand( new ModifyEstimateCmd( *node, node->estimate()->expectedEstimate(), d ) );
            }
            if ( unit != node->estimate()->unit() ) {
                if ( cmd == 0 ) cmd = new MacroCommand( i18n( "Modify estimate" ) );
                cmd->addCommand( new ModifyEstimateUnitCmd( *node, node->estimate()->unit(), unit ) );
            }
            if ( cmd == 0 ) {
                return false;
            }
            emit executeCommand( cmd );
            return true;
    }
    return false;
}

bool NodeItemModel::setOptimisticRatio( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            if ( value.toInt() == node->estimate()->optimisticRatio() ) {
                return false;
            }
            emit executeCommand( new EstimateModifyOptimisticRatioCmd( *node, node->estimate()->optimisticRatio(), value.toInt(), "Modify estimate" ) );
            return true;
    }
    return false;
}

bool NodeItemModel::setPessimisticRatio( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            if ( value.toInt() == node->estimate()->pessimisticRatio() ) {
                return false;
            }
            emit executeCommand( new EstimateModifyPessimisticRatioCmd( *node, node->estimate()->pessimisticRatio(), value.toInt(), "Modify estimate" ) );
            return true;
    }
    return false;
}

bool NodeItemModel::setRiskType( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            if ( value.toInt() == node->estimate()->risktype() ) {
                return false;
            }
            Estimate::Risktype v = Estimate::Risktype( value.toInt() );
            emit executeCommand( new EstimateModifyRiskCmd( *node, node->estimate()->risktype(), v, "Modify Risk Type" ) );
            return true;
    }
    return false;
}

bool NodeItemModel::setRunningAccount( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            //kDebug()<<node->name();
            QStringList lst = m_nodemodel.runningAccount( node, Role::EnumList ).toStringList();
            if ( value.toInt() >= lst.count() ) {
                return false;
            }
            Account *a = m_project->accounts().findAccount( lst.at( value.toInt() ) );
            Account *old = node->runningAccount();
            if ( old != a ) {
                emit executeCommand( new NodeModifyRunningAccountCmd( *node, old, a, i18n( "Modify Running Account" ) ) );
            }
            return true;
    }
    return false;
}

bool NodeItemModel::setStartupAccount( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            //kDebug()<<node->name();
            QStringList lst = m_nodemodel.startupAccount( node, Role::EnumList ).toStringList();
            if ( value.toInt() >= lst.count() ) {
                return false;
            }
            Account *a = m_project->accounts().findAccount( lst.at( value.toInt() ) );
            Account *old = node->startupAccount();
            //kDebug()<<(value.toInt())<<";"<<(lst.at( value.toInt()))<<":"<<a;
            if ( old != a ) {
                emit executeCommand( new NodeModifyStartupAccountCmd( *node, old, a, i18n( "Modify Startup Account" ) ) );
            }
            return true;
    }
    return false;
}

bool NodeItemModel::setStartupCost( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            double v = KGlobal::locale()->readMoney( value.toString() );
            if ( v == node->startupCost() ) {
                return false;
            }
            emit executeCommand( new NodeModifyStartupCostCmd( *node, v, i18n( "Modify Startup Cost" ) ) );
            return true;
    }
    return false;
}

bool NodeItemModel::setShutdownAccount( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            //kDebug()<<node->name();
            QStringList lst = m_nodemodel.shutdownAccount( node, Role::EnumList ).toStringList();
            if ( value.toInt() >= lst.count() ) {
                return false;
            }
            Account *a = m_project->accounts().findAccount( lst.at( value.toInt() ) );
            Account *old = node->shutdownAccount();
            if ( old != a ) {
                emit executeCommand( new NodeModifyShutdownAccountCmd( *node, old, a, i18n( "Modify Shutdown Account" ) ) );
            }
            return true;
    }
    return false;
}

bool NodeItemModel::setShutdownCost( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            double v = KGlobal::locale()->readMoney( value.toString() );
            if ( v == node->shutdownCost() ) {
                return false;
            }
            emit executeCommand( new NodeModifyShutdownCostCmd( *node, v, i18n( "Modify Shutdown Cost" ) ) );
            return true;
    }
    return false;
}

bool NodeItemModel::setCompletion( Node *node, const QVariant &value, int role )
{
    kDebug()<<node->name()<<value<<role;
    if ( role != Qt::EditRole ) {
        return false;
    }
    if ( node->type() == Node::Type_Task ) {
        Completion &c = static_cast<Task*>( node )->completion();
        QDateTime dt = QDateTime::currentDateTime();
        QDate date = dt.date();
        // xgettext: no-c-format
        MacroCommand *m = new MacroCommand( i18n( "Modify % Completed" ) );
        if ( ! c.isStarted() ) {
            m->addCommand( new ModifyCompletionStartedCmd( c, true ) );
            m->addCommand( new ModifyCompletionStartTimeCmd( c, dt ) );
        }
        m->addCommand( new ModifyCompletionPercentFinishedCmd( c, date, value.toInt() ) );
        if ( value.toInt() == 100 ) {
            m->addCommand( new ModifyCompletionFinishedCmd( c, true ) );
            m->addCommand( new ModifyCompletionFinishTimeCmd( c, dt ) );
        }
        emit executeCommand( m ); // also adds a new entry if necessary
        if ( c.entrymode() == Completion::EnterCompleted ) {
            Duration planned = static_cast<Task*>( node )->plannedEffort( m_nodemodel.id() );
            Duration actual = ( planned * value.toInt() ) / 100;
            kDebug()<<planned.toString()<<value.toInt()<<actual.toString();
            NamedCommand *cmd = new ModifyCompletionActualEffortCmd( c, date, actual );
            cmd->execute();
            m->addCommand( cmd );
            cmd = new ModifyCompletionRemainingEffortCmd( c, date, planned - actual  );
            cmd->execute();
            m->addCommand( cmd );
        }
        return true;
    }
    if ( node->type() == Node::Type_Milestone ) {
        Completion &c = static_cast<Task*>( node )->completion();
        if ( value.toInt() > 0 ) {
            QDateTime dt = QDateTime::currentDateTime();
            QDate date = dt.date();
            MacroCommand *m = new MacroCommand( i18n( "Set finished" ) );
            m->addCommand( new ModifyCompletionStartedCmd( c, true ) );
            m->addCommand( new ModifyCompletionStartTimeCmd( c, dt ) );
            m->addCommand( new ModifyCompletionFinishedCmd( c, true ) );
            m->addCommand( new ModifyCompletionFinishTimeCmd( c, dt ) );
            m->addCommand( new ModifyCompletionPercentFinishedCmd( c, date, 100 ) );
            emit executeCommand( m ); // also adds a new entry if necessary
            return true;
        }
        return false;
    }
    return false;
}

bool NodeItemModel::setRemainingEffort( Node *node, const QVariant &value, int role )
{
    if ( role == Qt::EditRole && node->type() == Node::Type_Task ) {
        Task *t = static_cast<Task*>( node );
        double d( value.toList()[0].toDouble() );
        Duration::Unit unit = static_cast<Duration::Unit>( value.toList()[1].toInt() );
        Duration dur( d, unit );
        emit executeCommand( new ModifyCompletionRemainingEffortCmd( t->completion(), QDate::currentDate(), dur, i18n( "Modify Remaining Effort" ) ) );
        return true;
    }
    return false;
}

bool NodeItemModel::setActualEffort( Node *node, const QVariant &value, int role )
{
    if ( role == Qt::EditRole && node->type() == Node::Type_Task ) {
        Task *t = static_cast<Task*>( node );
        double d( value.toList()[0].toDouble() );
        Duration::Unit unit = static_cast<Duration::Unit>( value.toList()[1].toInt() );
        Duration dur( d, unit );
        emit executeCommand( new ModifyCompletionActualEffortCmd( t->completion(), QDate::currentDate(), dur, i18n( "Modify Actual Effort" ) ) );
        return true;
    }
    return false;
}

bool NodeItemModel::setStartedTime( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole: {
            Task *t = qobject_cast<Task*>( node );
            if ( t == 0 ) {
                return false;
            }
            MacroCommand *m = new MacroCommand( i18n( "Modify actual start time" ) );
            if ( ! t->completion().isStarted() ) {
                m->addCommand( new ModifyCompletionStartedCmd( t->completion(), true ) );
            }
            m->addCommand( new ModifyCompletionStartTimeCmd( t->completion(), value.toDateTime() ) );
            if ( t->type() == Node::Type_Milestone ) {
                m->addCommand( new ModifyCompletionFinishedCmd( t->completion(), true ) );
                m->addCommand( new ModifyCompletionFinishTimeCmd( t->completion(), value.toDateTime() ) );
                if ( t->completion().percentFinished() < 100 ) {
                    Completion::Entry *e = new Completion::Entry( 100, Duration::zeroDuration, Duration::zeroDuration );
                    m->addCommand( new AddCompletionEntryCmd( t->completion(), value.toDate(), e ) );
                }
            }
            emit executeCommand( m );
            return true;
        }
    }
    return false;
}

bool NodeItemModel::setFinishedTime( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole: {
            Task *t = qobject_cast<Task*>( node );
            if ( t == 0 ) {
                return false;
            }
            MacroCommand *m = new MacroCommand( i18n( "Modify actual finish time" ) );
            if ( ! t->completion().isFinished() ) {
                m->addCommand( new ModifyCompletionFinishedCmd( t->completion(), true ) );
                if ( t->completion().percentFinished() < 100 ) {
                    Completion::Entry *e = new Completion::Entry( 100, Duration::zeroDuration, Duration::zeroDuration );
                    m->addCommand( new AddCompletionEntryCmd( t->completion(), value.toDate(), e ) );
                }
            }
            m->addCommand( new ModifyCompletionFinishTimeCmd( t->completion(), value.toDateTime() ) );
            if ( t->type() == Node::Type_Milestone ) {
                m->addCommand( new ModifyCompletionStartedCmd( t->completion(), true ) );
                m->addCommand( new ModifyCompletionStartTimeCmd( t->completion(), value.toDateTime() ) );
            }
            emit executeCommand( m );
            return true;
        }
    }
    return false;
}

QVariant NodeItemModel::data( const QModelIndex &index, int role ) const
{
    QVariant result;
    Node *n = node( index );
    if ( n != 0 ) {
        result = m_nodemodel.data( n, index.column(), role );
        //kDebug()<<n->name()<<": "<<index.column()<<", "<<role<<result;
    }
    if ( role == Qt::EditRole ) {
        switch ( index.column() ) {
            case NodeModel::NodeActualStart:
            case NodeModel::NodeActualFinish:
                if ( ! result.isValid() ) {
                    return QDateTime::currentDateTime();
                }
            break;
        }
    }
    if ( role == Qt::DisplayRole && ! result.isValid() ) {
        result = " "; // HACK to show focus in empty cells
    }
    return result;
}

bool NodeItemModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
    if ( ! index.isValid() ) {
        return ItemModelBase::setData( index, value, role );
    }
    if ( ( flags(index) &Qt::ItemIsEditable ) == 0 || role != Qt::EditRole ) {
        kWarning()<<index<<value<<role;
        return false;
    }
    Node *n = node( index );
    switch (index.column()) {
        case NodeModel::NodeName: return setName( n, value, role );
        case NodeModel::NodeType: return setType( n, value, role );
        case NodeModel::NodeResponsible: return setLeader( n, value, role );
        case NodeModel::NodeAllocation: return setAllocation( n, value, role );
        case NodeModel::NodeEstimateType: return setEstimateType( n, value, role );
        case NodeModel::NodeEstimateCalendar: return setEstimateCalendar( n, value, role );
        case NodeModel::NodeEstimate: return setEstimate( n, value, role );
        case NodeModel::NodeOptimisticRatio: return setOptimisticRatio( n, value, role );
        case NodeModel::NodePessimisticRatio: return setPessimisticRatio( n, value, role );
        case NodeModel::NodeRisk: return setRiskType( n, value, role );
        case NodeModel::NodeConstraint: return setConstraint( n, value, role );
        case NodeModel::NodeConstraintStart: return setConstraintStartTime( n, value, role );
        case NodeModel::NodeConstraintEnd: return setConstraintEndTime( n, value, role );
        case NodeModel::NodeRunningAccount: return setRunningAccount( n, value, role );
        case NodeModel::NodeStartupAccount: return setStartupAccount( n, value, role );
        case NodeModel::NodeStartupCost: return setStartupCost( n, value, role );
        case NodeModel::NodeShutdownAccount: return setShutdownAccount( n, value, role );
        case NodeModel::NodeShutdownCost: return setShutdownCost( n, value, role );
        case NodeModel::NodeDescription: return setDescription( n, value, role );
        case NodeModel::NodeCompleted: return setCompletion( n, value, role );
        case NodeModel::NodeActualEffort: return setActualEffort( n, value, role );
        case NodeModel::NodeRemainingEffort: return setRemainingEffort( n, value, role );
        case NodeModel::NodeActualStart: return setStartedTime( n, value, role );
        case NodeModel::NodeActualFinish: return setFinishedTime( n, value, role );
        default:
            qWarning("data: invalid display value column %d", index.column());
            return false;
    }
    return false;
}

QVariant NodeItemModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal ) {
        if ( role == Qt::DisplayRole ) {
            return m_nodemodel.headerData( section, role );
        } else if ( role == Qt::TextAlignmentRole ) {
            switch (section) {
                case NodeModel::NodeName: return Qt::AlignLeft;
                case NodeModel::NodeType: return Qt::AlignCenter;
                default: return QVariant();
            }
        }
    }
    if ( role == Qt::ToolTipRole ) {
        return NodeModel::headerData( section, role );
    }
    return ItemModelBase::headerData(section, orientation, role);
}

QAbstractItemDelegate *NodeItemModel::createDelegate( int column, QWidget *parent ) const
{
    switch ( column ) {
        //case NodeModel::NodeAllocation: return new ??Delegate( parent );
        case NodeModel::NodeEstimateType: return new EnumDelegate( parent );
        case NodeModel::NodeEstimateCalendar: return new EnumDelegate( parent );
        case NodeModel::NodeEstimate: return new DurationSpinBoxDelegate( parent );
        case NodeModel::NodeOptimisticRatio: return new SpinBoxDelegate( parent );
        case NodeModel::NodePessimisticRatio: return new SpinBoxDelegate( parent );
        case NodeModel::NodeRisk: return new EnumDelegate( parent );
        case NodeModel::NodeConstraint: return new EnumDelegate( parent );
        case NodeModel::NodeRunningAccount: return new EnumDelegate( parent );
        case NodeModel::NodeStartupAccount: return new EnumDelegate( parent );
        case NodeModel::NodeStartupCost: return new MoneyDelegate( parent );
        case NodeModel::NodeShutdownAccount: return new EnumDelegate( parent );
        case NodeModel::NodeShutdownCost: return new MoneyDelegate( parent );

        case NodeModel::NodeCompleted: return new TaskCompleteDelegate( parent );
        case NodeModel::NodeRemainingEffort: return new DurationSpinBoxDelegate( parent );
        case NodeModel::NodeActualEffort: return new DurationSpinBoxDelegate( parent );

        default: return 0;
    }
    return 0;
}

int NodeItemModel::columnCount( const QModelIndex &/*parent*/ ) const
{
    return m_nodemodel.propertyCount();
}

int NodeItemModel::rowCount( const QModelIndex &parent ) const
{
    Node *p = node( parent );
    return p == 0 ? 0 : p->numChildren();
}

Qt::DropActions NodeItemModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}


QStringList NodeItemModel::mimeTypes() const
{
    return QStringList() << "application/x-vnd.kde.kplato.nodeitemmodel.internal";
}

QMimeData *NodeItemModel::mimeData( const QModelIndexList & indexes ) const
{
    QMimeData *m = new QMimeData();
    QByteArray encodedData;
    QDataStream stream(&encodedData, QIODevice::WriteOnly);
    QList<int> rows;
    foreach (const QModelIndex &index, indexes) {
        if ( index.isValid() && !rows.contains( index.row() ) ) {
            //kDebug()<<index.row();
            Node *n = node( index );
            if ( n ) {
                rows << index.row();
                stream << n->id();
            }
        }
    }
    m->setData("application/x-vnd.kde.kplato.nodeitemmodel.internal", encodedData);
    return m;
}

bool NodeItemModel::dropAllowed( const QModelIndex &index, int dropIndicatorPosition, const QMimeData *data )
{
    //kDebug();
    Node *dn = node( index );
    if ( dn == 0 ) {
        kError()<<"no node to drop on!";
        return false; // hmmm
    }
    switch ( dropIndicatorPosition ) {
        case ItemModelBase::AboveItem:
        case ItemModelBase::BelowItem:
            // dn == sibling
            return dropAllowed( dn->parentNode(), data );
        case ItemModelBase::OnItem:
            // dn == new parent
            return dropAllowed( dn, data );
        default:
            break;
    }
    return false;
}

bool NodeItemModel::dropAllowed( Node *on, const QMimeData *data )
{
    if ( !data->hasFormat("application/x-vnd.kde.kplato.nodeitemmodel.internal") ) {
        return false;
    }
    if ( on == m_project ) {
        return true;
    }
    QByteArray encodedData = data->data( "application/x-vnd.kde.kplato.nodeitemmodel.internal" );
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    QList<Node*> lst = nodeList( stream );
    foreach ( Node *n, lst ) {
        if ( on == n || on->isChildOf( n ) ) {
            return false;
        }
    }
    lst = removeChildNodes( lst );
    foreach ( Node *n, lst ) {
        if ( ! m_project->canMoveTask( n, on ) ) {
            return false;
        }
    }
    return true;
}

QList<Node*> NodeItemModel::nodeList( QDataStream &stream )
{
    QList<Node*> lst;
    while (!stream.atEnd()) {
        QString id;
        stream >> id;
        Node *node = m_project->findNode( id );
        if ( node ) {
            lst << node;
        }
    }
    return lst;
}

QList<Node*> NodeItemModel::removeChildNodes( QList<Node*> nodes )
{
    QList<Node*> lst;
    foreach ( Node *node, nodes ) {
        bool ins = true;
        foreach ( Node *n, lst ) {
            if ( node->isChildOf( n ) ) {
                //kDebug()<<node->name()<<" is child of"<<n->name();
                ins = false;
                break;
            }
        }
        if ( ins ) {
            //kDebug()<<" insert"<<node->name();
            lst << node;
        }
    }
    QList<Node*> nl = lst;
    QList<Node*> nlst = lst;
    foreach ( Node *node, nl ) {
        foreach ( Node *n, nlst ) {
            if ( n->isChildOf( node ) ) {
                //kDebug()<<n->name()<<" is child of"<<node->name();
                int i = nodes.indexOf( n );
                lst.removeAt( i );
            }
        }
    }
    return lst;
}

bool NodeItemModel::dropMimeData( const QMimeData *data, Qt::DropAction action, int row, int /*column*/, const QModelIndex &parent )
{
    //kDebug()<<action;
    if (action == Qt::IgnoreAction) {
        return true;
    }
    if ( !data->hasFormat( "application/x-vnd.kde.kplato.nodeitemmodel.internal" ) ) {
        return false;
    }
    if ( action == Qt::MoveAction ) {
        //kDebug()<<"MoveAction";
        
        QByteArray encodedData = data->data( "application/x-vnd.kde.kplato.nodeitemmodel.internal" );
        QDataStream stream(&encodedData, QIODevice::ReadOnly);
        Node *par = 0;
        if ( parent.isValid() ) {
            par = node( parent );
        } else {
            par = m_project;
        }
        QList<Node*> lst = nodeList( stream );
        QList<Node*> nodes = removeChildNodes( lst ); // children goes with their parent
        foreach ( Node *n, nodes ) {
            if ( ! m_project->canMoveTask( n, par ) ) {
                //kDebug()<<"Can't move task:"<<n->name();
                return false;
            }
        }
        int offset = 0;
        MacroCommand *cmd = 0;
        foreach ( Node *n, nodes ) {
            if ( cmd == 0 ) cmd = new MacroCommand( i18n( "Move tasks" ) );
            cmd->addCommand( new NodeMoveCmd( m_project, n, par, row + offset ) );
            offset++;
        }
        if ( cmd ) {
            emit executeCommand( cmd );
        }
        //kDebug()<<row<<","<<column<<" parent="<<parent.row()<<","<<parent.column()<<":"<<par->name();
        return true;
    }
    return false;
}

Node *NodeItemModel::node( const QModelIndex &index ) const
{
    Node *n = m_project;
    if ( index.isValid() ) {
        if ( index.internalPointer() == 0 ) {
            
        }
        //kDebug()<<index.internalPointer()<<":"<<index.row()<<","<<index.column();
        n = static_cast<Node*>( index.internalPointer() );
        Q_ASSERT( n );
    }
    return n;
}

void NodeItemModel::slotNodeChanged( Node *node )
{
    if ( node == 0 || node->type() == Node::Type_Project ) {
        return;
    }
    int row = node->parentNode()->findChildNode( node );
    kDebug()<<node->name()<<row;
    emit dataChanged( createIndex( row, 0, node ), createIndex( row, columnCount()-1, node ) );
}

QModelIndex NodeItemModel::insertTask( Node *node, Node *after )
{
    emit executeCommand( new TaskAddCmd( m_project, node, after, i18n( "Add Task") ) );
    int row = -1;
    if ( node->parentNode() ) {
        row = node->parentNode()->indexOf( node );
    }
    if ( row != -1 ) {
        //kDebug()<<"Inserted: "<<node->name()<<"; "<<row;
        return createIndex( row, 0, node );
    }
    //kDebug()<<"Can't find "<<node->name();
    return QModelIndex();
}

QModelIndex NodeItemModel::insertSubtask( Node *node, Node *parent )
{
    emit executeCommand( new SubtaskAddCmd( m_project, node, parent, i18n( "Add Subtask" ) ) );
    reset();
    int row = -1;
    if ( node->parentNode() ) {
        row = node->parentNode()->indexOf( node );
    }
    if ( row != -1 ) {
        //kDebug()<<node->parentNode()<<" inserted: "<<node->name()<<"; "<<row;
        return createIndex( row, 0, node );
    }
    //kDebug()<<"Can't find "<<node->name();
    return QModelIndex();
}


//----------------------------
class GeneralNodeItemModel::Object
{
public:
    enum Type { Type_Node, Type_WorkPackage };
    Object( Node *n, Type typ = Type_Node, int r = -1 )
    : node( n ), type( typ ), row( r )
    {}

    bool isNode() const { return type == Type_Node; }
    bool isWorkPackage() const { return type == Type_WorkPackage; }

    Node *node;
    int type;
    int row; // if isWorkPackage()
};

//----------------------------
GeneralNodeItemModel::GeneralNodeItemModel( QObject *parent )
    : NodeItemModel( parent ),
    m_modus( 0 )
{
}

GeneralNodeItemModel::~GeneralNodeItemModel()
{
}

void GeneralNodeItemModel::setModus( int modus )
{
    if ( m_modus & WorkPackage ) {
        foreach ( Object *o, nodeObjects() ) {
            disconnect( o->node, SIGNAL( workPackageToBeAdded( Node*, int ) ), this, SLOT( slotWorkPackageToBeAdded( Node*, int ) ) );
            disconnect( o->node, SIGNAL( workPackageAdded( Node* ) ), this, SLOT( slotWorkPackageAdded( Node* ) ) );
        }
    }
    qDeleteAll( m_objects );
    m_objects.clear();
    m_modus = modus;
    if ( m_project == 0 ) {
        return;
    }
    foreach ( Node *n, m_project->allNodes() ) {
        m_objects << new Object( n );
        if ( m_modus & WorkPackage ) {
            connect( n, SIGNAL( workPackageToBeAdded( Node*, int ) ), SLOT( slotWorkPackageToBeAdded( Node*, int ) ) );
            connect( n, SIGNAL( workPackageAdded( Node* ) ), SLOT( slotWorkPackageAdded( Node* ) ) );
            for ( int i = 0; i < static_cast<Task*>( n )->workPackageLogCount(); ++i ) {
                m_objects << new Object( n, Object::Type_WorkPackage, i );
            }
        }
    }
}

void GeneralNodeItemModel::setProject( Project *project )
{
    NodeItemModel::setProject( project );
    setModus( m_modus );
    reset();
}

void GeneralNodeItemModel::slotNodeToBeInserted( Node *parent, int row )
{
    if ( m_modus == 0 ) {
        return NodeItemModel::slotNodeToBeInserted( parent, row );
    }
    if ( m_modus & Flat ) {
        int pos = nodeObjects().count();
        beginInsertRows( QModelIndex(), pos, pos );
        return;
    }
    beginInsertRows( index( parent ), row, row );
}

void GeneralNodeItemModel::slotNodeInserted( Node *node )
{
    if ( m_modus == 0 ) {
        return NodeItemModel::slotNodeInserted( node );
    }
    m_objects << new Object( node );
    connect( node, SIGNAL( workPackageToBeAdded( Node*, int ) ), SLOT( slotWorkPackageToBeAdded( Node*, int ) ) );
    connect( node, SIGNAL( workPackageAdded( Node* ) ), SLOT( slotWorkPackageAdded( Node* ) ) );

    endInsertRows();
}

void GeneralNodeItemModel::slotNodeToBeRemoved( Node *node )
{
    if ( m_modus == 0 ) {
        return NodeItemModel::slotNodeToBeRemoved( node );
    }
    Object *obj = findNodeObject( node );
    int row = m_objects.indexOf( obj );
    if ( row >= 0 ) {
        if ( m_modus & WorkPackage ) {
            disconnect( node, SIGNAL( workPackageToBeAdded( Node*, int ) ), this, SLOT( slotWorkPackageToBeAdded( Node*, int ) ) );
            disconnect( node, SIGNAL( workPackageAdded( Node* ) ), this, SLOT( slotWorkPackageAdded( Node* ) ) );
        }
        QModelIndex idx = index( node );
        beginRemoveRows( parent( idx ), idx.row(), idx.row() );
        m_objects.removeAt( row );
        QList<int> rows = workPackagePositions( obj );
        while ( ! rows.isEmpty() ) {
            delete m_objects.takeAt( rows.takeLast() );
        }
        delete obj;
        endRemoveRows();
    }
}

void GeneralNodeItemModel::slotWorkPackageToBeAdded( Node *node, int row )
{
    qDebug()<<"slotWorkPackageToBeAdded:"<<index( node )<<node->name()<<row;
    beginInsertRows( index( node ), row, row );
    m_objects << new Object( node, Object::Type_WorkPackage, row );
}

void GeneralNodeItemModel::slotWorkPackageAdded( Node *node )
{
    qDebug()<<"slotWorkPackageAdded:"<<node->name();
    endInsertRows();
    //HACK to get both views updated
    emit layoutAboutToBeChanged();
    emit layoutChanged();
}

void GeneralNodeItemModel::slotNodeRemoved( Node *node )
{
    if ( m_modus == 0 ) {
        return NodeItemModel::slotNodeRemoved( node );
    }
    // Do nothing!!
}

void GeneralNodeItemModel::slotWbsDefinitionChanged()
{
    if ( m_modus == 0 ) {
        return NodeItemModel::slotWbsDefinitionChanged();
    }
    //TODO
}

Qt::ItemFlags GeneralNodeItemModel::flags( const QModelIndex &index ) const
{
    if ( m_modus == 0 ) {
        return NodeItemModel::flags( index );
    }
    return QAbstractItemModel::flags( index ); //TODO
}

    
QModelIndex GeneralNodeItemModel::parent( const QModelIndex &index ) const
{
    if ( m_modus == 0 ) {
        return NodeItemModel::parent( index );
    }
    Object *obj = static_cast<Object*>( index.internalPointer() );
    if ( obj == 0 ) {
        return QModelIndex();
    }
    if ( obj->isWorkPackage() ) {
        return this->index( obj->node );
    }
    if ( m_modus & WBS ) {
        if ( obj->isNode() ) {
            return this->index( obj->node->parentNode() );
        }
    }
    return QModelIndex();
}

QModelIndex GeneralNodeItemModel::index( int row, int column, const QModelIndex &parent ) const
{
    if ( m_modus == 0 ) {
        return NodeItemModel::index( row, column, parent );
    }
    Object *par = static_cast<Object*>( parent.internalPointer() );
    if ( m_modus & WBS ) {
        if ( ! parent.isValid() ) {
            return createIndex( row, column, findNodeObject( m_project->childNode( row ) ) );
        }
        if ( par && par->isNode() ) {
            if ( par->node->type() == Node::Type_Summarytask ) {
                return createIndex( row, column, findNodeObject( par->node->childNode( row ) ) );
            }
            return createIndex( row, column, findWPObject( row, par ) );
        }
        return QModelIndex();
    }
    if ( m_modus & Flat  ) {
        if ( ! parent.isValid() ) {
            return createIndex( row, column, nodeObjects().value( row ) );
        }
        if ( par && par->isNode() ) {
            return createIndex( row, column, findWPObject( row, par ) );
        }
        return QModelIndex();
    }
    return QModelIndex();
}

QModelIndex GeneralNodeItemModel::index( const Node *node ) const
{
    if ( m_modus == 0 ) {
        return NodeItemModel::index( node );
    }
    Object *obj = findNodeObject( node );
    if ( obj ) {
        return createIndex( nodeObjects().indexOf( obj ), 0, obj );
    }
    return QModelIndex();
}

QVariant GeneralNodeItemModel::data( const QModelIndex &index, int role ) const
{
    if ( m_modus == 0 ) {
        return NodeItemModel::data( index, role );
    }
    QVariant result;
    Object *obj = static_cast<Object*>( index.internalPointer() );
    if ( obj && obj->isNode() ) {
        return m_nodemodel.data( obj->node, index.column(), role );
    }
    if ( obj && obj->isWorkPackage() ) {
        int col = -1;
        // map NodeModel columns to WorkPackageModel columns
        switch ( index.column() ) {
            case NodeModel::NodeName:
            case NodeModel::WPOwnerName:
                col = WorkPackageModel::WPOwnerName; break;
            case NodeModel::NodeStatus:
            case NodeModel::WPTransmitionStatus:
                col = WorkPackageModel::WPTransmitionStatus; break;
                col = WorkPackageModel::WPTransmitionStatus; break;
            case NodeModel::WPTransmitionTime:
                col = WorkPackageModel:: WPTransmitionTime; break;
            default: break;
        }
        if ( col >= 0 ) {
            return m_wpmodel.data( static_cast<const Task*>( obj->node )->workPackageAt( index.row() ), col, role );
        }
    }
    return result;
}

bool GeneralNodeItemModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
    if ( m_modus == 0 ) {
        return NodeItemModel::setData( index, value, role );
    }
    return false;
}

QAbstractItemDelegate *GeneralNodeItemModel::createDelegate( int column, QWidget *parent ) const
{
    if ( m_modus == 0 ) {
        return NodeItemModel::createDelegate( column, parent );
    }
    return 0; //TODO
}

int GeneralNodeItemModel::rowCount( const QModelIndex &parent ) const
{
    if ( m_modus == 0 ) {
        return NodeItemModel::rowCount( parent );
    }
    Object *par = static_cast<Object*>( parent.internalPointer() );
    if ( m_modus & WBS ) {
        if ( ! parent.isValid() ) {
            return m_project->numChildren();
        }
        if ( par && par->isNode() && par->node->type() == Node::Type_Summarytask ) {
            return par->node->numChildren();
        }
    }
    if ( m_modus & Flat ) {
        if ( ! parent.isValid() ) {
            return nodeObjects().count();
        }
    }
    if ( ( m_modus & WorkPackage ) && par && par->isNode() ) {
        //qDebug()<<"rowCount:"<<par->node->name()<<static_cast<const Task*>( par->node )->workPackageLogCount();
        return static_cast<const Task*>( par->node )->workPackageLogCount();
    }
    return 0;
}

Node *GeneralNodeItemModel::node( const QModelIndex &index ) const
{
    if ( m_modus == 0 ) {
        return NodeItemModel::node( index );
    }
    Object *obj = static_cast<Object*>( index.internalPointer() );
    if ( obj && obj->isNode() ) {
        return obj->node;
    }
    return 0;
}

void GeneralNodeItemModel::slotNodeChanged( Node *node )
{
    if ( m_modus == 0 ) {
        return NodeItemModel::slotNodeChanged( node );
    }
    Object *obj = findNodeObject( node );
    if ( obj && obj->isNode() ) {
        QModelIndex i = index( node );
        emit dataChanged( i, createIndex( i.row(), columnCount()-1, obj ) );
    }
}

GeneralNodeItemModel::Object *GeneralNodeItemModel::findNodeObject( const Node *node ) const
{
    foreach ( Object *o, m_objects ) {
        if ( o->node == node && o->isNode() ) {
            return o;
        }
    }
    return 0;
}

GeneralNodeItemModel::Object *GeneralNodeItemModel::findWPObject( int row, GeneralNodeItemModel::Object *parent ) const
{
    foreach ( Object *o, m_objects ) {
        if ( o->isWorkPackage() && o->node == parent->node && o->row == row ) {
            return o;
        }
    }
    return 0;
}

QList<GeneralNodeItemModel::Object*> GeneralNodeItemModel::nodeObjects() const
{
    QList<Object*> lst;
    foreach ( Object *o, m_objects ) {
        if ( o->isNode() ) {
            lst << o;
        }
    }
    return lst;
}

QList<int> GeneralNodeItemModel::workPackagePositions( GeneralNodeItemModel::Object *parent ) const
{
    QList<int> rows;
    for ( int i = 0; i < m_objects.count(); ++i ) {
        Object *o = m_objects.at( i );
        if ( o->node == parent->node && o->isWorkPackage() ) {
            rows << i;
        }
    }
    return rows;
}

//------------------------------------------------
GanttItemModel::GanttItemModel( QObject *parent )
    : NodeItemModel( parent ),
    m_showSpecial( false )
{
}

GanttItemModel::~GanttItemModel()
{
    QList<void*> lst = parentmap.values();
    while ( ! lst.isEmpty() )
        delete (int*)(lst.takeFirst());
}

int GanttItemModel::rowCount( const QModelIndex &parent ) const
{
    if ( m_showSpecial ) {
        if ( parentmap.values().contains( parent.internalPointer() ) ) {
            return 0;
        }
        Node *n = node( parent );
        if ( n && n->type() == Node::Type_Task ) {
            return 5; // the task + early start + late finish ++
        }
    }
    return NodeItemModel::rowCount( parent );
}

QModelIndex GanttItemModel::index( int row, int column, const QModelIndex &parent ) const
{
    if ( m_showSpecial && parent.isValid()  ) {
        Node *p = node( parent );
        if ( p->type() == Node::Type_Task ) {
            void *v = 0;
            foreach ( void *i, parentmap.values( p ) ) {
                if ( *( (int*)( i ) ) == row ) {
                    v = i;
                    break;
                }
            }
            if ( v == 0 ) {
                v = new int( row );
                const_cast<GanttItemModel*>( this )->parentmap.insertMulti( p, v );
            }
            return createIndex( row, column, v );
        }
    }
    return NodeItemModel::index( row, column, parent );
}

QModelIndex GanttItemModel::parent( const QModelIndex &idx ) const
{
    if ( m_showSpecial ) {
        QList<Node*> lst = parentmap.keys( idx.internalPointer() );
        if ( ! lst.isEmpty() ) {
            Q_ASSERT( lst.count() == 1 );
            return index( lst.first() );
        }
    }
    return NodeItemModel::parent( idx );
}

QVariant GanttItemModel::data( const QModelIndex &index, int role ) const
{
    if ( ! index.isValid() ) {
        return QVariant();
    }
    QModelIndex idx = index;
    QList<Node*> lst;
    if ( m_showSpecial ) {
        lst = parentmap.keys( idx.internalPointer() );
    }
    if ( ! lst.isEmpty() ) {
        Q_ASSERT( lst.count() == 1 );
        int row = *((int*)(idx.internalPointer()));
        Node *n = lst.first();
        if ( role == SpecialItemTypeRole ) {
            return row; // 0=task, 1=early start, 2=late finish...
        }
        switch ( row ) {
            case 0:  // the task
                if ( idx.column() == NodeModel::NodeType && role == KDGantt::ItemTypeRole ) {
                    switch ( n->type() ) {
                        case Node::Type_Task: return KDGantt::TypeTask;
                        default: break;
                    }
                }
                break;
            case 1: { // early start
                if ( role != Qt::DisplayRole && role != Qt::EditRole && role != KDGantt::ItemTypeRole ) {
                    return QVariant();
                }
                switch ( idx.column() ) {
                    case NodeModel::NodeName: return "Early Start";
                    case NodeModel::NodeType: return KDGantt::TypeEvent;
                    case NodeModel::NodeStartTime:
                    case NodeModel::NodeEndTime: return n->earlyStart( id() ).dateTime();
                    default: break;
                }
            }
            case 2: { // late finish
                if ( role != Qt::DisplayRole && role != Qt::EditRole && role != KDGantt::ItemTypeRole ) {
                    return QVariant();
                }
                switch ( idx.column() ) {
                    case NodeModel::NodeName: return "Late Finish";
                    case NodeModel::NodeType: return KDGantt::TypeEvent;
                    case NodeModel::NodeStartTime:
                    case NodeModel::NodeEndTime: return n->lateFinish( id() ).dateTime();
                    default: break;
                }
            }
            case 3: { // late start
                if ( role != Qt::DisplayRole && role != Qt::EditRole && role != KDGantt::ItemTypeRole ) {
                    return QVariant();
                }
                switch ( idx.column() ) {
                    case NodeModel::NodeName: return "Late Start";
                    case NodeModel::NodeType: return KDGantt::TypeEvent;
                    case NodeModel::NodeStartTime:
                    case NodeModel::NodeEndTime: return n->lateStart( id() ).dateTime();
                    default: break;
                }
            }
            case 4: { // early finish
                if ( role != Qt::DisplayRole && role != Qt::EditRole && role != KDGantt::ItemTypeRole ) {
                    return QVariant();
                }
                switch ( idx.column() ) {
                    case NodeModel::NodeName: return "Early Finish";
                    case NodeModel::NodeType: return KDGantt::TypeEvent;
                    case NodeModel::NodeStartTime:
                    case NodeModel::NodeEndTime: return n->earlyFinish( id() ).dateTime();
                    default: break;
                }
            }
            default: return QVariant();
        }
        idx = createIndex( idx.row(), idx.column(), n );
    } else {
        if ( role == SpecialItemTypeRole ) {
            return 0; // task of some type
        }
        if ( idx.column() == NodeModel::NodeType && role == KDGantt::ItemTypeRole ) {
            QVariant result = NodeItemModel::data( idx, Qt::EditRole );
            switch ( result.toInt() ) {
                case Node::Type_Summarytask: return KDGantt::TypeSummary;
                case Node::Type_Milestone: return KDGantt::TypeEvent;
                default: return m_showSpecial ? KDGantt::TypeMulti : KDGantt::TypeTask;
            }
        }
    }
    return NodeItemModel::data( idx, role );
}

//----------------------------
MilestoneItemModel::MilestoneItemModel( QObject *parent )
    : ItemModelBase( parent )
{
}

MilestoneItemModel::~MilestoneItemModel()
{
}

QList<Node*> MilestoneItemModel::mileStones() const
{
    QList<Node*> lst;
    foreach( Node* n, m_nodemap ) {
        if ( n->type() == Node::Type_Milestone ) {
            lst << n;
        }
    }
    return lst;
}

void MilestoneItemModel::slotNodeToBeInserted( Node *parent, int row )
{
}

void MilestoneItemModel::slotNodeInserted( Node *node )
{
    resetModel();
}

void MilestoneItemModel::slotNodeToBeRemoved( Node *node )
{
    //kDebug()<<node->name();
/*    int row = m_nodemap.values().indexOf( node );
    if ( row != -1 ) {
        Q_ASSERT( m_nodemap.contains( node->wbsCode() ) );
        Q_ASSERT( m_nodemap.keys().indexOf( node->wbsCode() ) == row );
        beginRemoveRows( QModelIndex(), row, row );
        m_nodemap.remove( node->wbsCode() );
        endRemoveRows();
    }*/
}

void MilestoneItemModel::slotNodeRemoved( Node *node )
{
    resetModel();
    //endRemoveRows();
}

void MilestoneItemModel::slotLayoutChanged()
{
    //kDebug()<<node->name();
    emit layoutAboutToBeChanged();
    emit layoutChanged();
}

void MilestoneItemModel::setProject( Project *project )
{
    if ( m_project ) {
        disconnect( m_project, SIGNAL( localeChanged() ), this, SLOT( slotLayoutChanged() ) );
        disconnect( m_project, SIGNAL( wbsDefinitionChanged() ), this, SLOT( slotWbsDefinitionChanged() ) );
        disconnect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotNodeChanged( Node* ) ) );
        disconnect( m_project, SIGNAL( nodeToBeAdded( Node*, int ) ), this, SLOT(  slotNodeToBeInserted( Node *, int ) ) );
        disconnect( m_project, SIGNAL( nodeToBeRemoved( Node* ) ), this, SLOT( slotNodeToBeRemoved( Node* ) ) );

        disconnect( m_project, SIGNAL( nodeToBeMoved( Node* ) ), this, SLOT( slotLayoutToBeChanged() ) );
        disconnect( m_project, SIGNAL( nodeMoved( Node* ) ), this, SLOT( slotLayoutChanged() ) );

        disconnect( m_project, SIGNAL( nodeAdded( Node* ) ), this, SLOT( slotNodeInserted( Node* ) ) );
        disconnect( m_project, SIGNAL( nodeRemoved( Node* ) ), this, SLOT( slotNodeRemoved( Node* ) ) );
    }
    m_project = project;
    //kDebug()<<m_project<<"->"<<project;
    m_nodemodel.setProject( project );
    if ( project ) {
        connect( m_project, SIGNAL( localeChanged() ), this, SLOT( slotLayoutChanged() ) );
        connect( m_project, SIGNAL( wbsDefinitionChanged() ), this, SLOT( slotWbsDefinitionChanged() ) );
        connect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotNodeChanged( Node* ) ) );
        connect( m_project, SIGNAL( nodeToBeAdded( Node*, int ) ), this, SLOT( slotNodeToBeInserted( Node *, int ) ) );
        connect( m_project, SIGNAL( nodeToBeRemoved( Node* ) ), this, SLOT( slotNodeToBeRemoved( Node* ) ) );

        connect( m_project, SIGNAL( nodeToBeMoved( Node* ) ), this, SLOT( slotLayoutToBeChanged() ) );
        connect( m_project, SIGNAL( nodeMoved( Node* ) ), this, SLOT( slotLayoutChanged() ) );

        connect( m_project, SIGNAL( nodeAdded( Node* ) ), this, SLOT( slotNodeInserted( Node* ) ) );
        connect( m_project, SIGNAL( nodeRemoved( Node* ) ), this, SLOT( slotNodeRemoved( Node* ) ) );
    }
    resetModel();
}

void MilestoneItemModel::setManager( ScheduleManager *sm )
{
    if ( m_nodemodel.manager() ) {
    }
    m_nodemodel.setManager( sm );
    if ( sm ) {
    }
    //kDebug()<<sm;
    resetModel();
}
    
bool MilestoneItemModel::resetData()
{
    int cnt = m_nodemap.count();
    m_nodemap.clear();
    if ( m_project != 0 ) {
        foreach ( Node *n, m_project->allNodes() ) {
            m_nodemap.insert( n->wbsCode(), n );
        }
    }
    return cnt != m_nodemap.count();
}

void MilestoneItemModel::resetModel()
{
    resetData();
    reset();
}

Qt::ItemFlags MilestoneItemModel::flags( const QModelIndex &index ) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags( index );
    if ( !index.isValid() ) {
        if ( m_readWrite ) {
            flags |= Qt::ItemIsDropEnabled;
        }
        return flags;
    }
    if ( m_readWrite ) {
        flags |= Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
        switch ( index.column() ) {
            case NodeModel::NodeName: // name
                flags |= Qt::ItemIsEditable;
                break;
            case NodeModel::NodeType: break; // Node type
            case NodeModel::NodeResponsible: // Responsible
                flags |= Qt::ItemIsEditable;
                break;
            case NodeModel::NodeConstraint: // constraint type
                flags |= Qt::ItemIsEditable;
                break;
            case NodeModel::NodeConstraintStart: { // constraint start
                Node *n = node( index );
                if ( n == 0 )
                    break;
                int c = n->constraint();
                if ( c == Node::MustStartOn || c == Node::StartNotEarlier || c == Node::FixedInterval ) {
                    flags |= Qt::ItemIsEditable;
                }
                break;
            }
            case NodeModel::NodeConstraintEnd: { // constraint end
                Node *n = node( index );
                if ( n == 0 )
                    break;
                int c = n->constraint();
                if ( c == Node::MustFinishOn || c == Node::FinishNotLater || c ==  Node::FixedInterval ) {
                    flags |= Qt::ItemIsEditable;
                }
                break;
            }
            case NodeModel::NodeStartupAccount: // startup account
            case NodeModel::NodeStartupCost: // startup cost
            case NodeModel::NodeShutdownAccount: // shutdown account
            case NodeModel::NodeShutdownCost: { // shutdown cost
                Node *n = node( index );
                if ( n && (n->type() == Node::Type_Task || n->type() == Node::Type_Milestone) ) {
                    flags |= Qt::ItemIsEditable;
                }
                break;
            }
            case NodeModel::NodeDescription: // description
                break;
            default: 
                flags &= ~Qt::ItemIsEditable;
        }
    }
    return flags;
}

QModelIndex MilestoneItemModel::parent( const QModelIndex &index ) const
{
    return QModelIndex();
}

QModelIndex MilestoneItemModel::index( int row, int column, const QModelIndex &parent ) const
{
    //kDebug()<<parent<<row<<", "<<m_nodemap.count();
    if ( m_project == 0 || row < 0 || column < 0 ) {
        //kDebug()<<"No project"<<m_project<<" or illegal row, column"<<row<<column;
        return QModelIndex();
    }
    if ( parent.isValid() || row >= m_nodemap.count() ) {
        //kDebug()<<"No index for"<<parent<<row<<","<<column;
        return QModelIndex();
    }
    return createIndex( row, column, m_nodemap.values().at( row ) );
}

QModelIndex MilestoneItemModel::index( const Node *node ) const
{
    if ( m_project == 0 || node == 0 ) {
        return QModelIndex();
    }
    return createIndex( m_nodemap.values().indexOf( const_cast<Node*>( node ) ), 0, const_cast<Node*>(node) );
}

bool MilestoneItemModel::setName( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            if ( value.toString() == node->name() ) {
                return false;
            }
            emit executeCommand( new NodeModifyNameCmd( *node, value.toString(), "Modify task name" ) );
            return true;
    }
    return false;
}

bool MilestoneItemModel::setLeader( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            if ( value.toString() == node->leader() ) {
                return false;
            }
            emit executeCommand( new NodeModifyLeaderCmd( *node, value.toString(), "Modify task responsible" ) );
            return true;
    }
    return false;
}

bool MilestoneItemModel::setDescription( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            if ( value.toString() == node->description() ) {
                return false;
            }
            emit executeCommand( new NodeModifyDescriptionCmd( *node, value.toString(), "Modify task description" ) );
            return true;
    }
    return false;
}

bool MilestoneItemModel::setType( Node *, const QVariant &, int )
{
    return false;
}

bool MilestoneItemModel::setConstraint( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            Node::ConstraintType v = Node::ConstraintType( value.toInt() );
            //kDebug()<<v;
            if ( v == node->constraint() ) {
                return false;
            }
            emit executeCommand( new NodeModifyConstraintCmd( *node, v, "Modify constraint type" ) );
            return true;
    }
    return false;
}

bool MilestoneItemModel::setConstraintStartTime( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            if ( value.toDateTime() == node->constraintStartTime().dateTime() ) {
                return false;
            }
            emit executeCommand( new NodeModifyConstraintStartTimeCmd( *node, value.toDateTime(), "Modify constraint start time" ) );
            return true;
    }
    return false;
}

bool MilestoneItemModel::setConstraintEndTime( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            if ( value.toDateTime() == node->constraintEndTime().dateTime() ) {
                return false;
            }
            emit executeCommand( new NodeModifyConstraintEndTimeCmd( *node, value.toDateTime(), "Modify constraint end time" ) );
            return true;
    }
    return false;
}

bool MilestoneItemModel::setRunningAccount( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            //kDebug()<<node->name();
            QStringList lst = m_nodemodel.runningAccount( node, Role::EnumList ).toStringList();
            if ( value.toInt() >= lst.count() ) {
                return false;
            }
            Account *a = m_project->accounts().findAccount( lst.at( value.toInt() ) );
            Account *old = node->runningAccount();
            if ( old != a ) {
                emit executeCommand( new NodeModifyRunningAccountCmd( *node, old, a, i18n( "Modify Running Account" ) ) );
            }
            return true;
    }
    return false;
}

bool MilestoneItemModel::setStartupAccount( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            //kDebug()<<node->name();
            QStringList lst = m_nodemodel.startupAccount( node, Role::EnumList ).toStringList();
            if ( value.toInt() >= lst.count() ) {
                return false;
            }
            Account *a = m_project->accounts().findAccount( lst.at( value.toInt() ) );
            Account *old = node->startupAccount();
            //kDebug()<<(value.toInt())<<";"<<(lst.at( value.toInt()))<<":"<<a;
            if ( old != a ) {
                emit executeCommand( new NodeModifyStartupAccountCmd( *node, old, a, i18n( "Modify Startup Account" ) ) );
            }
            return true;
    }
    return false;
}

bool MilestoneItemModel::setStartupCost( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            double v = KGlobal::locale()->readMoney( value.toString() );
            if ( v == node->startupCost() ) {
                return false;
            }
            emit executeCommand( new NodeModifyStartupCostCmd( *node, v, i18n( "Modify Startup Cost" ) ) );
            return true;
    }
    return false;
}

bool MilestoneItemModel::setShutdownAccount( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            //kDebug()<<node->name();
            QStringList lst = m_nodemodel.shutdownAccount( node, Role::EnumList ).toStringList();
            if ( value.toInt() >= lst.count() ) {
                return false;
            }
            Account *a = m_project->accounts().findAccount( lst.at( value.toInt() ) );
            Account *old = node->shutdownAccount();
            if ( old != a ) {
                emit executeCommand( new NodeModifyShutdownAccountCmd( *node, old, a, i18n( "Modify Shutdown Account" ) ) );
            }
            return true;
    }
    return false;
}

bool MilestoneItemModel::setShutdownCost( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            double v = KGlobal::locale()->readMoney( value.toString() );
            if ( v == node->shutdownCost() ) {
                return false;
            }
            emit executeCommand( new NodeModifyShutdownCostCmd( *node, v, i18n( "Modify Shutdown Cost" ) ) );
            return true;
    }
    return false;
}

QVariant MilestoneItemModel::data( const QModelIndex &index, int role ) const
{
    QVariant result;
    Node *n = node( index );
    if ( n != 0 ) {
        if ( index.column() == NodeModel::NodeType && role == KDGantt::ItemTypeRole ) {
            result = m_nodemodel.data( n, index.column(), Qt::EditRole );
            switch ( result.toInt() ) {
                case Node::Type_Summarytask: return KDGantt::TypeSummary;
                case Node::Type_Milestone: return KDGantt::TypeEvent;
                default: return KDGantt::TypeTask;
            }
            return result;
        }
    }
    result = m_nodemodel.data( n, index.column(), role );
    if ( role == Qt::DisplayRole && ! result.isValid() ) {
        result = " "; // HACK to show focus in empty cells
    }
    return result;
}

bool MilestoneItemModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
    if ( ( flags(index) &Qt::ItemIsEditable ) == 0 || role != Qt::EditRole ) {
        return false;
    }
    Node *n = node( index );
    switch (index.column()) {
        case NodeModel::NodeName: return setName( n, value, role );
        case NodeModel::NodeType: return setType( n, value, role );
        case NodeModel::NodeResponsible: return setLeader( n, value, role );
        case NodeModel::NodeAllocation: return false;
        case NodeModel::NodeConstraint: return setConstraint( n, value, role );
        case NodeModel::NodeConstraintStart: return setConstraintStartTime( n, value, role );
        case NodeModel::NodeConstraintEnd: return setConstraintEndTime( n, value, role );
        case NodeModel::NodeRunningAccount: return setRunningAccount( n, value, role );
        case NodeModel::NodeStartupAccount: return setStartupAccount( n, value, role );
        case NodeModel::NodeStartupCost: return setStartupCost( n, value, role );
        case NodeModel::NodeShutdownAccount: return setShutdownAccount( n, value, role );
        case NodeModel::NodeShutdownCost: return setShutdownCost( n, value, role );
        case NodeModel::NodeDescription: return setDescription( n, value, role );
        default:
            qWarning("data: invalid display value column %d", index.column());
            return false;
    }
    return false;
}

QVariant MilestoneItemModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal ) {
        if ( role == Qt::DisplayRole ) {
            return m_nodemodel.headerData( section, role );
        } else if ( role == Qt::TextAlignmentRole ) {
            switch (section) {
                case NodeModel::NodeName: return Qt::AlignLeft;
                case NodeModel::NodeType: return Qt::AlignCenter;
                default: return QVariant();
            }
        }
    }
    if ( role == Qt::ToolTipRole ) {
        return NodeModel::headerData( section, role );
    }
    return ItemModelBase::headerData(section, orientation, role);
}

QAbstractItemDelegate *MilestoneItemModel::createDelegate( int column, QWidget *parent ) const
{
    switch ( column ) {
        case NodeModel::NodeConstraint: return new EnumDelegate( parent );
        case NodeModel::NodeRunningAccount: return new EnumDelegate( parent );
        case NodeModel::NodeStartupAccount: return new EnumDelegate( parent );
        case NodeModel::NodeStartupCost: return new MoneyDelegate( parent );
        case NodeModel::NodeShutdownAccount: return new EnumDelegate( parent );
        case NodeModel::NodeShutdownCost: return new MoneyDelegate( parent );
        default: return 0;
    }
    return 0;
}

int MilestoneItemModel::columnCount( const QModelIndex &/*parent*/ ) const
{
    return m_nodemodel.propertyCount();
}

int MilestoneItemModel::rowCount( const QModelIndex &parent ) const
{
    //kDebug()<<parent;
    if ( parent.isValid() ) {
        return 0;
    }
    //kDebug()<<m_nodemap.count();
    return m_nodemap.count();
}

Qt::DropActions MilestoneItemModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}


QStringList MilestoneItemModel::mimeTypes() const
{
    return QStringList();
}

QMimeData *MilestoneItemModel::mimeData( const QModelIndexList & indexes ) const
{
    QMimeData *m = new QMimeData();
    QByteArray encodedData;
    QDataStream stream(&encodedData, QIODevice::WriteOnly);
    QList<int> rows;
    foreach (const QModelIndex &index, indexes) {
        if ( index.isValid() && !rows.contains( index.row() ) ) {
            //kDebug()<<index.row();
            Node *n = node( index );
            if ( n ) {
                rows << index.row();
                stream << n->id();
            }
        }
    }
    m->setData("application/x-vnd.kde.kplato.nodeitemmodel.internal", encodedData);
    return m;
}

bool MilestoneItemModel::dropAllowed( const QModelIndex &index, int dropIndicatorPosition, const QMimeData *data )
{
    //kDebug();
    Node *dn = node( index );
    if ( dn == 0 ) {
        kError()<<"no node to drop on!";
        return false; // hmmm
    }
    switch ( dropIndicatorPosition ) {
        case ItemModelBase::AboveItem:
        case ItemModelBase::BelowItem:
            // dn == sibling
            return dropAllowed( dn->parentNode(), data );
        case ItemModelBase::OnItem:
            // dn == new parent
            return dropAllowed( dn, data );
        default:
            break;
    }
    return false;
}

bool MilestoneItemModel::dropAllowed( Node *on, const QMimeData *data )
{
    if ( !data->hasFormat("application/x-vnd.kde.kplato.nodeitemmodel.internal") ) {
        return false;
    }
    if ( on == m_project ) {
        return true;
    }
    QByteArray encodedData = data->data( "application/x-vnd.kde.kplato.nodeitemmodel.internal" );
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    QList<Node*> lst = nodeList( stream );
    foreach ( Node *n, lst ) {
        if ( on == n || on->isChildOf( n ) ) {
            return false;
        }
    }
    lst = removeChildNodes( lst );
    foreach ( Node *n, lst ) {
        if ( ! m_project->canMoveTask( n, on ) ) {
            return false;
        }
    }
    return true;
}

QList<Node*> MilestoneItemModel::nodeList( QDataStream &stream )
{
    QList<Node*> lst;
    while (!stream.atEnd()) {
        QString id;
        stream >> id;
        Node *node = m_project->findNode( id );
        if ( node ) {
            lst << node;
        }
    }
    return lst;
}

QList<Node*> MilestoneItemModel::removeChildNodes( QList<Node*> nodes )
{
    QList<Node*> lst;
    foreach ( Node *node, nodes ) {
        bool ins = true;
        foreach ( Node *n, lst ) {
            if ( node->isChildOf( n ) ) {
                //kDebug()<<node->name()<<" is child of"<<n->name();
                ins = false;
                break;
            }
        }
        if ( ins ) {
            //kDebug()<<" insert"<<node->name();
            lst << node;
        }
    }
    QList<Node*> nl = lst;
    QList<Node*> nlst = lst;
    foreach ( Node *node, nl ) {
        foreach ( Node *n, nlst ) {
            if ( n->isChildOf( node ) ) {
                //kDebug()<<n->name()<<" is child of"<<node->name();
                int i = nodes.indexOf( n );
                lst.removeAt( i );
            }
        }
    }
    return lst;
}

bool MilestoneItemModel::dropMimeData( const QMimeData *data, Qt::DropAction action, int row, int /*column*/, const QModelIndex &parent )
{
    //kDebug()<<action;
    if (action == Qt::IgnoreAction) {
        return true;
    }
    if ( !data->hasFormat( "application/x-vnd.kde.kplato.nodeitemmodel.internal" ) ) {
        return false;
    }
    if ( action == Qt::MoveAction ) {
        //kDebug()<<"MoveAction";
        
        QByteArray encodedData = data->data( "application/x-vnd.kde.kplato.nodeitemmodel.internal" );
        QDataStream stream(&encodedData, QIODevice::ReadOnly);
        Node *par = 0;
        if ( parent.isValid() ) {
            par = node( parent );
        } else {
            par = m_project;
        }
        QList<Node*> lst = nodeList( stream );
        QList<Node*> nodes = removeChildNodes( lst ); // children goes with their parent
        foreach ( Node *n, nodes ) {
            if ( ! m_project->canMoveTask( n, par ) ) {
                //kDebug()<<"Can't move task:"<<n->name();
                return false;
            }
        }
        int offset = 0;
        MacroCommand *cmd = 0;
        foreach ( Node *n, nodes ) {
            if ( cmd == 0 ) cmd = new MacroCommand( i18n( "Move tasks" ) );
            cmd->addCommand( new NodeMoveCmd( m_project, n, par, row + offset ) );
            offset++;
        }
        if ( cmd ) {
            emit executeCommand( cmd );
        }
        //kDebug()<<row<<","<<column<<" parent="<<parent.row()<<","<<parent.column()<<":"<<par->name();
        return true;
    }
    return false;
}

Node *MilestoneItemModel::node( const QModelIndex &index ) const
{
    Node *n = 0;
    if ( index.isValid() ) {
        //kDebug()<<index;
        n = static_cast<Node*>( index.internalPointer() );
    }
    return n;
}

void MilestoneItemModel::slotNodeChanged( Node *node )
{
    //kDebug()<<node->name();
    if ( node == 0 ) {
        return;
    }
//    if ( ! m_nodemap.contains( node->wbsCode() ) || m_nodemap.value( node->wbsCode() ) != node ) {
        emit layoutAboutToBeChanged();
        if ( resetData() ) {
            reset();
        } else {
            emit layoutChanged();
        }
        return;
/*    }
    int row = m_nodemap.values().indexOf( node );
    kDebug()<<node->name()<<": "<<node->typeToString()<<row;
    emit dataChanged( createIndex( row, 0, node ), createIndex( row, columnCount()-1, node ) );*/
}

void MilestoneItemModel::slotWbsDefinitionChanged()
{
    //kDebug();
    if ( m_project == 0 ) {
        return;
    }
    if ( ! m_nodemap.isEmpty() ) {
        emit layoutAboutToBeChanged();
        resetData();
        emit layoutChanged();
    }
}

//--------------
NodeSortFilterProxyModel::NodeSortFilterProxyModel( ItemModelBase* model, QObject *parent, bool filterUnscheduled )
    : QSortFilterProxyModel( parent ),
    m_filterUnscheduled( filterUnscheduled )
{
    setSourceModel( model );
    setDynamicSortFilter( true );
}

ItemModelBase *NodeSortFilterProxyModel::itemModel() const
{
    return static_cast<ItemModelBase *>( sourceModel() );
}

void NodeSortFilterProxyModel::setFilterUnscheduled( bool on ) {
    m_filterUnscheduled = on;
    invalidateFilter();
}

bool NodeSortFilterProxyModel::filterAcceptsRow ( int row, const QModelIndex & parent ) const
{
    //kDebug()<<sourceModel()<<row<<parent;
    if ( itemModel()->project() == 0 ) {
        //kDebug()<<itemModel()->project();
        return false;
    }
    if ( m_filterUnscheduled ) {
        QString s = sourceModel()->data( sourceModel()->index( row, NodeModel::NodeNotScheduled, parent ), Qt::EditRole ).toString();
        if ( s == "true" ) {
            //kDebug()<<"Filtered unscheduled:"<<sourceModel()->index( row, 0, parent );
            return false;
        }
    }
    bool accepted = QSortFilterProxyModel::filterAcceptsRow( row, parent );
    //kDebug()<<this<<sourceModel()->index( row, 0, parent )<<"accepted ="<<accepted<<filterRegExp()<<filterRegExp().isEmpty()<<filterRegExp().capturedTexts();
    return accepted;
}

} //namespace KPlato

#include "kptnodeitemmodel.moc"
