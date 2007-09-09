/* This file is part of the KDE project
 * Copyright ( C ) 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
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

#include "KPrAnimationMoveAppear.h"

#include <QPainter>
#include <KoCanvasBase.h>
#include <KoShape.h>
#include <KoViewConverter.h>
#include <kdebug.h>

#define TIMEFACTOR 1000.0

KPrAnimationMoveAppear::KPrAnimationMoveAppear( KoShape * shape, int step )
: KPrShapeAnimation( shape, step, Appear )
{
    // TODO use bounding rect + shadow
    m_shapeRect = m_shape->boundingRect();
    double x = m_shapeRect.x() + m_shapeRect.width();
    m_translate = QPointF( -x, 0 );
    m_timeLine.setFrameRange( int( -x * TIMEFACTOR ), 0 );
}

KPrAnimationMoveAppear::~KPrAnimationMoveAppear()
{
}

bool KPrAnimationMoveAppear::animate( QPainter &painter, const KoViewConverter &converter )
{
    painter.translate( converter.documentToView( m_translate ) );
    return m_finished;
}

void KPrAnimationMoveAppear::animateRect( QRectF & rect )
{
    rect.translate( m_translate );
}

void KPrAnimationMoveAppear::next( int currentTime, KoCanvasBase * canvas )
{
    canvas->updateCanvas( m_shapeRect.translated( m_translate ) );
    m_translate.setX( m_timeLine.frameForTime( currentTime ) / TIMEFACTOR );
    canvas->updateCanvas( m_shapeRect.translated( m_translate ) );
    m_finished = m_translate.x() == 0;
    kDebug() << currentTime << m_translate << m_finished << m_shapeRect;
}

void KPrAnimationMoveAppear::finish( KoCanvasBase * canvas )
{
    canvas->updateCanvas( m_shapeRect.translated( m_translate ) );
    m_translate.setX( 0 );
    canvas->updateCanvas( m_shapeRect );
}
