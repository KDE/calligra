/*
 * This file is part of Maemo 5 Office UI for Calligra
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2010 Jarosław Staniek <staniek@kde.org>
 *
 * Contact: Arjun Asthana <arjun@iiitd.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef COLLABORATEIFACE_H
#define COLLABORATEIFACE_H

#include <QByteArray>
#include <QColor>

//! Interface for collaborative editing
class CollaborateInterface
{
public:
    virtual void sendString(uint start, uint end, const QByteArray &msg, int source = -1) = 0;
    virtual void sendBackspace(uint start, uint end, int source = -1) = 0;
    virtual void sendFormat(uint start, uint end, FormatFlag format, int source = -1) = 0;
    virtual void sendFontSize(uint start, uint end, uint size, int source = -1) = 0;
    virtual void sendTextColor(uint start, uint end, QRgb color, int source = -1) = 0;
    virtual void sendTextBackgroundColor(uint start, uint end, QRgb color, int source = -1) = 0;
    virtual void sendFontType(uint start, uint end, const QString &font, int source = -1) = 0;
};

#endif
