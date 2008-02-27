/* This file is part of the KDE project
   Copyright (C) 2008 Carlos Licea <carlos.licea@kdemail.net>

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

#include "KPrIrisWipeEffectStrategyBase.h"

#include <QWidget>
#include <QPainter>

KPrIrisWipeEffectStrategyBase::KPrIrisWipeEffectStrategyBase(KPrPageEffect::SubType subType, const char * smilType, const char *smilSubType, bool reverse)
: KPrPageEffectStrategy( subType, smilType, smilSubType, reverse )
{
}

KPrIrisWipeEffectStrategyBase::~KPrIrisWipeEffectStrategyBase()
{
}

void KPrIrisWipeEffectStrategyBase::setup( const KPrPageEffect::Data &data, QTimeLine &timeLine )
{
    //Check if m_shape hasn't been initialized already
    //weird things happen if we don't check it
    if ( m_shape.isEmpty() )
    {
        setShape();
    }

    const int width = data.m_widget->width();
    const int height = data.m_widget->height();

    if( width > height )
    {
        timeLine.setFrameRange( 0, width );
    }
    else
    {
        timeLine.setFrameRange( 0, height );
    }
}

void KPrIrisWipeEffectStrategyBase::paintStep( QPainter &p, int currPos, const KPrPageEffect::Data &data )
{
    const int width = data.m_widget->width();
    const int height = data.m_widget->height();
    qreal scaleStep;
    if( width > height )
    {
        scaleStep = 1 / m_shape.boundingRect().width();
    }
    else
    {
        scaleStep = 1 / m_shape.boundingRect().height();
    }

    QRect rect( 0, 0, width, height );
    p.drawPixmap( QPoint( 0, 0 ), data.m_oldPage, rect );

    QMatrix matrix;
    matrix.translate( width/2, height/2 );
    matrix.scale( currPos*scaleStep, currPos*scaleStep );

    p.setClipPath( matrix.map(m_shape) );
    p.drawPixmap( QPoint( 0, 0 ), data.m_newPage, rect );
}


void KPrIrisWipeEffectStrategyBase::next( const KPrPageEffect::Data &data )
{
    const int width = data.m_widget->width();
    const int height = data.m_widget->height();
    const int currPos = data.m_timeLine.frameForTime( data.m_currentTime );
    qreal scaleStep;
    if( width > height )
    {
        scaleStep = 1 / m_shape.boundingRect().width();
    }
    else
    {
        scaleStep = 1 / m_shape.boundingRect().height();
    }

    QMatrix matrix;
    matrix.translate( width/2, height/2 );
    matrix.scale( currPos*scaleStep, currPos*scaleStep );
    QPainterPath newPath ( matrix.map( m_shape ) );

    data.m_widget->update( newPath.boundingRect().toRect() );
}
