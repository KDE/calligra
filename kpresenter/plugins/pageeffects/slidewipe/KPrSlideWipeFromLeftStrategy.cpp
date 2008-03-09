/* This file is part of the KDE project
   Copyright (C) 2008 Thorsten Zachmann <zachmann@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KPrSlideWipeFromLeftStrategy.h"
#include "KPrSlideWipeEffectFactory.h"

#include <QWidget>
#include <QPainter>

KPrSlideWipeFromLeftStrategy::KPrSlideWipeFromLeftStrategy()
: KPrPageEffectStrategy( KPrSlideWipeEffectFactory::FromLeft, "slideWipe", "fromLeft", false )
{
}

KPrSlideWipeFromLeftStrategy::~KPrSlideWipeFromLeftStrategy()
{
}

void KPrSlideWipeFromLeftStrategy::setup( const KPrPageEffect::Data &data, QTimeLine &timeLine )
{
    timeLine.setFrameRange( 0, data.m_widget->width() );
}

void KPrSlideWipeFromLeftStrategy::paintStep( QPainter &p, int currPos, const KPrPageEffect::Data &data )
{
    int height = data.m_widget->height();
    int width = data.m_widget->width();
    QRect rect1( currPos, 0, width - currPos, height );
    QRect rect2( width - currPos, 0, currPos, height );
    p.drawPixmap( QPoint( currPos, 0 ), data.m_oldPage, rect1 );
    p.drawPixmap( QPoint( 0, 0 ), data.m_newPage, rect2 );
}

void KPrSlideWipeFromLeftStrategy::next( const KPrPageEffect::Data &data )
{
    int currPos = data.m_timeLine.frameForTime( data.m_currentTime );
    data.m_widget->update( 0, 0, currPos, data.m_widget->height() );
}
