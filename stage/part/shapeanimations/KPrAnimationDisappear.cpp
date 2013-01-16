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

#include "KPrAnimationDisappear.h"

#include <QPainter>
#include <KoCanvasBase.h>
#include <KoShape.h>
#include <KoViewConverter.h>

#include "KPrAnimationData.h"

KPrAnimationDisappear::KPrAnimationDisappear( KoShape * shape, int step )
: KPrShapeAnimationOld( shape, step, Disappear )
{
}

KPrAnimationDisappear::~KPrAnimationDisappear()
{
}

KPrAnimationData * KPrAnimationDisappear::animationData( KoCanvasBase * canvas, KoShapeManager * shapeManager, const QRectF & pageRect )
{
    Q_UNUSED( pageRect );
    KPrAnimationDataTranslate * data = new KPrAnimationDataTranslate( canvas, shapeManager, m_shape->boundingRect() );
    double x = data->m_boundingRect.x() + data->m_boundingRect.width() + 2.0;
    data->m_translate = QPointF( -x, 0 );
    data->m_timeLine.setDuration( 1 );
    return data;
}

bool KPrAnimationDisappear::animate( QPainter &painter, const KoViewConverter &converter, KPrAnimationData * animationData  )
{
    KPrAnimationDataTranslate * data = dynamic_cast<KPrAnimationDataTranslate *>( animationData );
    Q_ASSERT( data );
    painter.translate( converter.documentToView( data->m_translate ) );
    return data->m_finished;
}

void KPrAnimationDisappear::animateRect( QRectF & rect, KPrAnimationData * animationData )
{
    KPrAnimationDataTranslate * data = dynamic_cast<KPrAnimationDataTranslate *>( animationData );
    Q_ASSERT( data );
    rect.translate( data->m_translate );
}

void KPrAnimationDisappear::next( int currentTime, KPrAnimationData * animationData )
{
    Q_UNUSED( currentTime );
    KPrAnimationDataTranslate * data = dynamic_cast<KPrAnimationDataTranslate *>( animationData );
    Q_ASSERT( data );
    data->m_canvas->updateCanvas( data->m_boundingRect.translated( data->m_translate ) );
    data->m_finished = true;
}

void KPrAnimationDisappear::finish( KPrAnimationData * animationData )
{
    KPrAnimationDataTranslate * data = dynamic_cast<KPrAnimationDataTranslate *>( animationData );
    Q_ASSERT( data );
    data->m_canvas->updateCanvas( data->m_boundingRect.translated( data->m_translate ) );
}
