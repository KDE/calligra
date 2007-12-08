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

KPrPageEffect::KPrPageEffect( int duration, const QString & id )
: m_duration( duration )
, m_id( id )
{
}

KPrPageEffect::~KPrPageEffect()
{
}

void KPrPageEffect::next( const Data &data )
{
    data.m_widget->update();
}

void KPrPageEffect::finish( const Data &data )
{
    data.m_widget->update();
}

int KPrPageEffect::duration() const
{
    return m_duration;
}

const QString & KPrPageEffect::id() const
{
    return m_id;
}
