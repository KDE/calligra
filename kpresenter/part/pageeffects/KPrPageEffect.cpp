/* This file is part of the KDE project
   Copyright (C) 2007 Thorsten Zachmann <zachmann@kde.org>

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

#include "KPrPageEffect.h"

#include <QWidget>

KPrPageEffect::KPrPageEffect( const QPixmap &px1, const QPixmap &px2, QWidget * w )
: m_px1( px1 )
, m_px2( px2 )
, m_widget( w )
, m_finish( false )
, m_lastPos( 0, 0 )
{
    Q_ASSERT( px1.size() == px2.size() );
    Q_ASSERT( px1.size() == m_widget->size() );
    m_widget->setAttribute( Qt::WA_OpaquePaintEvent );
}

KPrPageEffect::~KPrPageEffect()
{
    m_widget->setAttribute( Qt::WA_OpaquePaintEvent, false );
}

void KPrPageEffect::next( int currentTime )
{
    Q_UNUSED( currentTime );
    m_widget->update();
}

void KPrPageEffect::finish()
{
    m_finish = true;
    m_widget->update();
}

bool KPrPageEffect::isFinished()
{
    return m_finish;
}

int KPrPageEffect::duration()
{
    return m_timeLine.duration();
}
