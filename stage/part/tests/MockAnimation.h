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

#ifndef MOCKANIMATION_H
#define MOCKANIMATION_H

#include "shapeanimations/KPrShapeAnimationOld.h"
#include "shapeanimations/KPrAnimationData.h"

#include <KoViewConverter.h>

#include <QPainter>

class MockAppearAnimation : public KPrShapeAnimationOld
{
public:
    MockAppearAnimation( KoShape * shape, int step )
    : KPrShapeAnimationOld( shape, step, Appear )
    {}

    KPrAnimationData * animationData( KoCanvasBase * canvas, KoShapeManager * shapeManager, const QRectF & pageRect )
    {
        Q_UNUSED( canvas );
        Q_UNUSED( shapeManager );
        Q_UNUSED( pageRect );
        return 0;
    }

    bool animate( QPainter &painter, const KoViewConverter &converter, KPrAnimationData * animationData )
    {
        Q_UNUSED( painter );
        Q_UNUSED( converter );
        Q_UNUSED( animationData );
        return true;
    }

    void animateRect( QRectF & rect, KPrAnimationData * animationData )
    {
        Q_UNUSED( rect );
        Q_UNUSED( animationData );
    }

    void next( int currentTime, KPrAnimationData * animationData )
    {
        Q_UNUSED( currentTime );
        Q_UNUSED( animationData );
    }

    void finish( KPrAnimationData * animationData )
    {
        Q_UNUSED(animationData);
    }
};

class MockDisappearAnimation : public KPrShapeAnimationOld
{
public:
    MockDisappearAnimation( KoShape * shape, int step )
    : KPrShapeAnimationOld( shape, step, Disappear )
    {}

    KPrAnimationData * animationData( KoCanvasBase * canvas, KoShapeManager * shapeManager, const QRectF & pageRect )
    {
        Q_UNUSED( canvas );
        Q_UNUSED( shapeManager );
        Q_UNUSED( pageRect );
        return 0;
    }

    bool animate( QPainter &painter, const KoViewConverter &converter, KPrAnimationData * animationData )
    {
        Q_UNUSED( painter );
        Q_UNUSED( converter );
        Q_UNUSED( animationData );
        return true;
    }

    void animateRect( QRectF & rect, KPrAnimationData * animationData )
    {
        Q_UNUSED( rect );
        Q_UNUSED( animationData );
    }

    void next( int currentTime, KPrAnimationData * animationData )
    {
        Q_UNUSED( currentTime );
        Q_UNUSED( animationData );
    }

    void finish( KPrAnimationData * animationData )
    {
        Q_UNUSED(animationData);
    }
};


#endif /* MOCKANIMATION_H */
