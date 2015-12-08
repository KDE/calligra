/* This file is part of the KDE project
   Copyright (C) 2006,2008 Jan Hambrecht <jaham@gmx.net>
   Copyright (C) 2006 Laurent Montel <montel@kde.org>
   Copyright (C) 2006 Tim Beaulen <tbscope@gmail.com>

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

#include "KarbonCursor.h"

#include <QCursor>
#include <QBitmap>
#include <QSize>


QCursor KarbonCursor::needleArrow()
{
    static const unsigned char needle_bits[] = {
        0x00, 0x00, 0x10, 0x00, 0x20, 0x00, 0x60, 0x00, 0xc0, 0x00, 0xc0, 0x01,
        0x80, 0x03, 0x80, 0x07, 0x00, 0x0f, 0x00, 0x1f, 0x00, 0x3e, 0x00, 0x7e,
        0x00, 0x7c, 0x00, 0x1c, 0x00, 0x18, 0x00, 0x00
    };

    QBitmap b = QBitmap::fromData(QSize(16, 16), needle_bits);
    QBitmap m = b.createHeuristicMask(false);

    return QCursor(b, m, 2, 0);
}

QCursor KarbonCursor::needleMoveArrow()
{
    static const unsigned char needle_move_bits[] = {
        0x00, 0x00, 0x10, 0x00, 0x20, 0x00, 0x60, 0x00, 0xc0, 0x00, 0xc0, 0x01,
        0x80, 0x03, 0x80, 0x07, 0x10, 0x0f, 0x38, 0x1f, 0x54, 0x3e, 0xfe, 0x7e,
        0x54, 0x7c, 0x38, 0x1c, 0x10, 0x18, 0x00, 0x00
    };

    QBitmap b = QBitmap::fromData(QSize(16, 16), needle_move_bits);
    QBitmap m = b.createHeuristicMask(false);

    return QCursor(b, m, 2, 0);
}
