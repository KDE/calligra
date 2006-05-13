/*
   This file is part of the KDE project
   Copyright (C) 2001, 2002 Nicolas GOUTTE <goutte@kde.org>

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
#include <QFile>

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
            ordered=false;
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
            ordered=true;
            strResult="<ol>\n";
            break;
        }
    }
    return strResult;
}

void HtmlDocStructWorker::openFormatData(const FormatData& formatOrigin,
    const FormatData& format, const bool force, const bool allowBold)
{
    // TODO/FIXME: find another way to find fixed fonts
    // TODO/FIXME: (leaves out "Typewriter", "Monospace", "Mono")
    if (format.text.fontName.contains("ourier"))
    {
        *m_streamOut << "<tt>"; // teletype
    }

    if (force || (formatOrigin.text.italic!=format.text.italic))
    {
        if (format.text.italic)
        {
            *m_streamOut << "<i>";
        }
    }

    if (force || ((formatOrigin.text.weight>=75)!=(format.text.weight>=75)))
    {
        if (allowBold && (format.text.weight >= 75))
        {
            *m_streamOut << "<b>";
        }
    }

    if (force || (formatOrigin.text.verticalAlignment!=format.text.verticalAlignment))
    {
        if (1==format.text.verticalAlignment)
        {
            *m_streamOut << "<sub>"; //Subscript
        }
        else if (2==format.text.verticalAlignment)
        {
            *m_streamOut << "<sup>"; //Superscript
        }
    }

    // Strict (X)HTML gives use also <big> and <small> to play with.
}

void HtmlDocStructWorker::closeFormatData(const FormatData& formatOrigin,
    const FormatData& format, const bool force, const bool allowBold)
{
    if (force || (formatOrigin.text.verticalAlignment!=format.text.verticalAlignment))
    {
        if (2==format.text.verticalAlignment)
        {
            *m_streamOut << "</sup>"; //Superscript
        }
        else if (1==format.text.verticalAlignment)
        {
            *m_streamOut << "</sub>"; //Subscript
        }
    }

    if (force || ((formatOrigin.text.weight>=75)!=(format.text.weight>=75)))
    {
        if (allowBold && (format.text.weight >= 75))
        {
            *m_streamOut << "</b>";
        }
    }

    if (force || (formatOrigin.text.italic!=format.text.italic))
    {
        if (format.text.italic)
        {
            *m_streamOut << "</i>";
        }
    }

    if (format.text.fontName.contains("ourier")) // Courier?
    {
        *m_streamOut << "</tt>"; // teletype
    }
}

void HtmlDocStructWorker::openParagraph(const QString& strTag,
    const LayoutData& layout,QChar::Direction /*direction*/)
{
    *m_streamOut << '<' << strTag << ">";
    openFormatData(layout.formatData,layout.formatData,true,(strTag[0]!='h'));
}

void HtmlDocStructWorker::closeParagraph(const QString& strTag,
    const LayoutData& layout)
{
    closeFormatData(layout.formatData,layout.formatData,true,(strTag[0]!='h'));
    *m_streamOut << "</" << strTag << ">\n";
}

void HtmlDocStructWorker::openSpan(const FormatData& formatOrigin, const FormatData& format)
{
    openFormatData(formatOrigin,format,false,true);
}

void HtmlDocStructWorker::closeSpan(const FormatData& formatOrigin, const FormatData& format)
{
    closeFormatData(formatOrigin,format,false,true);
}

