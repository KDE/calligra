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

#include "KPrShapeAnimationOld.h"

KPrShapeAnimationOld::KPrShapeAnimationOld( KoShape * shape, int step, Type type )
: m_shape( shape )
, m_step( step )
, m_type( type )
, m_finished( false )
{
    // TODO make setable
    m_timeLine.setDuration( 5000 );
    m_timeLine.setCurveShape( QTimeLine::LinearCurve );
}

KPrShapeAnimationOld::~KPrShapeAnimationOld()
{
}

int KPrShapeAnimationOld::duration() const
{
    return m_timeLine.duration();
}

int KPrShapeAnimationOld::step() const
{
    return m_step;
}

void KPrShapeAnimationOld::setStep( int step )
{
    m_step = step;
}

KoShape * KPrShapeAnimationOld::shape() const
{
    return m_shape;
}

KPrShapeAnimationOld::Type KPrShapeAnimationOld::type() const
{
    return m_type;
}
