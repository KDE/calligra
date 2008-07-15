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

KPrIrisWipeEffectStrategyBase::KPrIrisWipeEffectStrategyBase( QPainterPath shape, int subType, const char * smilType, const char *smilSubType, bool reverse )
: KPrPageEffectStrategy( subType, smilType, smilSubType, reverse )
, m_shape( shape )
{
}

KPrIrisWipeEffectStrategyBase::~KPrIrisWipeEffectStrategyBase()
{
}

void KPrIrisWipeEffectStrategyBase::setup( const KPrPageEffect::Data &data, QTimeLine &timeLine )
{
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
    qreal fullScale;
    if( width > height )
    {
        scaleStep = 1 / m_shape.boundingRect().width();
        fullScale = width;
    }
    else
    {
        scaleStep = 1 / m_shape.boundingRect().height();
        fullScale = height;
    }

    if( !reverse() )
    {
        QRect rect( 0, 0, width, height );
        p.drawPixmap( QPoint( 0, 0 ), data.m_oldPage, rect );

        QMatrix matrix;
        matrix.translate( width/2, height/2 );
        matrix.scale( currPos*scaleStep, currPos*scaleStep );

        p.setClipPath( matrix.map(m_shape) );
        p.drawPixmap( QPoint( 0, 0 ), data.m_newPage, rect );
    }
    else
    {
        QRect rect( 0, 0, width, height );
        p.drawPixmap( QPoint( 0, 0 ), data.m_newPage, rect );

        QMatrix matrix;
        matrix.translate( width/2, height/2 );
        matrix.scale( ( fullScale - currPos )*scaleStep, ( fullScale - currPos )*scaleStep );

        p.setClipPath( matrix.map(m_shape) );
        p.drawPixmap( QPoint( 0, 0 ), data.m_oldPage, rect );
    }
}


void KPrIrisWipeEffectStrategyBase::next( const KPrPageEffect::Data &data )
{
    const int width = data.m_widget->width();
    const int height = data.m_widget->height();
    const int currPos = data.m_timeLine.frameForTime( data.m_currentTime );
    qreal scaleStep;
    qreal fullScale;
    if( width > height )
    {
        scaleStep = 1 / m_shape.boundingRect().width();
        fullScale = width;
    }
    else
    {
        scaleStep = 1 / m_shape.boundingRect().height();
        fullScale = height;
    }

    QPainterPath newPath;
    if( !reverse() )
    {
        QMatrix matrix;
        matrix.translate( width/2, height/2 );
        matrix.scale( currPos*scaleStep, currPos*scaleStep );
        newPath = matrix.map( m_shape );
    }
    else
    {
        QMatrix matrix;
        matrix.translate( width/2, height/2 );
        matrix.scale( ( fullScale - currPos )*scaleStep, ( fullScale - currPos )*scaleStep );
        newPath = matrix.map( m_shape );
    }

    data.m_widget->update( newPath.boundingRect().toRect() );
}
