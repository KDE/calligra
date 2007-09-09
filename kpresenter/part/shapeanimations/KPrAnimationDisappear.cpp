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

KPrAnimationDisappear::KPrAnimationDisappear( KoShape * shape, int step )
: KPrShapeAnimation( shape, step, Disappear )
{
    m_timeLine.setDuration( 1 );

    // TODO use bounding rect + shadow
    m_shapeRect = m_shape->boundingRect();
    // the 2.0 is because of the anitialiasing
    double x = m_shapeRect.x() + m_shapeRect.width() + 2.0;
    m_translate = QPointF( -x, 0 );
}

KPrAnimationDisappear::~KPrAnimationDisappear()
{
}

bool KPrAnimationDisappear::animate( QPainter &painter, const KoViewConverter &converter )
{
    painter.translate( m_translate );
    return m_finished;
}

void KPrAnimationDisappear::animateRect( QRectF & rect )
{
    rect.translate( m_translate );
}

void KPrAnimationDisappear::next( int currentTime, KoCanvasBase * canvas )
{
    Q_UNUSED( currentTime );
    canvas->updateCanvas( m_shapeRect.translated( m_translate ) );
    m_finished = true;
}

void KPrAnimationDisappear::finish( KoCanvasBase * canvas )
{
    canvas->updateCanvas( m_shapeRect.translated( m_translate ) );
}
