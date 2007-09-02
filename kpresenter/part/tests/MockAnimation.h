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

#include "shapeanimations/KPrShapeAnimation.h"

class MockAppearAnimation : public KPrShapeAnimation
{
public:
	MockAppearAnimation( KoShape * shape, int step )
    : KPrShapeAnimation( shape, step, Appear )        
    {}

    bool animate( QPainter &painter, const KoViewConverter &converter ) { Q_UNUSED( painter ); Q_UNUSED( converter ); return true; }

    void animateRect( QRectF & rect ) { Q_UNUSED( rect ); }

    void next( int currentTime, KoCanvasBase * canvas ) { Q_UNUSED( currentTime ); Q_UNUSED( canvas ); }
    
    void finish( KoCanvasBase * canvas ) { Q_UNUSED(canvas); }
};

class MockDisappearAnimation : public KPrShapeAnimation
{
public:
	MockDisappearAnimation( KoShape * shape, int step )
    : KPrShapeAnimation( shape, step, Disappear )        
    {}

    bool animate( QPainter &painter, const KoViewConverter &converter ) { Q_UNUSED( painter ); Q_UNUSED( converter ); return true; }

    void animateRect( QRectF & rect ) { Q_UNUSED( rect ); }

    void next( int currentTime, KoCanvasBase * canvas ) { Q_UNUSED( currentTime ); Q_UNUSED( canvas ); }
    
    void finish( KoCanvasBase * canvas ) { Q_UNUSED(canvas); }
};


#endif /* MOCKANIMATION_H */
