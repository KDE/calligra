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
#include "ExportBasic.h"

HtmlBasicWorker::HtmlBasicWorker( const QString &cssURL )
{
  m_cssURL = cssURL;
}

QString HtmlBasicWorker::textFormatToCss(const TextFormatting& formatData) const
{// PROVISORY
    QString strElement;

    // Font name
    QString fontName = formatData.fontName;
    if ( !fontName.isEmpty() )
    {
        strElement+="font-family: ";
        strElement+=escapeHtmlText(fontName); // TODO: add alternative font names
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
        // Give color
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
            ordered=false;
            strResult="<ul>\n";
            break;
        }
    case CounterData::STYLE_NONE: // We cannot specify "no bullet"
        {
            ordered=false;
            strResult="<ul>\n";
            break;
        }
    case CounterData::STYLE_CIRCLEBULLET:
        {
            ordered=false;
            strResult="<ul type=\"circle\">\n";
            break;
        }
    case CounterData::STYLE_SQUAREBULLET:
        {
            ordered=false;
            strResult="<ul type=\"square\">\n";
            break;
        }
    case CounterData::STYLE_DISCBULLET:
        {
            ordered=false;
            strResult="<ul type=\"disc\">\n";
            break;
        }
    case CounterData::STYLE_NUM:
        {
            ordered=true;
            strResult="<ol type=\"1\">\n";
            break;
        }
    case CounterData::STYLE_ALPHAB_L:
        {
            ordered=true;
            strResult="<ol type=\"a\">\n";
            break;
        }
    case CounterData::STYLE_ALPHAB_U:
        {
            ordered=true;
            strResult="<ol type=\"A\">\n";
            break;
        }
    case CounterData::STYLE_ROM_NUM_L:
        {
            ordered=true;
            strResult="<ol type=\"i\">\n";
            break;
        }
    case CounterData::STYLE_ROM_NUM_U:
        {
            ordered=true;
            strResult="<ol type=\"I\">\n";
            break;
        }
    case CounterData::STYLE_CUSTOM:
        {
            // We cannot keep the custom type/style
            ordered=true;
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

void HtmlBasicWorker::openFormatData(const FormatData& formatOrigin,
    const FormatData& format, const bool force,const bool allowBold)
{
    bool useCSS = !m_cssURL.isEmpty();
    QString attr;

    if( !useCSS && ( force || formatOrigin.text.fontName != format.text.fontName ) && !format.text.fontName.isEmpty() )
    {
        attr += " face=\"";
        attr += escapeHtmlText(format.text.fontName); // TODO: add alternative font names
        attr += "\"";
    }

    if( !useCSS && ( force || formatOrigin.text.fontSize != format.text.fontSize ) && format.text.fontSize > 0 )
    {
        // We use absolute font sizes, as relative ones give too many problems.
        int size=format.text.fontSize;
        // 12pt is considered the normal size
        size /= 4;
        if (size<1) size=1;
        if (size>7) size=7;
        attr += " size=\""; // in XML numbers must be quoted!
        attr += QString::number(size,10);
        attr += "\"";
    }

    if( ( force || formatOrigin.text.fgColor != format.text.fgColor ) &&
          format.text.fgColor.isValid() )
    {
        // Give color
        attr += " color=\"";
        attr += format.text.fgColor.name();
        attr += "\"";
    }

    if( !attr.isEmpty() )
    {
        *m_streamOut << "<font" << attr << ">";
    }

    if (force || ((formatOrigin.text.weight>=75)!=(format.text.weight>=75)))
    {
        if (allowBold && (format.text.weight>=75))
        {
            *m_streamOut << "<b>";
        }
    }

    if (force || (formatOrigin.text.italic!=format.text.italic))
    {
        if (format.text.italic)
        {
            *m_streamOut << "<i>";
        }
    }

    if (force || (formatOrigin.text.underline!=format.text.underline))
    {
        if (format.text.underline)
        {
            *m_streamOut << "<u>";
        }
    }

    if (force || (formatOrigin.text.strikeout!=format.text.strikeout))
    {
        if (format.text.strikeout)
        {
            *m_streamOut << "<s>";
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
}

void HtmlBasicWorker::closeFormatData(const FormatData& formatOrigin,
    const FormatData& format, const bool force,const bool allowBold)
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

    if (force || (formatOrigin.text.strikeout!=format.text.strikeout))
    {
        if (format.text.strikeout)
        {
            *m_streamOut << "</s>";
        }
    }

    if (force || (formatOrigin.text.underline!=format.text.underline))
    {
        if (format.text.underline)
        {
            *m_streamOut << "</u>";
        }
    }

    if (force || (formatOrigin.text.italic!=format.text.italic))
    {
        if (format.text.italic)
        {
            *m_streamOut << "</i>";
        }
    }

    if (force || ((formatOrigin.text.weight>=75)!=(format.text.weight>=75)))
    {
        if (allowBold && (format.text.weight >= 75))
        {
            *m_streamOut << "</b>";
        }
    }

    bool fontName =  ( force || formatOrigin.text.fontName != format.text.fontName ) &&
                       !format.text.fontName.isEmpty();
    bool fontSize =  ( force || formatOrigin.text.fontSize != format.text.fontSize ) &&
                       format.text.fontSize>0;
    bool fontColor = ( force ||formatOrigin.text.fgColor != format.text.fgColor ) &&
                       format.text.fgColor.isValid();

    if( ( m_cssURL.isEmpty() && ( fontName || fontSize ) ) || fontColor )
    {
        *m_streamOut << "</font>";
    }

}

void HtmlBasicWorker::openParagraph(const QString& strTag,
    const LayoutData& layout, QChar::Direction direction)
{
    *m_streamOut << '<' << strTag;

    if ( (layout.alignment=="left") || (layout.alignment== "right")
        || (layout.alignment=="center") || (layout.alignment=="justify"))
    {
        *m_streamOut << " align=\"" << layout.alignment << "\"";
        if ( (direction == QChar::DirRLE) || (direction == QChar::DirRLO) )
          *m_streamOut << " dir=\"rtl\"";
    }
    else if ( layout.alignment=="auto")
    {
        // Do nothing, the user-agent should be more intelligent than us.
    }
    else
    {
        kWarning(30503) << "Unknown alignment: " << layout.alignment << endl;
    }

    *m_streamOut << ">";

    // Allow bold only if tag is not a heading!
    openFormatData(layout.formatData,layout.formatData,true,(strTag[0]!='h'));
}

void HtmlBasicWorker::closeParagraph(const QString& strTag,
    const LayoutData& layout)
{
     // Allow bold only if tag is not a heading!
    closeFormatData(layout.formatData,layout.formatData,true,(strTag[0]!='h'));

    *m_streamOut << "</" << strTag << ">\n";
}

void HtmlBasicWorker::openSpan(const FormatData& formatOrigin, const FormatData& format)
{
    openFormatData(formatOrigin,format,false,true);
}

void HtmlBasicWorker::closeSpan(const FormatData& formatOrigin, const FormatData& format)
{
    closeFormatData(formatOrigin,format,false,true);
}

bool HtmlBasicWorker::doOpenBody(void)
{
    // Define the background color as white!
    *m_streamOut << "<body bgcolor=\"#FFFFFF\">\n";
    return true;
}

QString HtmlBasicWorker::customCSSURL(void) const
{
  return m_cssURL;
}
