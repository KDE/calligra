/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <david@mandrakesoft.com>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <koGlobal.h>
#include "kwunit.h"
#include <klocale.h>

QString KWUnit::unitDescription( Unit _unit )
{
    switch ( _unit )
    {
    case KWUnit::U_MM:
        return i18n("Millimeters (mm)");
    case KWUnit::U_INCH:
        return i18n("Inches (inch)");
    case KWUnit::U_PT:
        return i18n("Points (pt)" );
    default:
        return i18n("Error!");
    }
}
