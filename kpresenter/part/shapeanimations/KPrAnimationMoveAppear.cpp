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

#include "KPrAnimationData.h"

#define TIMEFACTOR 1000.0

KPrAnimationMoveAppear::KPrAnimationMoveAppear( KoShape * shape, int step )
: KPrTranslateAnimation( shape, step, Appear )
{
}

KPrAnimationMoveAppear::~KPrAnimationMoveAppear()
{
}

KPrAnimationData * KPrAnimationMoveAppear::animationData( KoCanvasBase * canvas, KoShapeManager * shapeManager )
{
    KPrAnimationDataTranslate * data = new KPrAnimationDataTranslate( canvas, shapeManager, m_shape->boundingRect() );
    // TODO use bounding rect + shadow
    double x = data->m_boundingRect.x() + data->m_boundingRect.width();
    data->m_translate = QPointF( -x, 0 );
    data->m_timeLine.setDuration( 5000 );
    data->m_timeLine.setCurveShape( QTimeLine::LinearCurve );
    data->m_timeLine.setFrameRange( int( -x * TIMEFACTOR ), 0 );
    return data;
}

void KPrAnimationMoveAppear::next( int currentTime, KPrAnimationData * animationData )
{
    KPrAnimationDataTranslate * data = dynamic_cast<KPrAnimationDataTranslate *>( animationData );
    Q_ASSERT( data );
    data->m_canvas->updateCanvas( data->m_boundingRect.translated( data->m_translate ) );
    data->m_translate.setX( data->m_timeLine.frameForTime( currentTime ) / TIMEFACTOR );
    data->m_canvas->updateCanvas( data->m_boundingRect.translated( data->m_translate ) );
    data->m_finished = data->m_translate.x() == 0;
    kDebug() << currentTime << data->m_translate << data->m_finished << data->m_boundingRect;
}

void KPrAnimationMoveAppear::finish( KPrAnimationData * animationData )
{
    KPrAnimationDataTranslate * data = dynamic_cast<KPrAnimationDataTranslate *>( animationData );
    Q_ASSERT( data );
    data->m_canvas->updateCanvas( data->m_boundingRect.translated( data->m_translate ) );
    data->m_translate.setX( 0 );
    data->m_canvas->updateCanvas( data->m_boundingRect );
}
