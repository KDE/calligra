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

#include <qbrush.h>

#include "kspread_format.h"
#include "KSpreadLayoutIface.h"

#include "KSpreadColumnIface.h"

using namespace KSpread;

ColumnIface::ColumnIface(ColumnFormat *_lay)
    :LayoutIface(_lay)
{
    m_colLayout=_lay;
}

void ColumnIface::update()
{
    //todo
}

int ColumnIface::column()
{
    return m_colLayout->column();
}

void ColumnIface::setHide(bool _hide)
{
    m_colLayout->setHide(_hide);
}

bool ColumnIface::isHide()const
{
    return m_colLayout->isHide();
}

int ColumnIface::width() const
{
    return m_colLayout->width();
}

void ColumnIface::setWidth( int _w )
{
    m_colLayout->setWidth(_w);
}
