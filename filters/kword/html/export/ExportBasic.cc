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
#include "ExportBasic.h"

QString HtmlBasicWorker::textFormatToCss(const TextFormatting& formatData) const
{// PROVISORY
    QString strElement;

    // Font name
    QString fontName = formatData.fontName;
    if ( !fontName.isEmpty() )
    {
        strElement+="font-family: ";
        strElement+=fontName; // TODO: add alternative font names
        strElement+="; ";
    }

    const int size=formatData.fontSize;
    if (size>0)
    {
        // We use absolute font sizes.
        strElement+="font-size: ";
        strElement+=QString::number(size,10);
        strElement+="pt; ";
    }

    if ( formatData.fgColor.isValid() )
    {
        // Give colour
        strElement+="color: ";
        strElement+=formatData.fgColor.name();
        strElement+="; ";
    }
    return strElement;
}

QString HtmlBasicWorker::getStartOfListOpeningTag(const CounterData::Style typeList, bool& ordered)
{
    QString strResult;
    switch (typeList)
    {
    case CounterData::STYLE_CUSTOMBULLET: // We cannot keep the custom type/style
    default:
        {
            m_orderedList=false;
            strResult="<ul>\n";
            break;
        }
    case CounterData::STYLE_NONE: // We cannot specify "no bullet"
        {
            m_orderedList=false;
            strResult="<ul>\n";
            break;
        }
    case CounterData::STYLE_CIRCLEBULLET:
        {
            m_orderedList=false;
            strResult="<ul type=\"circle\">\n";
            break;
        }
    case CounterData::STYLE_SQUAREBULLET:
        {
            m_orderedList=false;
            strResult="<ul type=\"square\">\n";
            break;
        }
    case CounterData::STYLE_DISCBULLET:
        {
            m_orderedList=false;
            strResult="<ul type=\"disc\">\n";
            break;
        }
    case CounterData::STYLE_NUM:
        {
            m_orderedList=true;
            strResult="<ol type=\"1\">\n";
            break;
        }
    case CounterData::STYLE_ALPHAB_L:
        {
            m_orderedList=true;
            strResult="<ol type=\"a\">\n";
            break;
        }
    case CounterData::STYLE_ALPHAB_U:
        {
            m_orderedList=true;
            strResult="<ol type=\"A\">\n";
            break;
        }
    case CounterData::STYLE_ROM_NUM_L:
        {
            m_orderedList=true;
            strResult="<ol type=\"i\">\n";
            break;
        }
    case CounterData::STYLE_ROM_NUM_U:
        {
            m_orderedList=true;
            strResult="<ol type=\"I\">\n";
            break;
        }
    case CounterData::STYLE_CUSTOM:
        {
            // We cannot keep the custom type/style
            m_orderedList=true;
            strResult="<ol>\n";
            break;
        }
    }
    return strResult;
}

void HtmlBasicWorker::writeDocType(void)
{
    // write <!DOCTYPE
    *m_streamOut << "<!DOCTYPE ";
    if (isXML())
    {
        *m_streamOut << "html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"";
        *m_streamOut << " \"DTD/xhtml1-transitional.dtd\">\n";

    }
    else
    {
        *m_streamOut << "HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"";
        *m_streamOut << " \"http://www.w3.org/TR/html4/loose.dtd\">\n";
    }
}

void HtmlBasicWorker::openFormatData(const FormatData& format, const bool allowBold)
{
    // TODO: <font>

    if (allowBold && (format.text.weight>=75))
    {
        *m_streamOut << "<b>";
    }
    if (format.text.italic)
    {
        *m_streamOut << "<i>";
    }
    if (format.text.underline)
    {
        *m_streamOut << "<u>";
    }
    if (format.text.strikeout)
    {
        *m_streamOut << "<s>";
    }
    if (1==format.text.verticalAlignment)
    {
        *m_streamOut << "<sub>"; //Subscript
    }
    else if (2==format.text.verticalAlignment)
    {
        *m_streamOut << "<sup>"; //Superscript
    }
}

void HtmlBasicWorker::closeFormatData(const FormatData& format, const bool allowBold)
{
    if (2==format.text.verticalAlignment)
    {
        *m_streamOut << "</sup>"; //Superscript
    }
    else if (1==format.text.verticalAlignment)
    {
        *m_streamOut << "</sub>"; //Subscript
    }
    if (format.text.strikeout)
    {
        *m_streamOut << "</s>";
    }
    if (format.text.underline)
    {
        *m_streamOut << "</u>";
    }
    if (format.text.italic)
    {
        *m_streamOut << "</i>";
    }
    if (allowBold && (format.text.weight >= 75))
    {
        *m_streamOut << "</b>";
    }
}

void HtmlBasicWorker::openParagraph(const QString& strTag, 
    const LayoutData& layout)
{
    *m_streamOut << '<' << strTag;

    if ( (layout.alignment=="left") || (layout.alignment== "right")
        || (layout.alignment=="center") || (layout.alignment=="justify"))
    {
        *m_streamOut << " align=\"" << layout.alignment << "\"";
    }
    else
    {
        kdWarning(30503) << "Unknown alignment: " << layout.alignment << endl;
    }

    *m_streamOut << ">";

    openFormatData(layout.formatData,(strTag[0]!='h')); // Allow bold only if tag is not a heading!
}

void HtmlBasicWorker::closeParagraph(const QString& strTag,
    const LayoutData& layout)
{
    closeFormatData(layout.formatData,(strTag[0]!='h')); // Allow bold only if tag is not a heading!

    *m_streamOut << "</" << strTag << ">\n";
}

void HtmlBasicWorker::openSpan(const FormatData&, const FormatData& format)
{
    openFormatData(format, true);
}

void HtmlBasicWorker::closeSpan(const FormatData&, const FormatData& format)
{
    closeFormatData(format, true);
}

bool HtmlBasicWorker::doOpenBody(void)
{
    // Define the background colour as white!
    *m_streamOut << "<body bgcolor=\"#FFFFFF\">\n";
    return true;
}

