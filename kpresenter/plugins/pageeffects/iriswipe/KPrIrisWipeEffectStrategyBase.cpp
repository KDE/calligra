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
    setShape();
    m_width = data.m_widget->width();
    m_height = data.m_widget->height();

    if( m_width > m_height )
    {
        timeLine.setFrameRange( 0, data.m_widget->width() );
        m_scaleStep = 1 / m_shape.boundingRect().width();
    }
    else
    {
        timeLine.setFrameRange( 0, data.m_widget->height() );
        m_scaleStep = 1 / m_shape.boundingRect().height();
    }
}

void KPrIrisWipeEffectStrategyBase::paintStep( QPainter &p, int currPos, const KPrPageEffect::Data &data )
{
    QRect rect( 0, 0, m_width, m_height );
    p.drawPixmap( QPoint( 0, 0 ), data.m_oldPage, rect );

    p.setClipPath( m_modifiedShape );
    p.drawPixmap( QPoint( 0, 0 ), data.m_newPage, rect );
}


void KPrIrisWipeEffectStrategyBase::next( const KPrPageEffect::Data &data )
{
    const int currPos = data.m_timeLine.frameForTime( data.m_currentTime );

    QPainterPath newPath;
    QMatrix matrix;
    matrix.translate( m_width/2, m_height/2 );
    matrix.scale( currPos*m_scaleStep, currPos*m_scaleStep );
    newPath = matrix.map( m_shape );

    data.m_widget->update( newPath.substracted( m_modifiedShape ) );
    m_modifiedShape = newPath;
}
