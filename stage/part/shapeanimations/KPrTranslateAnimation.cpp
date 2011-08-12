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

#include "KPrTranslateAnimation.h"

#include <QPainter>

#include <KoCanvasBase.h>
#include <KoViewConverter.h>

#include "KPrAnimationData.h"

KPrTranslateAnimation::KPrTranslateAnimation( KoShape * shape, int step, Type type )
: KPrShapeAnimationOld( shape, step, type )
{
}

KPrTranslateAnimation::~KPrTranslateAnimation()
{
}

bool KPrTranslateAnimation::animate( QPainter &painter, const KoViewConverter &converter, KPrAnimationData * animationData )
{
    KPrAnimationDataTranslate * data = dynamic_cast<KPrAnimationDataTranslate *>( animationData );
    Q_ASSERT( data );
    painter.translate( converter.documentToView( data->m_translate ) );
    return data->m_finished;
}

void KPrTranslateAnimation::animateRect( QRectF & rect, KPrAnimationData * animationData )
{
    KPrAnimationDataTranslate * data = dynamic_cast<KPrAnimationDataTranslate *>( animationData );
    Q_ASSERT( data );
    rect.translate( data->m_translate );
}

void KPrTranslateAnimation::next( int currentTime, KPrAnimationData * animationData )
{
    KPrAnimationDataTranslate * data = dynamic_cast<KPrAnimationDataTranslate *>( animationData );
    Q_ASSERT( data );
    data->m_canvas->updateCanvas( data->m_boundingRect.translated( data->m_translate ) );
    data->m_translate.setX( data->m_timeLine.frameForTime( currentTime ) / TIMEFACTOR );
    data->m_canvas->updateCanvas( data->m_boundingRect.translated( data->m_translate ) );
    data->m_finished = data->m_timeLine.frameForTime( currentTime ) == data->m_timeLine.endFrame();
}

void KPrTranslateAnimation::finish( KPrAnimationData * animationData )
{
    KPrAnimationDataTranslate * data = dynamic_cast<KPrAnimationDataTranslate *>( animationData );
    Q_ASSERT( data );
    data->m_canvas->updateCanvas( data->m_boundingRect.translated( data->m_translate ) );
    data->m_translate.setX( data->m_timeLine.endFrame() / TIMEFACTOR );
    data->m_canvas->updateCanvas( data->m_boundingRect );
}
