//

/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003, 2004 Nicolas GOUTTE <goutte@kde.org>

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

#include <qstring.h>

#include <kdebug.h>

#include <kword13document.h>

#include "kword13oasisgenerator.h"

KWord13OasisGenerator::KWord13OasisGenerator( void )
{
}

KWord13OasisGenerator::~KWord13OasisGenerator( void )
{
}

QString KWord13OasisGenerator::escapeOOText(const QString& strText) const
{
    // Escape quotes (needed in attributes)
    // Escape apostrophs (allowed by XML)

    QString strReturn;
    QChar ch;

    for (uint i=0; i<strText.length(); i++)
    {
        ch=strText[i];
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
                strReturn+="&quot;";
                break;
            }
        case 39: // '
            {
                strReturn+="&apos;";
                break;
            }
        default:
            {
                strReturn+=ch;
                break;
            }
        }
    }

    return strReturn;
}


QString KWord13OasisGenerator::escapeOOSpan(const QString& strText) const
// We need not only to escape the classical XML stuff but also to take care of spaces and tabs.
// Also we must take care about not falling into the rules in XML about white space between 2 opening tags or between 2 closing tags
{
    QString strReturn;
    QChar ch;
    int spaceNumber = 0; // How many spaces should be written
    uint spaceSequenceStart = 9999; // Where does the space sequence start (start value must be non-null)

    for (uint i=0; i<strText.length(); i++)
    {
        ch=strText[i];

        if (ch!=' ')
        {
            // The next character is not a space (anymore)
            if ( spaceNumber > 0 )
            {
                if ( spaceSequenceStart )
                {   // Generate a real space only if we are not at start

                    strReturn += ' ';
                    --spaceNumber;
                }
                if ( spaceNumber > 1 )
                {
                    strReturn += "<text:s text:c=\"";
                    strReturn += QString::number( spaceNumber );
                    strReturn += "\"/>";
                }
            }
            spaceNumber = 0;
        }

        // ### TODO: would be switch/case or if/elseif the best?
        switch (ch.unicode())
        {
        case 9: // Tab
            {
                strReturn+="<text:tab-stop/>";
                break;
            }
        case 10: // Line-feed
            {
                strReturn+="<text:line-break/>";
                break;
            }
        case 32: // Space
            {
                if ( spaceNumber > 0 )
                {
                    ++spaceNumber;
                }
                else
                {
                    spaceNumber = 1;
                    spaceSequenceStart = i;
                }
                break;
            }
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
                strReturn+="&quot;";
                break;
            }
        case 39: // '
            {
                strReturn+="&apos;";
                break;
            }
        case 1: // (Non-XML-compatible) replacement character from KWord 0.8
            {
                strReturn += '#'; //use KWord 1.[123] replacement character instead
                break;
            }
        // Following characters are not allowed in XML (but some files from KWord 0.8 have some of them.)
        case  0: case  2 ... 8: case 11: case 12: case 14 ... 31:
            {
                kdWarning(30518) << "Not allowed XML character: " << ch.unicode() << endl;
                strReturn += '?';
                break;
            }
        case 13: // ### TODO: what to do with it?
        default:
            {
                strReturn+=ch;
                break;
            }
        }
    }

    if ( spaceNumber > 0 )
    {
        // The last characters were spaces
        // We do not care about writing a real space (also to avoid to have <tag> </tag>)
        strReturn+="<text:s text:c=\"";
        strReturn+=QString::number(spaceNumber);
        strReturn+="\"/>";
    }

    return strReturn;
}



bool KWord13OasisGenerator::generate ( const QString& fileName, KWord13Document& kwordDocument )
{
    return true;
}

