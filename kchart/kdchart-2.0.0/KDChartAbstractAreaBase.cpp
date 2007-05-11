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

#include "KDChartAbstractAreaBase.h"
#include "KDChartAbstractAreaBase_p.h"
#include <KDChartBackgroundAttributes.h>
#include <KDChartFrameAttributes.h>
#include <KDChartTextAttributes.h>
#include "KDChartPainterSaver_p.h"
#include <QPainter>

#include <KDABLibFakes>


using namespace KDChart;

AbstractAreaBase::Private::Private() :
    visible( true )
    // PENDING(khz) dockingPointToPadding?, alignToDockingPoint?
{
    init();
}


AbstractAreaBase::Private::~Private() {}


void AbstractAreaBase::Private::init()
{
}




AbstractAreaBase::AbstractAreaBase() :
    _d( new Private() )
{
}

AbstractAreaBase::~AbstractAreaBase()
{
    delete _d; _d = 0;
}


void AbstractAreaBase::init()
{
}


#define d d_func()

void AbstractAreaBase::alignToReferencePoint( const RelativePosition& position )
{
    Q_UNUSED( position );
    // PENDING(kalle) FIXME
    qWarning( "Sorry, not implemented: void AbstractAreaBase::alignToReferencePoint( const RelativePosition& position )" );
}

void AbstractAreaBase::setFrameAttributes( const FrameAttributes &a )
{
    d->frameAttributes = a;
}

FrameAttributes AbstractAreaBase::frameAttributes() const
{
    return d->frameAttributes;
}

void AbstractAreaBase::setBackgroundAttributes( const BackgroundAttributes &a )
{
    d->backgroundAttributes = a;
}

BackgroundAttributes AbstractAreaBase::backgroundAttributes() const
{
    return d->backgroundAttributes;
}


void AbstractAreaBase::paintBackground( QPainter& painter, const QRect& rect )
{
    Q_ASSERT_X ( d != 0, "AbstractAreaBase::paintBackground()",
                "Private class was not initialized!" );

#define attributes d->backgroundAttributes

    if( !attributes.isVisible() ) return;

    /* first draw the brush (may contain a pixmap)*/
    if( Qt::NoBrush != attributes.brush().style() ) {
        KDChart::PainterSaver painterSaver( &painter );
        painter.setPen( Qt::NoPen );
        const QPointF newTopLeft( painter.deviceMatrix().map( rect.topLeft() ) );
        painter.setBrushOrigin( newTopLeft );
        painter.setBrush( attributes.brush() );
        painter.drawRect( rect );
    }
    /* next draw the backPixmap over the brush */
    if( !attributes.pixmap().isNull() &&
        attributes.pixmapMode() != BackgroundAttributes::BackgroundPixmapModeNone ) {
        QPointF ol = rect.topLeft();
        if( BackgroundAttributes::BackgroundPixmapModeCentered == attributes.pixmapMode() )
        {
            ol.setX( rect.center().x() - attributes.pixmap().width() / 2 );
            ol.setY( rect.center().y() - attributes.pixmap().height()/ 2 );
            painter.drawPixmap( ol, attributes.pixmap() );
        } else {
            QMatrix m;
            double zW = (double)rect.width()  / (double)attributes.pixmap().width();
            double zH = (double)rect.height() / (double)attributes.pixmap().height();
            switch( attributes.pixmapMode() ) {
            case BackgroundAttributes::BackgroundPixmapModeScaled:
            {
                double z;
                z = qMin( zW, zH );
                m.scale( z, z );
            }
            break;
            case BackgroundAttributes::BackgroundPixmapModeStretched:
                m.scale( zW, zH );
                break;
            default:
                ; // Cannot happen, previously checked
            }
            QPixmap pm = attributes.pixmap().transformed( m );
            ol.setX( rect.center().x() - pm.width() / 2 );
            ol.setY( rect.center().y() - pm.height()/ 2 );
            painter.drawPixmap( ol, pm );
        }
    }
#undef attributes
}


void AbstractAreaBase::paintFrame( QPainter& painter, const QRect& rect )
{
    Q_ASSERT_X ( d != 0, "AbstractAreaBase::paintFrame()",
                "Private class was not initialized!" );

#define attributes d->frameAttributes

    if( !attributes.isVisible() ) return;

    const QPen oldPen( painter.pen() );
    painter.setPen( attributes.pen() );
    painter.drawRect( rect );
    painter.setPen( oldPen );
#undef attributes
}


QRect AbstractAreaBase::innerRect() const
{
    Q_ASSERT_X ( d != 0, "AbstractAreaBase::innerRect()",
                "Private class was not initialized!" );

    const int padding
        = d->frameAttributes.isVisible()
        ? qMax( d->frameAttributes.padding(), 0 ) : 0;
    return
        QRect( QPoint(0,0), areaGeometry().size() )
            .adjusted( padding, padding, -padding, -padding );
}

void AbstractAreaBase::positionHasChanged()
{
    // this bloc left empty intentionally
}

