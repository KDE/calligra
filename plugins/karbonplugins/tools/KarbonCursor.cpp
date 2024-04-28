/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006, 2008 Jan Hambrecht <jaham@gmx.net>
   SPDX-FileCopyrightText: 2006 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2006 Tim Beaulen <tbscope@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KarbonCursor.h"

#include <QBitmap>
#include <QCursor>
#include <QSize>

QCursor KarbonCursor::needleArrow()
{
    static const unsigned char needle_bits[] = {0x00, 0x00, 0x10, 0x00, 0x20, 0x00, 0x60, 0x00, 0xc0, 0x00, 0xc0, 0x01, 0x80, 0x03, 0x80, 0x07,
                                                0x00, 0x0f, 0x00, 0x1f, 0x00, 0x3e, 0x00, 0x7e, 0x00, 0x7c, 0x00, 0x1c, 0x00, 0x18, 0x00, 0x00};

    QBitmap b = QBitmap::fromData(QSize(16, 16), needle_bits);
    QBitmap m = b.createHeuristicMask(false);

    return QCursor(b, m, 2, 0);
}

QCursor KarbonCursor::needleMoveArrow()
{
    static const unsigned char needle_move_bits[] = {0x00, 0x00, 0x10, 0x00, 0x20, 0x00, 0x60, 0x00, 0xc0, 0x00, 0xc0, 0x01, 0x80, 0x03, 0x80, 0x07,
                                                     0x10, 0x0f, 0x38, 0x1f, 0x54, 0x3e, 0xfe, 0x7e, 0x54, 0x7c, 0x38, 0x1c, 0x10, 0x18, 0x00, 0x00};

    QBitmap b = QBitmap::fromData(QSize(16, 16), needle_move_bits);
    QBitmap m = b.createHeuristicMask(false);

    return QCursor(b, m, 2, 0);
}
