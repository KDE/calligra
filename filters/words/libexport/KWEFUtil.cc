/*
   This file is part of the KDE project
   Copyright (C) 2001 Nicolas GOUTTE <goutte@kde.org>

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

#include <QString>
#include <QTextCodec>

#include <KoPageLayout.h>

#include "KWEFUtil.h"

QString KWEFUtil::EscapeSgmlText(const QTextCodec* codec,
                                 const QString& strIn,
                                 const bool quot /* = false */ ,
                                 const bool apos /* = false */)
{
    QString strReturn;
    QChar ch;

    uint strInLength = strIn.length();
    for (uint i = 0; i < strInLength; i++) {
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
            if (quot)
                strReturn += "&quot;";
            else
                strReturn += ch;
            break;
        }
        case 39: { // '
            // NOTE:  HTML does not define &apos; by default (only XML/XHTML does)
            if (apos)
                strReturn += "&apos;";
            else
                strReturn += ch;
            break;
        }
        default: {
            // verify that the character ch can be expressed in the
            //   encoding in which we will write the HTML file.
            if (codec) {
                if (!codec->canEncode(ch)) {
                    strReturn += QString("&#%1;").arg(ch.unicode());
                    break;
                }
            }
            strReturn += ch;
            break;
        }
        }
    }

    return strReturn;
}
