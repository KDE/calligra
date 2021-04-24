// This file is part of the KDE project
// SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

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
