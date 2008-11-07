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
    showCriticalPath( false ),
    showCriticalTasks( false ),
    showAppointments( false )
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
    QString txt;
    if ( showTaskName ) {
        txt = data( idx, NodeModel::NodeName, Qt::DisplayRole ).toString();
    }
    if ( type == KDGantt::TypeTask && showResources ) {
        if ( ! txt.isEmpty() ) {
            txt += ' ';
        }
        txt += '(' + data( idx, NodeModel::NodeAssigments, Qt::DisplayRole ).toString() + ')';
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

    if (  typ == KDGantt::TypeEvent ) {
        itemRect = QRectF( itemRect.left()-itemRect.height()/2., itemRect.top(), itemRect.height(), itemRect.height() );
    }

    qreal left = itemRect.left();
    qreal width = itemRect.width();
    switch ( opt.displayPosition ) {
        case KDGantt::StyleOptionGanttItem::Left:
            left -= tw;
            width += tw;
            break;
        case KDGantt::StyleOptionGanttItem::Right:
            width += tw;
            break;
        case KDGantt::StyleOptionGanttItem::Center:
            break;
    }
    width += dw;
    return KDGantt::Span( left, width );
}

/*! Paints the gantt item \a idx using \a painter and \a opt
 */
void GanttItemDelegate::paintGanttItem( QPainter* painter, const KDGantt::StyleOptionGanttItem& opt, const QModelIndex& idx )
{
    if ( !idx.isValid() ) return;
    
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
            const qreal deltaX = qMin( r.width()/2., deltaY );
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
