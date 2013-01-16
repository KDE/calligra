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

#ifndef KPRTRANSLATEANIMATION_H
#define KPRTRANSLATEANIMATION_H

#include "KPrShapeAnimationOld.h"

/**
 * This is the base class for all animations which are animated by
 * translating the painter.
 */
class KPrTranslateAnimation : public KPrShapeAnimationOld
{
public:
    KPrTranslateAnimation( KoShape * shape, int step, Type type );
    virtual ~KPrTranslateAnimation();

    /// reimplemented
    virtual KPrAnimationData * animationData( KoCanvasBase * canvas, KoShapeManager * shapeManager, const QRectF & pageRect ) = 0;
    /// reimplemented
    virtual bool animate( QPainter &painter, const KoViewConverter &converter, KPrAnimationData * animationData );
    /// reimplemented
    virtual void animateRect( QRectF & rect, KPrAnimationData * animationData );
    /// reimplemented
    virtual void next( int currentTime, KPrAnimationData * animationData );
    /// reimplemented
    virtual void finish( KPrAnimationData * animationData );
};

#endif /* KPRTRANSLATEANIMATION_H */
