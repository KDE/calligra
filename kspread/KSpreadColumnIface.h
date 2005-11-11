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

#ifndef KSPREAD_COLUMN_IFACE_H
#define KSPREAD_COLUMN_IFACE_H

#include "KSpreadLayoutIface.h"

namespace KSpread
{
class ColumnFormat;

class ColumnIface : virtual public LayoutIface
{
    K_DCOP
public:
    ColumnIface(ColumnFormat *_lay);
    virtual void update();
k_dcop:
    virtual int column();
    virtual void setHide(bool _hide);
    virtual bool isHide()const;
    virtual int width() const;
    virtual void setWidth( int _w );

private:
    ColumnFormat *m_colLayout;
};

} // namespace KSpread

#endif
