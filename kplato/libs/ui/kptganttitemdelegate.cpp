/* This file is part of the KDE project
  Copyright (C) 2008 Dag Andersen <danders@get2net.dk>

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

#include <kdebug.h>

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
    showNegativeFloat( false ), // NOTE: atm for test, activate when the ui can be changed
    showCriticalPath( false ),
    showCriticalTasks( false ),
    showAppointments( false ),
    showTimeConstraint( false ) // NOTE: atm for test, activate when the ui can be changed
{
    QLinearGradient b( 0., 0., 0., QApplication::fontMetrics().height() );
    b.setColorAt( 0., Qt::red );
    b.setColorAt( 1., Qt::darkRed );
    m_criticalBrush = QBrush( b );
}

QVariant GanttItemDelegate::data( const QModelIndex& idx, int column, int role ) const
{
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
    //kDebug()<<txt;
    return txt;
}

int GanttItemDelegate::itemFloatWidth( const KDGantt::StyleOptionGanttItem& opt, const QModelIndex& idx ) const
{
    double fl = data( idx, NodeModel::NodePositiveFloat, Qt::EditRole ).toDouble();
    if ( fl == 0.0 ) {
        return 0.0;
    }
    QDateTime et = data( idx, NodeModel::NodeEndTime, Qt::EditRole ).toDateTime();
    if ( ! et.isValid() ) {
        return 0.0;
    }
    QDateTime dt = ( DateTime( KDateTime( et ) ) + Duration( fl, Duration::Unit_h ) ).dateTime();
    int dw = 0;
    qreal v1 = 0.0;
    if ( dt.isValid() ) {
        v1 = opt.grid->mapToChart( dt );
        qreal v2 = opt.grid->mapToChart( et );
        if ( v1 > 0.0 && v2 >= 0.0 && v1 > v2 ) {
            dw = (int)( v1 - v2 );
        }
    }
    return dw;
}

int GanttItemDelegate::itemNegativeFloatWidth( const KDGantt::StyleOptionGanttItem& opt, const QModelIndex& idx ) const
{
    double fl = data( idx, NodeModel::NodeNegativeFloat, Qt::EditRole ).toDouble();
    if ( fl == 0.0 ) {
        return 0;
    }
    int dw = 0;
    if ( hasStartConstraint( idx ) ) {
        QDateTime st = data( idx, NodeModel::NodeStartTime, Qt::EditRole ).toDateTime();
        if ( ! st.isValid() ) {
            return 0;
        }
        QDateTime dt = ( DateTime( KDateTime( st ) ) - Duration( fl, Duration::Unit_h ) ).dateTime();
        if ( dt.isValid() ) {
            qreal v1 = opt.grid->mapToChart( dt );
            qreal v2 = opt.grid->mapToChart( st );
            dw = (int)( v2 + opt.itemRect.right() - v1 ); // relative end
        }
    } else if ( hasEndConstraint( idx ) ) {
        QDateTime et = data( idx, NodeModel::NodeEndTime, Qt::EditRole ).toDateTime();
        if ( ! et.isValid() ) {
            return 0;
        }
        QDateTime dt = ( DateTime( KDateTime( et ) ) - Duration( fl, Duration::Unit_h ) ).dateTime();
        if ( dt.isValid() ) {
            qreal v1 = opt.grid->mapToChart( dt );
            qreal v2 = opt.grid->mapToChart( et );
            dw = (int)( v2 - v1 ); // relative end
        }
    }
    //kDebug()<<data( idx, NodeModel::NodeName ).toString()<<data( idx, NodeModel::NodeConstraint ).toString()<<data( idx, NodeModel::NodeNegativeFloat  ).toString()<<dw;
    return dw;
}

bool GanttItemDelegate::hasStartConstraint( const QModelIndex& idx ) const
{
    //kDebug()<<data( idx, NodeModel::NodeName ).toString()<<data( idx, NodeModel::NodeConstraint ).toString()<<data( idx, NodeModel::NodeConstraint, Qt::EditRole ).toInt();
    switch ( data( idx, NodeModel::NodeConstraint, Qt::EditRole ).toInt() ) {
        case Node::FixedInterval:
        case Node::StartNotEarlier:
        case Node::MustStartOn: return true;
        default: break;
    }
    return false;
}

int GanttItemDelegate::itemStartConstraintWidth( const KDGantt::StyleOptionGanttItem& opt, const QModelIndex& idx ) const
{
    QDateTime dt;
    if ( hasStartConstraint( idx ) ) {
        dt = data( idx, NodeModel::NodeConstraintStart, Qt::EditRole ).toDateTime();
    }
    if ( ! dt.isValid() ) {
        return 0;
    }
    QDateTime st = data( idx, NodeModel::NodeStartTime, Qt::EditRole ).toDateTime();
    if ( ! st.isValid() ) {
        return 0;
    }

    int dw = 0;
    qreal sc = opt.grid->mapToChart( dt );
    qreal pos = opt.grid->mapToChart( st );
    dw = (int)( pos - sc ); // usually >= 0
    //kDebug()<<data( idx, NodeModel::NodeName ).toString()<<dw;
    return dw;
}

bool GanttItemDelegate::hasEndConstraint( const QModelIndex& idx ) const
{
    //kDebug()<<data( idx, NodeModel::NodeName ).toString()<<data( idx, NodeModel::NodeConstraint ).toString()<<data( idx, NodeModel::NodeConstraint, Qt::EditRole ).toInt();
    switch ( data( idx, NodeModel::NodeConstraint, Qt::EditRole ).toInt() ) {
        case Node::FixedInterval:
        case Node::FinishNotLater:
        case Node::MustFinishOn: return true;
        default: break;
    }
    return false;
}

int GanttItemDelegate::itemEndConstraintWidth( const KDGantt::StyleOptionGanttItem& opt, const QModelIndex& idx ) const
{
    QDateTime dt;
    if ( hasEndConstraint( idx ) ) {
        dt = data( idx, NodeModel::NodeConstraintEnd, Qt::EditRole ).toDateTime();
    }
    if ( ! dt.isValid() ) {
        return 0;
    }
    QDateTime et = data( idx, NodeModel::NodeEndTime, Qt::EditRole ).toDateTime();
    if ( ! et.isValid() ) {
        return 0;
    }

    int dw = 0;
    qreal ec = opt.grid->mapToChart( dt );
    qreal pos = opt.grid->mapToChart( et );
    dw = (int)( ec - pos ); // usually >= 0
    //kDebug()<<data( idx, NodeModel::NodeName ).toString()<<dw;
    return dw;
}

KDGantt::Span GanttItemDelegate::itemBoundingSpan( const KDGantt::StyleOptionGanttItem& opt, const QModelIndex& idx ) const
{
    //kDebug()<<opt<<idx;
    if ( !idx.isValid() ) return KDGantt::Span();

    QRectF itemRect = opt.itemRect;
    
    int typ = idx.model()->data( idx, KDGantt::ItemTypeRole ).toInt();
    QString txt = itemText( idx, typ );
    int tw = 0;
    if ( ! txt.isEmpty() ) {
        tw = opt.fontMetrics.width( txt );
        tw += static_cast<int>( itemRect.height()/1.5 );
    }

    int dw = 0;
    if ( showPositiveFloat ) {
        dw = itemFloatWidth( opt, idx );
    }
    int nfw = 0;
    if ( showNegativeFloat ) {
        nfw = itemNegativeFloatWidth( opt, idx ) - itemRect.width(); // relative start
    }
    int cwstart = 0;
    int cwend = 0;
    if ( showTimeConstraint ) {
        if ( hasStartConstraint( idx ) ) {
            cwstart = itemStartConstraintWidth( opt, idx );
            if ( cwstart >= 0 ) {
                cwstart += (int)(itemRect.height()/2.);
            }
        }
        if ( hasEndConstraint( idx ) ) {
            cwend = itemEndConstraintWidth( opt, idx );
            if ( cwend >= 0 ) {
                cwend += (int)(itemRect.height()/2.);
            }
        }
        if ( cwend < 0 && cwstart < 0 ) {
            int v = cwstart;
            cwstart = -cwend;
            cwend = - v;
        } else if ( cwend < 0 ) {
            cwstart = qMax( cwstart, -cwend - (int)(itemRect.right()) );
        } else if ( cwstart < 0 ) {
            cwend = qMax(  cwend, -cwstart - (int)(itemRect.right()) );
        }
    }
    if ( idx.model()->data( idx, GanttItemModel::SpecialItemTypeRole ).toInt() > 0 ) {
        itemRect = QRectF( itemRect.left()-itemRect.height()/4., itemRect.top(), itemRect.height()/2., itemRect.height() );
    } else if (  typ == KDGantt::TypeEvent ) {
        itemRect = QRectF( itemRect.left()-itemRect.height()/2., itemRect.top(), itemRect.height(), itemRect.height() );
    }

    qreal left = itemRect.left();
    qreal width = itemRect.width();
    switch ( opt.displayPosition ) {
        case KDGantt::StyleOptionGanttItem::Left:
            left -= qMax( tw, qMax( cwstart, nfw ) );
            width += qMax( tw, qMax( cwstart, nfw ) ) + qMax( dw, cwend );
            break;
        case KDGantt::StyleOptionGanttItem::Right:
            left -= qMax( cwstart, nfw );
            width += qMax( cwstart, nfw );
            width += qMax( tw, qMax( dw, cwend ) );
            break;
        case KDGantt::StyleOptionGanttItem::Center:
            left -= qMax( cwstart, nfw );
            width += qMax( cwstart, nfw );
            width += qMax( dw, cwend );
            break;
    }
    return KDGantt::Span( left, width );
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
    QRectF boundingRect = opt.boundingRect;
    boundingRect.setY( itemRect.y() );
    boundingRect.setHeight( itemRect.height() );
    
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
    painter->setBrush( defaultBrush( typ ) );

    qreal pw = painter->pen().width()/2.;
    switch( typ ) {
    case KDGantt::TypeTask:
        if ( itemRect.isValid() ) {
            pw-=1;
            QRectF r = itemRect;
            r.translate( 0., r.height()/6. );
            r.setHeight( 2.*r.height()/3. );
            painter->setBrushOrigin( itemRect.topLeft() );
            painter->save();
            painter->translate( 0.5, 0.5 );
            if ( showPositiveFloat ) {
                int dw = itemFloatWidth( opt, idx );
                if ( dw > 0 ) {
                    qreal h = r.height();
                    QRectF cr( r.right(), r.bottom(), dw, -h/4 );
                    painter->fillRect( cr, painter->pen().brush() );
                }
            }
            if ( showNegativeFloat ) {
                int dw = itemNegativeFloatWidth( opt, idx );
                if ( dw > 0 ) {
                    QRectF cr;
                    qreal h = r.height()/4.;
                    if ( hasStartConstraint( idx ) ) {
                        cr = QRectF( r.left(), r.bottom(), - dw + r.width(), -h );
                        painter->fillRect( cr, painter->pen().brush() );
                    } else if ( hasEndConstraint( idx ) ) {
                        cr = QRectF( r.left(), r.bottom(), -dw, -h );
                        painter->fillRect( cr, painter->pen().brush() );
                    }
                    //kDebug()<<data( idx, NodeModel::NodeName ).toString()<<data( idx, NodeModel::NodeConstraint ).toString()<<cr<<r<<boundingRect;
                }
            }
            if ( showTimeConstraint ) {
                //kDebug()<<data( idx, NodeModel::NodeName ).toString()<<data( idx, NodeModel::NodeConstraint ).toString()<<r<<boundingRect;
                painter->save();
                painter->setBrush( QBrush( Qt::darkGray ) );
                painter->setPen( Qt::black );
                qreal h = r.height()/2.;
                if ( hasStartConstraint( idx ) ) {
                    int dw = itemStartConstraintWidth( opt, idx ) + h;
                    QRectF cr( r.left()-dw, r.top() + h/2., h, h );
                    QPainterPath p( cr.topLeft() );
                    p.lineTo( cr.bottomLeft() );
                    p.lineTo( cr.right(), cr.top() + cr.height()/2. );
                    p.closeSubpath();
                    painter->drawPath( p );
                    if ( data( idx, NodeModel::NodeConstraint, Qt::EditRole ).toInt() != Node::StartNotEarlier ) {
                        painter->fillPath( p, QBrush( Qt::black ) );
                    }
                }
                if ( hasEndConstraint( idx ) ) {
                    int dw = itemEndConstraintWidth( opt, idx );
                    QRectF cr( r.right()+dw, r.top() + h/2., h, h );
                    QPainterPath p( cr.topRight() );
                    p.lineTo( cr.bottomRight() );
                    p.lineTo( cr.left(), cr.top() + cr.height()/2. );
                    p.closeSubpath();
                    painter->drawPath( p );
                    if ( data( idx, NodeModel::NodeConstraint, Qt::EditRole ).toInt() != Node::FinishNotLater ) {
                        painter->fillPath( p, QBrush( Qt::black ) );
                    }
                }
                painter->restore();
            }
            bool critical = false;
            if ( showCriticalTasks ) {
                critical = idx.model()->index( idx.row(), NodeModel::NodeCritical, idx.parent() ).data( Qt::DisplayRole ).toBool();
            }
            if ( ! critical && showCriticalPath ) {
                critical = idx.model()->index( idx.row(), NodeModel::NodeCriticalPath, idx.parent() ).data( Qt::DisplayRole ).toBool();
            }
            if ( critical ) {
                painter->setBrush( m_criticalBrush );
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
        //kDebug() << opt.boundingRect << opt.itemRect;
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
                int dw = itemFloatWidth( opt, idx );
                if ( dw > 0 ) {
                    qreal h = ir.height();
                    QRectF cr( ir.right(), ir.bottom(), dw, -h/4 );
                    painter->fillRect( cr, painter->pen().brush() );
                }
            }
            painter->restore();

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
void GanttItemDelegate::paintSpecialItem( QPainter* painter, const KDGantt::StyleOptionGanttItem& opt, const QModelIndex& idx, int typ )
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
        //kDebug()<<data( c.startIndex(), NodeModel::NodeName ).toString()<<data( c.endIndex(), NodeModel::NodeName ).toString()<<"critical path";
        QPen pen( Qt::red );
        c.setData( KDGantt::Constraint::ValidConstraintPen, pen );
        // FIXME How to make sure it's not obscured by other constraints?
    }
    KDGantt::ItemDelegate::paintConstraintItem( painter, opt, start, end, c );
}

} // namespace KPlato

#include "kptganttitemdelegate.moc"
