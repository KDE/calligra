// $Header$

/*
   This file is part of the KDE project
   Copyright (C) 2001 Nicolas GOUTTE <nicog@snafu.de>

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

#include <qdom.h>

#include "KWEFStructures.h"
#include "KWEFBaseClass.h"

QString KWEFBaseClass::getHtmlOpeningTagExtraAttributes(void) const
{
    return QString::null;
}

QString KWEFBaseClass::escapeText(const QString& strIn) const
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
                strReturn+="&quot;";
                break;
            }
        // NOTE: the apostrophe ' is not escaped,
        // NOTE:  as HTML does not define &apos; by default (only XML/XHTML does)
        default:
            {
                // TODO: verify that the character ch can be expressed in the
                // TODO:  encoding in which we will write the HTML file.
                strReturn+=ch;
                break;
            }
        }
    }

    return strReturn;
}

QString KWEFBaseClass::processDocTagStylesOnly(QDomElement myNode)
{
    return QString::null;
}

void KWEFBaseClass::processStyleTag (QDomNode, void *, QString&)
{
}

void KWEFBaseClass::helpStyleProcessing(QDomNode myNode,LayoutData* layout)
{
}

QString KWEFBaseClass::getBodyOpeningTagExtraAttributes(void) const
{
    return QString::null;
}

void KWEFBaseClass::ProcessParagraphData ( QString &paraText, ValueListFormatData &paraFormatDataList, QString &outputText)
{
}

QString KWEFBaseClass::getStartOfListOpeningTag(const CounterData::Style typeList, bool& ordered)
{
    return QString::null;
}

QString KWEFBaseClass::getParagraphElement(const QString& strTag, const QString& strParagraphText, LayoutData& layout)
{
    return QString::null;
}

