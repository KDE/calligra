/* This file is part of the KDE project
  Copyright (C) 2008 - 2011 Dag Andersen <danders@get2net.dk>

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
  Boston, MA 02110-1301, USA.
*/

#include "kptganttitemdelegate.h"

#include "kptnodeitemmodel.h"
#include "kptnode.h"
#include "kptresourceappointmentsmodel.h"
#include "kptdebug.h"

#include <QModelIndex>
#include <QApplication>
#include <QPainter>

#include <klocale.h>
#include <kglobal.h>

#include "kdganttglobal.h"
#include "kdganttstyleoptionganttitem.h"
#include "kdganttconstraint.h"
#include "kdganttabstractgrid.h"

/// The main namespace
namespace KPlato
{

GanttItemDelegate::GanttItemDelegate( QObject *parent )
    : KDGantt::ItemDelegate( parent ),
    showResources( false ),
    showTaskName( true ),
    showTaskLinks( true ),
    showProgress( false ),
    showPositiveFloat( false ),
    showNegativeFloat( false ),
    showCriticalPath( false ),
    showCriticalTasks( false ),
    showAppointments( false ),
    showTimeConstraint( false ),
    showSchedulingError( false )
{
    QLinearGradient b( 0., 0., 0., QApplication::fontMetrics().height() );
    b.setColorAt( 0., Qt::red );
    b.setColorAt( 1., Qt::darkRed );
    m_criticalBrush = QBrush( b );

    QLinearGradient sb( 0., 0., 0., QApplication::fontMetrics().height() );
    sb.setColorAt( 0., Qt::yellow );
    sb.setColorAt( 1., Qt::darkYellow );
    m_schedulingErrorBrush = QBrush( sb );
}

QString GanttItemDelegate::toolTip( const QModelIndex &idx ) const
{
    if ( !idx.isValid() || ! idx.model() ) {
        return QString();
    }
    const QAbstractItemModel* model = idx.model();
    if ( data( idx, NodeModel::NodeFinished, Qt::EditRole ).toBool() ) {
        // finished
        return i18nc( "@info:tooltip",
                "Name: %1<nl/>"
                "Actual finish: %2<nl/>"
                "Planned finish: %3<nl/>"
                "Status: %4",
                    model->data( idx, Qt::DisplayRole ).toString(),
                    data( idx, NodeModel::NodeActualFinish, Qt::DisplayRole ).toString(),
                    data( idx, NodeModel::NodeEndTime, Qt::DisplayRole ).toString(),
                    data( idx, NodeModel::NodeStatus, Qt::DisplayRole ).toString() );
    }
    if ( data( idx, NodeModel::NodeStarted, Qt::EditRole ).toBool() ) {
        // started
        return i18nc( "@info:tooltip",
                "Name: %1<nl/>"
                "Completion: %2 %<nl/>"
                "Actual start: %3<nl/>"
                "Planned: %4 - %5<nl/>"
                "Status: %6",
                    model->data( idx, Qt::DisplayRole ).toString(),
                    data( idx, NodeModel::NodeCompleted, Qt::DisplayRole ).toString(),
                    data( idx, NodeModel::NodeActualStart, Qt::DisplayRole ).toString(),
                    data( idx, NodeModel::NodeStartTime, Qt::DisplayRole ).toString(),
                    data( idx, NodeModel::NodeEndTime, Qt::DisplayRole ).toString(),
                    data( idx, NodeModel::NodeStatus, Qt::DisplayRole ).toString() );
    }
    // Planned
    KDGantt::StyleOptionGanttItem opt;
    int typ = data( idx, NodeModel::NodeType, Qt::EditRole ).toInt();
    switch ( typ ) {
        case Node::Type_Task: {
            int ctyp = data( idx, NodeModel::NodeConstraint, Qt::EditRole ).toInt();
            switch ( ctyp ) {
                case Node::MustStartOn:
                case Node::StartNotEarlier:
                case Node::FixedInterval:
                        return i18nc( "@info:tooltip",
                            "Name: %1<nl/>"
                            "Planned: %2 - %3<nl/>"
                            "Status: %4<nl/>"
                            "Constraint type: %5<nl/>"
                            "Constraint time: %6<nl/>"
                            "Negative float: %7 h",
                                model->data( idx, Qt::DisplayRole ).toString(),
                                data( idx, NodeModel::NodeStartTime, Qt::DisplayRole ).toString(),
                                data( idx, NodeModel::NodeEndTime, Qt::DisplayRole ).toString(),
                                data( idx, NodeModel::NodeSchedulingStatus, Qt::DisplayRole ).toString(),
                                data( idx, NodeModel::NodeConstraint, Qt::DisplayRole ).toString(),
                                data( idx, NodeModel::NodeConstraintStart, Qt::DisplayRole ).toString(),
                                data( idx, NodeModel::NodeNegativeFloat, Qt::DisplayRole ).toString()
                                );

                case Node::MustFinishOn:
                case Node::FinishNotLater:
                        return i18nc( "@info:tooltip",
                            "Name: %1<nl/>"
                            "Planned: %2 - %3<nl/>"
                            "Status: %4<nl/>"
                            "Constraint type: %5<nl/>"
                            "Constraint time: %6<nl/>"
                            "Negative float: %7 h",
                                model->data( idx, Qt::DisplayRole ).toString(),
                                data( idx, NodeModel::NodeStartTime, Qt::DisplayRole ).toString(),
                                data( idx, NodeModel::NodeEndTime, Qt::DisplayRole ).toString(),
                                data( idx, NodeModel::NodeSchedulingStatus, Qt::DisplayRole ).toString(),
                                data( idx, NodeModel::NodeConstraint, Qt::DisplayRole ).toString(),
                                data( idx, NodeModel::NodeConstraintEnd, Qt::DisplayRole ).toString(),
                                data( idx, NodeModel::NodeNegativeFloat, Qt::DisplayRole ).toString()
                                );

                default:
                    return i18nc( "@info:tooltip",
                            "Name: %1<nl/>"
                            "Planned: %2 - %3<nl/>"
                            "Status: %4<nl/>"
                            "Scheduling: %5",
                                model->data( idx, Qt::DisplayRole ).toString(),
                                data( idx, NodeModel::NodeStartTime, Qt::DisplayRole ).toString(),
                                data( idx, NodeModel::NodeEndTime, Qt::DisplayRole ).toString(),
                                data( idx, NodeModel::NodeSchedulingStatus, Qt::DisplayRole ).toString(),
                                data( idx, NodeModel::NodeConstraint, Qt::DisplayRole ).toString()
                                );
            }
        }
        case Node::Type_Milestone: {
            int ctyp = data( idx, NodeModel::NodeConstraint, Qt::EditRole ).toInt();
            switch ( ctyp ) {
                case Node::MustStartOn:
                case Node::StartNotEarlier:
                case Node::FixedInterval:
                        return i18nc( "@info:tooltip",
                            "Name: %1<nl/>"
                            "Planned: %2<nl/>"
                            "Status: %3<nl/>"
                            "Constraint type: %4<nl/>"
                            "Constraint time: %5<nl/>"
                            "Negative float: %6 h",
                                model->data( idx, Qt::DisplayRole ).toString(),
                                data( idx, NodeModel::NodeStartTime, Qt::DisplayRole ).toString(),
                                data( idx, NodeModel::NodeSchedulingStatus, Qt::DisplayRole ).toString(),
                                data( idx, NodeModel::NodeConstraint, Qt::DisplayRole ).toString(),
                                data( idx, NodeModel::NodeConstraintStart, Qt::DisplayRole ).toString(),
                                data( idx, NodeModel::NodeNegativeFloat, Qt::DisplayRole ).toString()
                                );

                case Node::MustFinishOn:
                case Node::FinishNotLater:
                        return i18nc( "@info:tooltip",
                            "Name: %1<nl/>"
                            "Planned: %2<nl/>"
                            "Status: %3<nl/>"
                            "Constraint type: %4<nl/>"
                            "Constraint time: %5<nl/>"
                            "Negative float: %6 h",
                                model->data( idx, Qt::DisplayRole ).toString(),
                                data( idx, NodeModel::NodeStartTime, Qt::DisplayRole ).toString(),
                                data( idx, NodeModel::NodeSchedulingStatus, Qt::DisplayRole ).toString(),
                                data( idx, NodeModel::NodeConstraint, Qt::DisplayRole ).toString(),
                                data( idx, NodeModel::NodeConstraintEnd, Qt::DisplayRole ).toString(),
                                data( idx, NodeModel::NodeNegativeFloat, Qt::DisplayRole ).toString()
                                );

                default:
                    return i18nc( "@info:tooltip",
                            "Name: %1<nl/>"
                            "Planned: %2<nl/>"
                            "Status: %3<nl/>"
                            "Scheduling: %4",
                                model->data( idx, Qt::DisplayRole ).toString(),
                                data( idx, NodeModel::NodeStartTime, Qt::DisplayRole ).toString(),
                                data( idx, NodeModel::NodeSchedulingStatus, Qt::DisplayRole ).toString(),
                                data( idx, NodeModel::NodeConstraint, Qt::DisplayRole ).toString()
                                );
            }
        }
        case Node::Type_Summarytask:
            return i18nc( "@info:tooltip",
                    "Name: %1<nl/>"
                    "Planned: %2 - %3",
                        model->data( idx, Qt::DisplayRole ).toString(),
                        data( idx, NodeModel::NodeStartTime, Qt::DisplayRole ).toString(),
                        data( idx, NodeModel::NodeEndTime, Qt::DisplayRole ).toString()
                        );
    }
    return QString();
}

QVariant GanttItemDelegate::data( const QModelIndex& idx, int column, int role ) const
{
    //kDebug(planDbg())<<idx<<column<<role;
    QModelIndex i = idx.model()->index( idx.row(), column, idx.parent() );
    return i.data( role );
}

QString GanttItemDelegate::itemText( const QModelIndex& idx, int type ) const
{
    if ( idx.model()->data( idx, GanttItemModel::SpecialItemTypeRole ).toInt() > 0 ) {
        return QString();
    }
    QString txt;
    if ( showTaskName ) {
        txt = data( idx, NodeModel::NodeName, Qt::DisplayRole ).toString();
    }
    if ( type == KDGantt::TypeTask && showResources ) {
        if ( ! txt.isEmpty() ) {
            txt += ' ';
        }
        txt += '(' + data( idx, NodeModel::NodeAssignments, Qt::DisplayRole ).toString() + ')';
    }
    //kDebug(planDbg())<<txt;
    return txt;
}

QRectF GanttItemDelegate::itemPositiveFloatRect( const KDGantt::StyleOptionGanttItem& opt, const QModelIndex& idx ) const
{
    QRectF r;
    double fl = data( idx, NodeModel::NodePositiveFloat, Qt::EditRole ).toDouble();
    if ( fl == 0.0 ) {
        return r;
    }
    QDateTime et = data( idx, NodeModel::NodeEndTime, Qt::EditRole ).toDateTime();
    if ( ! et.isValid() ) {
        return r;
    }
    QDateTime dt = ( DateTime( et ) + Duration( fl, Duration::Unit_h ) );
    qreal delta = opt.itemRect.height() / 6.0;
    r.setLeft( opt.itemRect.right() );
    r.setWidth( opt.grid->mapToChart( dt ) - opt.grid->mapToChart( et ) );
    r.setTop( opt.itemRect.bottom() - delta );
    r.setHeight( delta );
    return r;
}

QRectF GanttItemDelegate::itemNegativeFloatRect( const KDGantt::StyleOptionGanttItem& opt, const QModelIndex& idx ) const
{
    QRectF r;
    double fl = data( idx, NodeModel::NodeNegativeFloat, Qt::EditRole ).toDouble();
    if ( fl == 0.0 ) {
        return r;
    }
    QDateTime st = data( idx, NodeModel::NodeStartTime, Qt::EditRole ).toDateTime();
    if ( ! st.isValid() ) {
        return r;
    }
    QDateTime dt = ( DateTime( st ) - Duration( fl, Duration::Unit_h ) );
    qreal delta = opt.itemRect.height() / 6.0;
    r.setLeft( opt.itemRect.left() - qAbs( opt.grid->mapToChart( st ) - opt.grid->mapToChart( dt ) ) );
    r.setRight( opt.itemRect.left() );
    r.setTop( opt.itemRect.bottom() - delta );
    r.setHeight( delta );
    return r;
}

bool GanttItemDelegate::hasStartConstraint( const QModelIndex& idx ) const
{
    //kDebug(planDbg())<<data( idx, NodeModel::NodeName ).toString()<<data( idx, NodeModel::NodeConstraint ).toString()<<data( idx, NodeModel::NodeConstraint, Qt::EditRole ).toInt();
    switch ( data( idx, NodeModel::NodeConstraint, Qt::EditRole ).toInt() ) {
        case Node::FixedInterval:
        case Node::StartNotEarlier:
        case Node::MustStartOn: return true;
        default: break;
    }
    return false;
}

QRectF GanttItemDelegate::itemStartConstraintRect( const KDGantt::StyleOptionGanttItem& opt, const QModelIndex& idx ) const
{
    QRectF r;
    QDateTime dt;
    if ( hasStartConstraint( idx ) ) {
        dt = data( idx, NodeModel::NodeConstraintStart, Qt::EditRole ).toDateTime();
    }
    if ( ! dt.isValid() ) {
        return r;
    }
    QDateTime st = data( idx, NodeModel::NodeStartTime, Qt::EditRole ).toDateTime();
    if ( ! st.isValid() ) {
        return r;
    }
    qreal delta = opt.itemRect.height() / 2.0;
    r.setX( opt.grid->mapToChart( dt ) - opt.grid->mapToChart( st ) - delta );
    r.setY( opt.itemRect.y() + ( delta / 2.0 ) );
    r.setWidth( delta );
    r.setHeight( delta );
    return r;
}

bool GanttItemDelegate::hasEndConstraint( const QModelIndex& idx ) const
{
    //kDebug(planDbg())<<data( idx, NodeModel::NodeName ).toString()<<data( idx, NodeModel::NodeConstraint ).toString()<<data( idx, NodeModel::NodeConstraint, Qt::EditRole ).toInt();
    switch ( data( idx, NodeModel::NodeConstraint, Qt::EditRole ).toInt() ) {
        case Node::FixedInterval:
        case Node::FinishNotLater:
        case Node::MustFinishOn: return true;
        default: break;
    }
    return false;
}

QRectF GanttItemDelegate::itemEndConstraintRect( const KDGantt::StyleOptionGanttItem& opt, const QModelIndex& idx ) const
{
    QRectF r;
    QDateTime dt;
    if ( hasEndConstraint( idx ) ) {
        dt = data( idx, NodeModel::NodeConstraintEnd, Qt::EditRole ).toDateTime();
    }
    if ( ! dt.isValid() ) {
        return r;
    }
    QDateTime et = data( idx, NodeModel::NodeEndTime, Qt::EditRole ).toDateTime();
    if ( ! et.isValid() ) {
        return r;
    }
    qreal delta = opt.itemRect.height() / 2.0;
    r.setX( opt.itemRect.right() + ( opt.grid->mapToChart( dt ) - opt.grid->mapToChart( et ) ) );
    r.setY( opt.itemRect.y() + ( delta / 2.0 ) );
    r.setWidth( delta );
    r.setHeight( delta );
    return r;
}

KDGantt::Span GanttItemDelegate::itemBoundingSpan( const KDGantt::StyleOptionGanttItem& opt, const QModelIndex& idx ) const
{
    //kDebug(planDbg())<<opt<<idx;
    if ( !idx.isValid() ) return KDGantt::Span();

    QRectF optRect = opt.itemRect;
    QRectF itemRect = opt.itemRect;

    int typ = idx.model()->data( idx, KDGantt::ItemTypeRole ).toInt();
    QString txt = itemText( idx, typ );
    int tw = 0;
    if ( ! txt.isEmpty() ) {
        tw = opt.fontMetrics.width( txt );
        tw += static_cast<int>( itemRect.height()/1.5 );
    }
    if ( showPositiveFloat ) {
        QRectF fr = itemPositiveFloatRect( opt, idx );
        if ( fr.isValid() ) {
            itemRect = itemRect.united( fr );
        }
    }
    if ( showNegativeFloat ) {
        QRectF fr = itemNegativeFloatRect( opt, idx );
        if ( fr.isValid() ) {
            itemRect = itemRect.united( fr );
        }
    }
    if ( showTimeConstraint ) {
        QRectF cr = itemStartConstraintRect( opt, idx );
        if ( cr.isValid() ) {
            itemRect = itemRect.united( cr );
        }
        cr = itemEndConstraintRect( opt, idx );
        if ( cr.isValid() ) {
            itemRect = itemRect.united( cr );
        }
    }
    if ( idx.model()->data( idx, GanttItemModel::SpecialItemTypeRole ).toInt() > 0 ) {
        itemRect = itemRect.united( QRectF( opt.rect.left()-itemRect.height()/4., itemRect.top(), itemRect.height()/2., itemRect.height() ) );
    } else if (  typ == KDGantt::TypeEvent ) {
        optRect = QRectF( opt.rect.left()-itemRect.height()/2., itemRect.top(), itemRect.height(), itemRect.height() );
        itemRect = itemRect.united( optRect );
    }
    switch ( opt.displayPosition ) {
        case KDGantt::StyleOptionGanttItem::Left:
            itemRect = itemRect.united( optRect.adjusted( -tw, 0.0, 0.0, 0.0 ) );
            break;
        case KDGantt::StyleOptionGanttItem::Right:
            itemRect = itemRect.united( optRect.adjusted( 0.0, 0.0, tw, 0.0 ) );
            break;
        case KDGantt::StyleOptionGanttItem::Center:
            if ( optRect.width() < tw ) {
                itemRect = itemRect.united( optRect.adjusted( 0.0, 0.0, tw - optRect.width(), 0.0 ) );
            }
            break;
    }
    return KDGantt::Span( itemRect.left(), itemRect.width() );
}

/*! Paints the gantt item \a idx using \a painter and \a opt
 */
void GanttItemDelegate::paintGanttItem( QPainter* painter, const KDGantt::StyleOptionGanttItem& opt, const QModelIndex& idx )
{
    if ( !idx.isValid() ) return;

    int specialtype = idx.model()->data( idx, GanttItemModel::SpecialItemTypeRole ).toInt();
    if ( specialtype > 0 ) {
        return paintSpecialItem( painter, opt, idx, specialtype );
    }
    const KDGantt::ItemType typ = static_cast<KDGantt::ItemType>( idx.model()->data( idx, KDGantt::ItemTypeRole ).toInt() );

    QString txt = itemText( idx, typ );
    QRectF itemRect = opt.itemRect;

//     painter->save();
//     painter->setPen( Qt::blue );
//     painter->drawRect( opt.boundingRect.adjusted( -1., -1., 1., 1. ) );
//     painter->setPen( Qt::red );
//     painter->drawRect( itemRect );
//     painter->restore();

    QRectF textRect = itemRect;
    if ( ! txt.isEmpty() ) {
        int tw = opt.fontMetrics.width( txt ) + static_cast<int>( itemRect.height()/1.5 );
        switch( opt.displayPosition ) {
            case KDGantt::StyleOptionGanttItem::Left:
                textRect.adjust( -tw, 0.0, 0.0, 0.0 );
                break;
            case KDGantt::StyleOptionGanttItem::Right:
                textRect.adjust( 0.0, 0.0, tw, 0.0 );
                break;
            default:
                break;
        }
    }
    painter->save();

    QPen pen = defaultPen( typ );
    if ( opt.state & QStyle::State_Selected ) pen.setWidth( 2*pen.width() );
    painter->setPen( pen );

    qreal pw = painter->pen().width()/2.;
    switch( typ ) {
    case KDGantt::TypeTask:
        if ( itemRect.isValid() ) {
            pw-=1;
            QRectF r = itemRect;
            r.translate( 0., r.height()/6. );
            r.setHeight( 2.*r.height()/3. );
            painter->save();
            painter->setBrushOrigin( itemRect.topLeft() );
            painter->translate( 0.5, 0.5 );
            if ( showPositiveFloat ) {
                QRectF fr = itemPositiveFloatRect( opt, idx );
                if ( fr.isValid() ) {
                    painter->fillRect( fr, painter->pen().brush() );
                }
            }
            if ( showNegativeFloat ) {
                QRectF fr = itemNegativeFloatRect( opt, idx );
                if ( fr.isValid() ) {
                    painter->fillRect( fr, painter->pen().brush() );
                }
            }
            bool normal = true;
            if ( showSchedulingError ) {
                QList<int> lst;
                lst << NodeModel::NodeAssignmentMissing
                    << NodeModel::NodeResourceOverbooked
                    << NodeModel::NodeResourceUnavailable
                    << NodeModel::NodeConstraintsError
                    << NodeModel::NodeEffortNotMet
                    << NodeModel::NodeSchedulingError;
                foreach ( int i, lst ) {
                    QVariant v = data( idx, i, Qt::EditRole );
                    //kDebug(planDbg())<<idx.data(NodeModel::NodeName).toString()<<": showSchedulingError"<<i<<v;
                    if (  v.toBool() ) {
                        QVariant br = data( idx, i, Role::Foreground );
                        painter->setBrush( br.isValid() ? br.value<QBrush>() : m_schedulingErrorBrush );
                        normal = false;
                        break;
                    }
                }

            } else if ( showCriticalTasks ) {
                bool critical = data( idx, NodeModel::NodeCritical, Qt::DisplayRole ).toBool();
                if ( ! critical && showCriticalPath ) {
                    critical = data( idx, NodeModel::NodeCriticalPath, Qt::DisplayRole ).toBool();
                }
                if ( critical ) {
                    QVariant br = data( idx, NodeModel::NodeCritical, Role::Foreground );
                    painter->setBrush( br.isValid() ? br.value<QBrush>() : m_criticalBrush );
                    normal = false;
                }
            }
            if ( normal ) {
                QVariant br = idx.data( Role::Foreground );
                painter->setBrush( br.isValid() ? br.value<QBrush>() : defaultBrush( typ ) );
            }
            painter->drawRect( r );

            if ( showProgress ) {
                bool ok;
                qreal completion = idx.model()->data( idx, KDGantt::TaskCompletionRole ).toDouble( &ok );
                if ( ok ) {
                    qreal h = r.height();
                    QRectF cr( r.x(), r.y()+h/4. + 1,
                            r.width()*completion/100., h/2. - 2 );
                    painter->fillRect( cr, painter->pen().brush() );
                }
            }
            painter->restore();
            if ( showTimeConstraint ) {
                //kDebug(planDbg())<<data( idx, NodeModel::NodeName ).toString()<<data( idx, NodeModel::NodeConstraint ).toString()<<r<<boundingRect;
                painter->save();
                painter->setBrush( QBrush( Qt::darkGray ) );
                painter->setPen( Qt::black );
                QRectF cr = itemStartConstraintRect( opt, idx );
                if ( cr.isValid() ) {
                    QPainterPath p;
                    p.moveTo( cr.topLeft() );
                    p.lineTo( cr.bottomLeft() );
                    p.lineTo( cr.right(), cr.top() + ( cr.height() / 2.0 ) );
                    p.closeSubpath();
                    painter->drawPath( p );
                    if ( data( idx, NodeModel::NodeConstraint, Qt::EditRole ).toInt() != Node::StartNotEarlier ) {
                        painter->fillPath( p, QBrush( Qt::red ) );
                    }
                }
                cr = itemEndConstraintRect( opt, idx );
                if ( cr.isValid() ) {
                    QPainterPath p;
                    p.moveTo( cr.topRight() );
                    p.lineTo( cr.bottomRight() );
                    p.lineTo( cr.left(), cr.top() + ( cr.height() / 2.0 ) );
                    p.closeSubpath();
                    painter->drawPath( p );
                    if ( data( idx, NodeModel::NodeConstraint, Qt::EditRole ).toInt() != Node::FinishNotLater ) {
                        painter->fillPath( p, QBrush( Qt::red ) );
                    }
                }
                painter->restore();
            }
            Qt::Alignment ta;
            switch( opt.displayPosition ) {
            case KDGantt::StyleOptionGanttItem::Left: ta = Qt::AlignLeft; break;
            case KDGantt::StyleOptionGanttItem::Right: ta = Qt::AlignRight; break;
            case KDGantt::StyleOptionGanttItem::Center: ta = Qt::AlignCenter; break;
            }
            painter->drawText( textRect, ta, txt );
        }
        break;
    case KDGantt::TypeSummary:
        if ( opt.itemRect.isValid() ) {
            pw-=1;
            const QRectF r = QRectF( itemRect ).adjusted( -pw, -pw, pw, pw );
            QPainterPath path;
            const qreal deltaY = r.height()/2.;
            const qreal deltaX = qMin( r.width()/qreal(2), deltaY );
            path.moveTo( r.topLeft() );
            path.lineTo( r.topRight() );
            path.lineTo( QPointF( r.right(), r.top() + 2.*deltaY ) );
            //path.lineTo( QPointF( r.right()-3./2.*delta, r.top() + delta ) );
            path.quadTo( QPointF( r.right()-.5*deltaX, r.top() + deltaY ), QPointF( r.right()-2.*deltaX, r.top() + deltaY ) );
            //path.lineTo( QPointF( r.left()+3./2.*delta, r.top() + delta ) );
            path.lineTo( QPointF( r.left() + 2.*deltaX, r.top() + deltaY ) );
            path.quadTo( QPointF( r.left()+.5*deltaX, r.top() + deltaY ), QPointF( r.left(), r.top() + 2.*deltaY ) );
            path.closeSubpath();
            painter->setBrushOrigin( itemRect.topLeft() );
            painter->save();
            QVariant br = idx.data( Role::Foreground );
            painter->setBrush( br.isValid() ? br.value<QBrush>() : defaultBrush( typ ) );
            painter->translate( 0.5, 0.5 );
            painter->drawPath( path );
            painter->restore();
            Qt::Alignment ta;
            switch( opt.displayPosition ) {
            case KDGantt::StyleOptionGanttItem::Left: ta = Qt::AlignLeft; break;
            case KDGantt::StyleOptionGanttItem::Right: ta = Qt::AlignRight; break;
            case KDGantt::StyleOptionGanttItem::Center: ta = Qt::AlignCenter; break;
            }
            painter->drawText( textRect, ta | Qt::AlignVCenter, txt );
        }
        break;
    case KDGantt::TypeEvent:
        //kDebug(planDbg()) << opt.boundingRect << opt.itemRect;
        if ( opt.boundingRect.isValid() ) {
            pw-=1;
            painter->save();
            QRectF ir = itemRect;
            // do the same as for TypeTask
            ir.translate( 0., ir.height()/6. );
            ir.setHeight( 2.*ir.height()/3. );
            painter->setBrushOrigin( ir.topLeft() );
            painter->translate( 0.5, 0.5 );
            if ( showPositiveFloat ) {
                QRectF fr = itemPositiveFloatRect( opt, idx );
                if ( fr.isValid() ) {
                    painter->fillRect( fr, painter->pen().brush() );
                }
            }
            if ( showNegativeFloat ) {
                QRectF fr = itemNegativeFloatRect( opt, idx );
                if ( fr.isValid() ) {
                    painter->fillRect( fr, painter->pen().brush() );
                }
            }
            painter->restore();
            bool normal = true;
            if ( showSchedulingError ) {
                QList<int> lst;
                lst << NodeModel::NodeAssignmentMissing
                    << NodeModel::NodeResourceOverbooked
                    << NodeModel::NodeResourceUnavailable
                    << NodeModel::NodeConstraintsError
                    << NodeModel::NodeEffortNotMet
                    << NodeModel::NodeSchedulingError;
                foreach ( int i, lst ) {
                    QVariant v = data( idx, i, Qt::EditRole );
                    //kDebug(planDbg())<<idx.data(NodeModel::NodeName).toString()<<": showSchedulingError"<<i<<v;
                    if (  v.toBool() ) {
                        QVariant br = data( idx, i, Role::Foreground );
                        painter->setBrush( br.isValid() ? br.value<QBrush>() : m_schedulingErrorBrush );
                        normal = false;
                        break;
                    }
                }

            } else if ( showCriticalTasks ) {
                bool critical = data( idx, NodeModel::NodeCritical, Qt::DisplayRole ).toBool();
                if ( ! critical && showCriticalPath ) {
                    critical = data( idx, NodeModel::NodeCriticalPath, Qt::DisplayRole ).toBool();
                }
                if ( critical ) {
                    QVariant br = data( idx, NodeModel::NodeCritical, Role::Foreground );
                    painter->setBrush( br.isValid() ? br.value<QBrush>() : m_criticalBrush );
                    normal = false;
                }
            }
            if ( normal ) {
                QVariant br = idx.data( Role::Foreground );
                painter->setBrush( br.isValid() ? br.value<QBrush>() : defaultBrush( typ ) );
            }

            const qreal delta = static_cast< int >( ( itemRect.height() - pw ) / 2 );
            QPainterPath path;
            path.moveTo( 0., 0. );
            path.lineTo( delta, delta );
            path.lineTo( 0., 2.*delta );
            path.lineTo( -delta, delta );
            path.closeSubpath();

            painter->save();
            painter->translate( itemRect.left(), itemRect.top() + pw );
            painter->translate( 0.5, 0.5 );
            painter->drawPath( path );
            painter->restore();

            if ( showTimeConstraint ) {
                //kDebug(planDbg())<<data( idx, NodeModel::NodeName ).toString()<<data( idx, NodeModel::NodeConstraint ).toString()<<r<<boundingRect;
                painter->save();
                painter->setBrush( QBrush( Qt::darkGray ) );
                painter->setPen( Qt::black );
                QRectF cr = itemStartConstraintRect( opt, idx );
                if ( cr.isValid() ) {
                    QPainterPath p;
                    p.moveTo( cr.topLeft() );
                    p.lineTo( cr.bottomLeft() );
                    p.lineTo( cr.right(), cr.top() + ( cr.height() / 2.0 ) );
                    p.closeSubpath();
                    painter->drawPath( p );
                    if ( data( idx, NodeModel::NodeConstraint, Qt::EditRole ).toInt() != Node::StartNotEarlier ) {
                        painter->fillPath( p, QBrush( Qt::red ) );
                    }
                }
                cr = itemEndConstraintRect( opt, idx );
                if ( cr.isValid() ) {
                    QPainterPath p;
                    p.moveTo( cr.topRight() );
                    p.lineTo( cr.bottomRight() );
                    p.lineTo( cr.left(), cr.top() + ( cr.height() / 2.0 ) );
                    p.closeSubpath();
                    painter->drawPath( p );
                    if ( data( idx, NodeModel::NodeConstraint, Qt::EditRole ).toInt() != Node::FinishNotLater ) {
                        painter->fillPath( p, QBrush( Qt::red ) );
                    }
                }
                painter->restore();
            }

            Qt::Alignment ta;
            switch( opt.displayPosition ) {
            case KDGantt::StyleOptionGanttItem::Left: ta = Qt::AlignLeft; break;
            case KDGantt::StyleOptionGanttItem::Right: ta = Qt::AlignRight; break;
            case KDGantt::StyleOptionGanttItem::Center: ta = Qt::AlignCenter; break;
            }
            painter->drawText( textRect, ta | Qt::AlignVCenter, txt );
        }
        break;
    default:
        break;
    }
    painter->restore();
}

/*! Paints the gantt item \a idx using \a painter and \a opt
 */
void GanttItemDelegate::paintSpecialItem( QPainter* painter, const KDGantt::StyleOptionGanttItem& opt, const QModelIndex& /*idx*/, int typ )
{
    QRectF itemRect = opt.itemRect;
    QRectF boundingRect = opt.boundingRect;
    boundingRect.setY( itemRect.y() );
    boundingRect.setHeight( itemRect.height() );

    painter->save();

    QPen pen = defaultPen( KDGantt::TypeEvent );
    if ( opt.state & QStyle::State_Selected ) pen.setWidth( 2*pen.width() );
    painter->setPen( pen );

    qreal pw = painter->pen().width()/2.;
    switch( typ ) {
    case 1: // early start
        if ( boundingRect.isValid() ) {
            painter->setBrush( QBrush( "red" ) );
            pw-=1;
            QRectF r = boundingRect;
            r.setHeight( r.height()/3. );
            painter->setBrushOrigin( boundingRect.topLeft() );
            painter->translate( 0.5, 0.5 );
            QPainterPath p( r.topLeft() );
            p.lineTo( r.topRight() );
            p.lineTo( r.left() + ( r.width()/2.0 ), r.bottom() );
            p.closeSubpath();
            painter->fillPath( p, painter->brush() );
            //painter->drawPath( p );
        }
        break;
    case 2: // late finish
        if ( boundingRect.isValid() ) {
            painter->setBrush( QBrush( "blue" ) );
            pw-=1;
            QRectF r = boundingRect;
            r.setTop( r.bottom() - r.height()/2.8 );
            painter->setBrushOrigin( boundingRect.topLeft() );
            painter->translate( 0.5, -0.5 );

            QPainterPath p( r.bottomLeft() );
            p.lineTo( r.bottomRight() );
            p.lineTo( r.left() + ( r.width()/2.0 ), r.top() );
            p.closeSubpath();
            painter->fillPath( p, painter->brush() );
            //painter->drawPath( p );
        }
        break;
    case 3: // late start
        if ( boundingRect.isValid() ) {
            painter->setBrush( QBrush( "red" ) );
            pw-=1;
            QRectF r = boundingRect;
            r.setTop( r.bottom() - r.height()/2.8 );
            painter->setBrushOrigin( boundingRect.topLeft() );
            painter->translate( 0.5, -0.5 );

            QPainterPath p( r.bottomLeft() );
            p.lineTo( r.bottomRight() );
            p.lineTo( r.left() + ( r.width()/2.0 ), r.top() );
            p.closeSubpath();
            painter->fillPath( p, painter->brush() );
            //painter->drawPath( p );
        }
        break;
    case 4: // early finish
        if ( boundingRect.isValid() ) {
            painter->setBrush( QBrush( "blue" ) );
            pw-=1;
            QRectF r = boundingRect;
            r.setHeight( r.height()/3. );
            painter->setBrushOrigin( boundingRect.topLeft() );
            painter->translate( 0.5, 0.5 );
            QPainterPath p( r.topLeft() );
            p.lineTo( r.topRight() );
            p.lineTo( r.left() + ( r.width()/2.0 ), r.bottom() );
            p.closeSubpath();
            painter->fillPath( p, painter->brush() );
            //painter->drawPath( p );
        }
        break;
    default:
        break;
    }
    painter->restore();
}

void GanttItemDelegate::paintConstraintItem( QPainter* painter, const  QStyleOptionGraphicsItem& opt, const QPointF& start, const QPointF& end, const  KDGantt::Constraint &constraint )
{
    if ( ! showTaskLinks ) {
        return;
    }
    if ( ! showCriticalPath ) {
        return KDGantt::ItemDelegate::paintConstraintItem( painter, opt, start, end, constraint );
    }
    KDGantt::Constraint c( constraint );
    if ( data( c.startIndex(), NodeModel::NodeCriticalPath ).toBool() &&
         data( c.endIndex(), NodeModel::NodeCriticalPath ).toBool() )
    {
        //kDebug(planDbg())<<data( c.startIndex(), NodeModel::NodeName ).toString()<<data( c.endIndex(), NodeModel::NodeName ).toString()<<"critical path";
        QPen pen( Qt::red );
        c.setData( KDGantt::Constraint::ValidConstraintPen, pen );
        // FIXME How to make sure it's not obscured by other constraints?
    }
    KDGantt::ItemDelegate::paintConstraintItem( painter, opt, start, end, c );
}

//------------------------
ResourceGanttItemDelegate::ResourceGanttItemDelegate( QObject *parent )
    : KDGantt::ItemDelegate( parent )
{
    QLinearGradient b( 0., 0., 0., QApplication::fontMetrics().height() );
    b.setColorAt( 0., Qt::red );
    b.setColorAt( 1., Qt::darkRed );
    m_overloadBrush = QBrush( b );

    b.setColorAt( 0., QColor( Qt::yellow ).lighter( 175 ) );
    b.setColorAt( 1., QColor( Qt::yellow ).darker( 125 ) );
    m_underloadBrush = QBrush( b );
}

QVariant ResourceGanttItemDelegate::data( const QModelIndex& idx, int column, int role ) const
{
    QModelIndex i = idx.model()->index( idx.row(), column, idx.parent() );
    return i.data( role );
}


/*! Paints the gantt item \a idx using \a painter and \a opt
 */
void ResourceGanttItemDelegate::paintGanttItem( QPainter* painter, const KDGantt::StyleOptionGanttItem& opt, const QModelIndex& idx )
{
    if ( !idx.isValid() ) return;
    const KDGantt::ItemType typ = static_cast<KDGantt::ItemType>( idx.model()->data( idx, KDGantt::ItemTypeRole ).toInt() );
    const QString& txt = opt.text;
    QRectF itemRect = opt.itemRect;
    QRectF boundingRect = opt.boundingRect;
    boundingRect.setY( itemRect.y() );
    boundingRect.setHeight( itemRect.height() );

    painter->save();

    QPen pen = defaultPen( typ );
    if ( opt.state & QStyle::State_Selected ) pen.setWidth( 2*pen.width() );
    painter->setPen( pen );
    painter->setBrush( defaultBrush( typ ) );

    qreal pw = painter->pen().width()/2.;
    switch( typ ) {
    case KDGantt::TypeTask:
        if ( itemRect.isValid() ) {
            qreal pw = painter->pen().width()/2.;
            pw-=1;
            QRectF r = itemRect;
            r.translate( 0., r.height()/12. );
            r.setHeight( 5.*r.height()/6. );
            painter->setBrushOrigin( itemRect.topLeft() );

            painter->save();
            if ( idx.data( Role::ObjectType ).toInt() == OT_External ) {
                painter->setBrush( Qt::blue );
            }
            painter->translate( 0.5, 0.5 );
            painter->drawRect( r );
            QModelIndex ix = idx.model()->index( idx.row(), ResourceAppointmentsRowModel::Load, idx.parent() );
            painter->restore();
            Qt::Alignment ta;
            switch( opt.displayPosition ) {
            case KDGantt::StyleOptionGanttItem::Left: ta = Qt::AlignLeft; break;
            case KDGantt::StyleOptionGanttItem::Right: ta = Qt::AlignRight; break;
            case KDGantt::StyleOptionGanttItem::Center: ta = Qt::AlignCenter; break;
            }
            painter->drawText( boundingRect, ta, txt );
        }
        break;
    case KDGantt::TypeSummary:
        if ( idx.data( Role::ObjectType ).toInt() == OT_Resource ) {
            paintResourceItem( painter, opt, idx );
        } else if ( itemRect.isValid() ) {
            pw-=1;
            const QRectF r = QRectF( opt.itemRect ).adjusted( -pw, -pw, pw, pw );
            QPainterPath path;
            const qreal deltaY = r.height()/2.;
            const qreal deltaX = qMin( r.width()/qreal(2), deltaY );
            path.moveTo( r.topLeft() );
            path.lineTo( r.topRight() );
            path.lineTo( QPointF( r.right(), r.top() + 2.*deltaY ) );
            //path.lineTo( QPointF( r.right()-3./2.*delta, r.top() + delta ) );
            path.quadTo( QPointF( r.right()-.5*deltaX, r.top() + deltaY ), QPointF( r.right()-2.*deltaX, r.top() + deltaY ) );
            //path.lineTo( QPointF( r.left()+3./2.*delta, r.top() + delta ) );
            path.lineTo( QPointF( r.left() + 2.*deltaX, r.top() + deltaY ) );
            path.quadTo( QPointF( r.left()+.5*deltaX, r.top() + deltaY ), QPointF( r.left(), r.top() + 2.*deltaY ) );
            path.closeSubpath();
            painter->setBrushOrigin( itemRect.topLeft() );
            painter->save();
            painter->translate( 0.5, 0.5 );
            painter->drawPath( path );
            painter->restore();
            Qt::Alignment ta;
            switch( opt.displayPosition ) {
            case KDGantt::StyleOptionGanttItem::Left: ta = Qt::AlignLeft; break;
            case KDGantt::StyleOptionGanttItem::Right: ta = Qt::AlignRight; break;
            case KDGantt::StyleOptionGanttItem::Center: ta = Qt::AlignCenter; break;
            }
            painter->drawText( boundingRect, ta | Qt::AlignVCenter, txt );
        }
        break;
    case KDGantt::TypeEvent: /* TODO */
        if ( opt.boundingRect.isValid() ) {
            const qreal pw = painter->pen().width() / 2. - 1;
            const QRectF r = QRectF( opt.rect ).adjusted( -pw, -pw, pw, pw );
            QPainterPath path;
            const qreal delta = static_cast< int >( r.height() / 2 );
            path.moveTo( delta, 0. );
            path.lineTo( 2.*delta, delta );
            path.lineTo( delta, 2.*delta );
            path.lineTo( 0., delta );
            path.closeSubpath();
            painter->save();
            painter->translate( r.topLeft() );
            painter->translate( 0.5, 0.5 );
            painter->drawPath( path );
            painter->restore();
            Qt::Alignment ta;
            switch( opt.displayPosition ) {
            case KDGantt::StyleOptionGanttItem::Left: ta = Qt::AlignLeft; break;
            case KDGantt::StyleOptionGanttItem::Right: ta = Qt::AlignRight; break;
            case KDGantt::StyleOptionGanttItem::Center: ta = Qt::AlignCenter; break;
            }
            painter->drawText( boundingRect, ta | Qt::AlignVCenter, txt );
        }
        break;
    default:
        break;
    }
    painter->restore();
}

void ResourceGanttItemDelegate::paintResourceItem( QPainter* painter, const KDGantt::StyleOptionGanttItem& opt, const QModelIndex& idx )
{
    if ( ! opt.itemRect.isValid() ) {
        return;
    }
    qreal pw = painter->pen().width()/2.;
    pw-=1;
    QRectF r = opt.itemRect;
    QRectF boundingRect = opt.boundingRect;
    boundingRect.setY( r.y() );
    boundingRect.setHeight( r.height() );
    r.translate( 0., r.height()/12. );
    r.setHeight( 5.*r.height()/6. );
    painter->setBrushOrigin( opt.itemRect.topLeft() );

    qreal x0 = opt.grid->mapToChart( idx.data( KDGantt::StartTimeRole ).toDateTime() );

    Appointment *external = static_cast<Appointment*>( idx.data( Role::ExternalAppointments ).value<void*>() );
    Appointment *internal = static_cast<Appointment*>( idx.data( Role::InternalAppointments ).value<void*>() );
    int rl = idx.data( Role::Maximum ).toInt(); //TODO check calendar
    Appointment tot = *external + *internal;
    painter->save();
    // TODO check load vs units properly, it's not as simple as below!
    foreach ( const AppointmentInterval &i, tot.intervals().map() ) {
        int il = i.load();
        QString txt = KGlobal::locale()->formatNumber( (double)il / (double)rl, 1 );
        QPen pen = painter->pen();
        if ( il > rl ) {
            painter->setBrush( m_overloadBrush );
            pen.setColor( Qt::white );
        } else if ( il < rl ) {
            painter->setBrush( m_underloadBrush );
        } else {
            painter->setBrush( defaultBrush( KDGantt::TypeTask ) );
        }
        qreal v1 = opt.grid->mapToChart( i.startTime() );
        qreal v2 = opt.grid->mapToChart( i.endTime() );
        QRectF rr( v1 - x0, r.y(), v2 - v1, r.height() );
        painter->drawRect( rr );
        if ( painter->boundingRect( rr, Qt::AlignCenter, txt ).width() < rr.width() ) {
            QPen pn = painter->pen();
            painter->setPen( pen );
            painter->drawText( rr, Qt::AlignCenter, txt );
            painter->setPen( pn );
        }
    }

    painter->restore();
    Qt::Alignment ta;
    switch( opt.displayPosition ) {
    case KDGantt::StyleOptionGanttItem::Left: ta = Qt::AlignLeft; break;
    case KDGantt::StyleOptionGanttItem::Right: ta = Qt::AlignRight; break;
    case KDGantt::StyleOptionGanttItem::Center: ta = Qt::AlignCenter; break;
    }
    painter->drawText( boundingRect, ta, opt.text );
}

} // namespace KPlato

#include "kptganttitemdelegate.moc"
