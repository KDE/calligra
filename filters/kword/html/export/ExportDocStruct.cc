// $Header$

/*
   This file is part of the KDE project
   Copyright (C) 2001, 2002 Nicolas GOUTTE <nicog@snafu.de>

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
#include <qregexp.h>
#include <qtextcodec.h>
#include <qfile.h>

#include <klocale.h>
#include <kdebug.h>

#include <KWEFBaseWorker.h>

#include "ExportFilter.h"
#include "ExportDocStruct.h"

QString HtmlDocStructWorker::getStartOfListOpeningTag(const CounterData::Style typeList, bool& ordered)
{
    QString strResult;
    switch (typeList)
    {
    case CounterData::STYLE_CUSTOMBULLET:
    default:
    case CounterData::STYLE_NONE:
    case CounterData::STYLE_CIRCLEBULLET:
    case CounterData::STYLE_SQUAREBULLET:
    case CounterData::STYLE_DISCBULLET:
        {
            m_orderedList=false;
            strResult="<ul>\n";
            break;
        }
    case CounterData::STYLE_NUM:
    case CounterData::STYLE_ALPHAB_L:
    case CounterData::STYLE_ALPHAB_U:
    case CounterData::STYLE_ROM_NUM_L:
    case CounterData::STYLE_ROM_NUM_U:
    case CounterData::STYLE_CUSTOM:
        {
            m_orderedList=true;
            strResult="<ol>\n";
            break;
        }
    }
    return strResult;
}

void HtmlDocStructWorker::openFormatData(const FormatData& format)
{
    // TODO/FIXME: find another way to find fixed fonts 
    // TODO/FIXME: (leaves out "Typewriter", "Monospace", "Mono")
    if (format.text.fontName.contains("ourier"))
    {
        *m_streamOut << "<tt>"; // teletype
    }
    if (format.text.italic)
    {
        *m_streamOut << "<i>";
    }
    if (format.text.weight >= 75)
    {
        *m_streamOut << "<b>";
    }

    if (1==format.text.verticalAlignment)
    {
        *m_streamOut << "<sub>"; //Subscript
    }
    else if (2==format.text.verticalAlignment)
    {
        *m_streamOut << "<sup>"; //Superscript
    }

    // Strict (X)HTML gives use also <big> and <small> to play with.
}

void HtmlDocStructWorker::closeFormatData(const FormatData& format)
{
    if (2==format.text.verticalAlignment)
    {
        *m_streamOut << "</sup>"; //Superscript
    }
    else if (1==format.text.verticalAlignment)
    {
        *m_streamOut << "</sub>"; //Subscript
    }

    if (format.text.weight >= 75)
    {
        *m_streamOut << "</b>";
    }
    if (format.text.italic)
    {
        *m_streamOut << "</i>";
    }
    if (format.text.fontName.contains("ourier")) // Courier?
    {
        *m_streamOut << "</tt>"; // teletype
    }
}

void HtmlDocStructWorker::openParagraph(const QString& strTag, const LayoutData& layout)
{
    *m_streamOut << '<' << strTag << ">";
    openFormatData(layout.formatData);
}

void HtmlDocStructWorker::closeParagraph(const QString& strTag, const LayoutData& layout)
{
    closeFormatData(layout.formatData);
    *m_streamOut << "</" << strTag << ">\n";
}

void HtmlDocStructWorker::openSpan(const FormatData& format)
{
    openFormatData(format);
}

void HtmlDocStructWorker::closeSpan(const FormatData& format)
{
    closeFormatData(format);
}

