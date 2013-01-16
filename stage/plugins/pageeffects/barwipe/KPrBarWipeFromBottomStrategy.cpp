/* This file is part of the KDE project
   Copyright (C) 2008 Timothée Lacroix <dakeyras.khan@gmail.com>
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

#include "KPrBarWipeFromBottomStrategy.h"
#include "KPrBarWipeEffectFactory.h"

#include <QWidget>
#include <QPainter>

KPrBarWipeFromBottomStrategy::KPrBarWipeFromBottomStrategy()
: KPrPageEffectStrategy( KPrBarWipeEffectFactory::FromBottom, "barWipe", "topToBottom", true )
{
}

KPrBarWipeFromBottomStrategy::~KPrBarWipeFromBottomStrategy()
{
}

void KPrBarWipeFromBottomStrategy::setup( const KPrPageEffect::Data &data, QTimeLine &timeLine )
{
    timeLine.setFrameRange( 0, data.m_widget->height() );
}

void KPrBarWipeFromBottomStrategy::paintStep( QPainter &p, int currPos, const KPrPageEffect::Data &data )
{
    int width = data.m_widget->width();
    int height = data.m_widget->height();
    QRect rect1( 0, 0, width, height - currPos );
    QRect rect2( 0, height-currPos, width, currPos );
    p.drawPixmap( QPoint( 0, 0 ), data.m_oldPage, rect1 );
    p.drawPixmap( QPoint( 0, height - currPos ), data.m_newPage, rect2 );
}

void KPrBarWipeFromBottomStrategy::next( const KPrPageEffect::Data &data )
{
    int lastPos = data.m_timeLine.frameForTime( data.m_lastTime );
    int currPos = data.m_timeLine.frameForTime( data.m_currentTime );
    data.m_widget->update( 0, data.m_widget->height() - currPos, data.m_widget->width(), currPos - lastPos );
}
