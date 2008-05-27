/****************************************************************************
 ** Copyright (C) 2001-2006 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KD Gantt library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KD Gantt licenses may use this file in
 ** accordance with the KD Gantt Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.kdab.net/kdgantt for
 **   information about KD Gantt Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/
#include "kdganttconstraintgraphicsitem.h"
#include "kdganttconstraintmodel.h"
#include "kdganttgraphicsscene.h"
#include "kdganttitemdelegate.h"
#include "kdganttsummaryhandlingproxymodel.h"

#include <QPainter>
#include <QDebug>

using namespace KDGantt;

/*!\class KDGantt::ConstraintGraphicsItem
 * \internal
 */

ConstraintGraphicsItem::ConstraintGraphicsItem( const Constraint& c, QGraphicsItem* parent, GraphicsScene* scene )
    : QGraphicsItem( parent, scene ),  m_constraint( c )
{
    qDebug() << "ConstraintGraphicsItem::ConstraintGraphicsItem()";
    setPos( QPointF( 0., 0. ) );
    setAcceptsHoverEvents( false );
    setAcceptedMouseButtons( Qt::NoButton );
    setZValue( 10. );
}

ConstraintGraphicsItem::~ConstraintGraphicsItem()
{
}

int ConstraintGraphicsItem::type() const
{
    return Type;
}

GraphicsScene* ConstraintGraphicsItem::scene() const
{
    return qobject_cast<GraphicsScene*>( QGraphicsItem::scene() );
}

Constraint ConstraintGraphicsItem::proxyConstraint() const
{
    return Constraint( scene()->summaryHandlingModel()->mapFromSource( m_constraint.startIndex() ),
                       scene()->summaryHandlingModel()->mapFromSource( m_constraint.endIndex() ),
                       m_constraint.type() );
}

QRectF ConstraintGraphicsItem::boundingRect() const
{
    return scene()->itemDelegate()->constraintBoundingRect( m_start, m_end, m_constraint );
}

void ConstraintGraphicsItem::paint( QPainter* painter, const QStyleOptionGraphicsItem* option,
                                    QWidget* widget )
{
    Q_UNUSED( widget );

    scene()->itemDelegate()->paintConstraintItem( painter, *option, m_start, m_end, m_constraint );
}

QString ConstraintGraphicsItem::ganttToolTip() const
{
    return m_constraint.data( Qt::ToolTipRole ).toString();
}

void ConstraintGraphicsItem::setStart( const QPointF& start )
{
    prepareGeometryChange();
    m_start = start;
    update();
}

void ConstraintGraphicsItem::setEnd( const QPointF& end )
{
    prepareGeometryChange();
    m_end = end;
    update();
}

void ConstraintGraphicsItem::updateItem( const QPointF& start,const QPointF& end )
{
    qDebug() << "ConstraintGraphicsItem::updateItem("<<start<<end<<")";
    setStart( start );
    setEnd( end );
}

