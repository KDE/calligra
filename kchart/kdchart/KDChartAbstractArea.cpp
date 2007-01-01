/****************************************************************************
 ** Copyright (C) 2006 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KD Chart library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KD Chart licenses may use this file in
 ** accordance with the KD Chart Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.kdab.net/kdchart for
 **   information about KDChart Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/

#include "KDChartAbstractArea.h"
#include "KDChartAbstractArea_p.h"

#include <qglobal.h>

#include <QPainter>
#include <QRect>

#include <KDABLibFakes>


using namespace KDChart;

AbstractArea::Private::Private() :
    AbstractAreaBase::Private()
{
    // this bloc left empty intentionally
}


AbstractArea::Private::~Private()
{
    // this bloc left empty intentionally
}


AbstractArea::AbstractArea()
    : QObject()
    , KDChart::AbstractAreaBase()
    , KDChart::AbstractLayoutItem()
{
    // this bloc left empty intentionally
}

AbstractArea::~AbstractArea()
{
    // this bloc left empty intentionally
}


void AbstractArea::init()
{
    // this bloc left empty intentionally
}

void AbstractArea::paintIntoRect( QPainter& painter, const QRect& rect )
{
    const QRect oldGeometry( geometry() );
    if( oldGeometry != rect )
        setGeometry( rect );
    painter.translate( rect.left(), rect.top() );
    paintAll( painter );
    painter.translate( -rect.left(), -rect.top() );
    if( oldGeometry != rect )
        setGeometry( oldGeometry );
}

void AbstractArea::paintAll( QPainter& painter )
{
    // Paint the background and frame
    paintBackground( painter, geometry() );
    paintFrame(      painter, geometry() );

    // temporarily adjust the widget size, to be sure all content gets calculated
    // to fit into the inner rectangle
    const QRect oldGeometry( areaGeometry()  );
    QRect inner( innerRect() );
    inner.moveTo(
        oldGeometry.left() + inner.left(),
        oldGeometry.top()  + inner.top() );
    const bool needAdjustGeometry = oldGeometry != inner;
    if( needAdjustGeometry )
        setGeometry( inner );
    paint( &painter );
    if( needAdjustGeometry )
        setGeometry( oldGeometry );
    //qDebug() << "AbstractAreaWidget::paintAll() done.";
}

QRect AbstractArea::areaGeometry() const
{
    return geometry();
}

void AbstractArea::positionHasChanged()
{
    emit positionChanged( this );
}

