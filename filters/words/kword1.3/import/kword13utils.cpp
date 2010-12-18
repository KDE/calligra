/*
   This file is part of the KDE project
   Copyright (C) 2004 Nicolas GOUTTE <goutte@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kword13utils.h"

QString EscapeXmlDump(const QString& strIn)
{
    QString strReturn;
    QChar ch;

    for (uint i = 0; i < strIn.length(); i++) {
        ch = strIn[i];
        switch (ch.unicode()) {
        case 38: { // &
            strReturn += "&amp;";
            break;
        }
        case 60: { // <
            strReturn += "&lt;";
            break;
        }
        case 62: { // >
            strReturn += "&gt;";
            break;
        }
        case 34: { // "
            strReturn += "&quot;";
            break;
        }
        case 39: { // '
            strReturn += "&apos;";
            break;
        }
        default: {
            strReturn += ch;
            break;
        }
        }
    }

    return strReturn;
}

