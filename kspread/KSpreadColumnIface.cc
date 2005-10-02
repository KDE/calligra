/* This file is part of the KDE project

   Copyright 2002 Laurent Montel <montel@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>

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

#include "KSpreadLayoutIface.h"
#include "KSpreadColumnIface.h"
#include "kspread_format.h"
#include <qbrush.h>

KSpreadColumnIface::KSpreadColumnIface(ColumnFormat *_lay)
    :KSpreadLayoutIface(_lay)
{
    m_colLayout=_lay;
}

void KSpreadColumnIface::update()
{
    //todo
}

int KSpreadColumnIface::column()
{
    return m_colLayout->column();
}

void KSpreadColumnIface::setHide(bool _hide)
{
    m_colLayout->setHide(_hide);
}

bool KSpreadColumnIface::isHide()const
{
    return m_colLayout->isHide();
}

int KSpreadColumnIface::width() const
{
    return m_colLayout->width();
}

void KSpreadColumnIface::setWidth( int _w )
{
    m_colLayout->setWidth(_w);
}
