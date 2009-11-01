/* Swinder - Portable library for spreadsheet
   Copyright (C) 2003-2005 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2006,2009 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>

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
   Boston, MA 02110-1301, USA
 */
#include "utils.h"

namespace Swinder {

Value errorAsValue( int errorCode )
{
    Value result(Value::Error);

    switch (errorCode) {
        case 0x00: result = Value::errorNULL();  break;
        case 0x07: result = Value::errorDIV0();  break;
        case 0x0f: result = Value::errorVALUE(); break;
        case 0x17: result = Value::errorREF();   break;
        case 0x1d: result = Value::errorNAME();  break;
        case 0x24: result = Value::errorNUM();   break;
        case 0x2A: result = Value::errorNA();    break;
        default: break;
    }

    return result;
}

} // namespace Swinder
