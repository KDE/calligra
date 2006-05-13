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
                      const bool apos /* = false */ )
{
    QString strReturn;
    QChar ch;

    for (uint i=0; i<strIn.length(); i++)
    {
        ch=strIn[i];
        switch (ch.unicode())
        {
        case 38: // &
            {
                strReturn+="&amp;";
                break;
            }
        case 60: // <
            {
                strReturn+="&lt;";
                break;
            }
        case 62: // >
            {
                strReturn+="&gt;";
                break;
            }
        case 34: // "
            {
                if (quot)
                    strReturn+="&quot;";
                else
                    strReturn+=ch;
                break;
            }
        case 39: // '
            {
                // NOTE:  HTML does not define &apos; by default (only XML/XHTML does)
                if (apos)
                    strReturn+="&apos;";
                else
                    strReturn+=ch;
                break;
            }
        default:
            {
                // verify that the character ch can be expressed in the
                //   encoding in which we will write the HTML file.
                if (codec)
                {
                    if (!codec->canEncode(ch))
                    {
                        strReturn+=QString("&#%1;").arg(ch.unicode());
                        break;
                    }
                }
                strReturn+=ch;
                break;
            }
        }
    }

    return strReturn;
}

void KWEFUtil::GetNativePaperFormat(const int format,
    QString& width, QString& height, QString& units)
// Find data for paper format, as needed for AbiWord and CSS
{
    switch (format)
    {
        // ISO A formats
        case PG_DIN_A0: // ISO A0
        {
            width="84.1"; height="118.0"; units="cm";
            break;
        }
        case PG_DIN_A1: // ISO A1
        {
            width="59.4"; height="84.1"; units="cm";
            break;
        }
        case PG_DIN_A2: // ISO A2
        {
            width="42.0"; height="59.4"; units="cm";
            break;
        }
        case PG_DIN_A3: // ISO A3
        {
            width="29.7"; height="42.0"; units="cm";
            break;
        }
        case PG_DIN_A4: // ISO A4
        {
            width="21.0"; height="29.7"; units="cm";
            break;
        }
        case PG_DIN_A5: // ISO A5
        {
            width="14.8"; height="21.0"; units="cm";
            break;
        }
        case PG_DIN_A6: // ISO A6
        {
            width="10.5"; height="14.8"; units="cm";
            break;
        }
        // ISO B formats
        case PG_DIN_B0: // ISO B0
        {
            width="100.0"; height="141.0"; units="cm";
            break;
        }
        case PG_DIN_B1: // ISO B1
        {
            width="70.7"; height="100.0"; units="cm";
            break;
        }
        case PG_DIN_B2: // ISO B2
        {
            width="50.0"; height="70.7"; units="cm";
            break;
        }
        case PG_DIN_B3: // ISO B3
        {
            width="35.3"; height="50.0"; units="cm";
            break;
        }
        case PG_DIN_B4: // ISO B4
        {
            width="25.8"; height="35.3"; units="cm";
            break;
        }
        case PG_DIN_B5: // ISO B5
        {
            width="17.6"; height="25.0"; units="cm";
            break;
        }
        case PG_DIN_B6: // ISO B6
        {
            width="12.5"; height="17.6"; units="cm";
            break;
        }
        // American formats
        case PG_US_LETTER: // US Letter
        {
            width="8.5"; height="11.0"; units="inch";
            break;
        }
        case PG_US_LEGAL: // US Legal
        {
            width="8.5"; height="14.0"; units="inch";
            break;
        }
        case PG_US_EXECUTIVE: // US Executive
        {
            width="7.5"; height="10.0"; units="inch";
            break;
        }
        // Other format not supported yet by AbiWord CVS 2001-04-25)
        case PG_DIN_A7: // ISO A7
        case PG_DIN_A8: // ISO A8
        case PG_DIN_A9: // ISO A9
        case PG_DIN_B10: // ISO B10
        // Other formats
        case PG_SCREEN: // Screen
        case PG_CUSTOM: // Custom
        default:
        {
            // TODO
            width=QString::null;
            height=QString::null;
            units=QString::null;
            break;
        }
    }
}
