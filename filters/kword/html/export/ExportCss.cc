// $Header$

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qstring.h>
#include <qtextcodec.h>
#include <qfile.h>

#include <klocale.h>
#include <kdebug.h>

#include <KWEFUtil.h>
#include <KWEFBaseWorker.h>

#include "ExportFilter.h"
#include "ExportCss.h"

QString HtmlCssWorker::escapeCssIdentifier(const QString& strText) const
{
    // Reference: section 4.1.3 of the CSS2 recommendation
    // NOTE: when we need to escape, we choose the numerical CSS escape as it is encoding neutral.
    // But be careful, we cannot have spaces in identifiers, even after CSS escapes
    // Therefore we must take care that the character after the CSS escape is not part of it.

    QString strReturn;
    bool lastEscaped=false; // Was the last character escaped?

    for (uint i=0; i<strText.length(); i++)
    {
        const QChar ch(strText[i]);

        if (lastEscaped
            && (((ch>='a') && (ch<='f'))
            || ((ch>='A') && (ch<='F'))
            || ((ch>='0') && (ch<='9')) ))
        {
             // The previous characters was escaped and the current character could be an hexadecimal digit
             // Therefore we need to escape it too.
            strReturn+='\\'; // start escape
            strReturn+=QString::number(ch.unicode(),16);
            lastEscaped=true;
        }
        else if (((ch>='a') && (ch<='z'))
            || ((ch>='A') && (ch<='Z')))
        {
            strReturn+=ch;
            lastEscaped=false;
        }
        else if (((ch>='0') && (ch<='9'))
            || (ch=='-') || (ch=='_')) // The underscore is allowed in the CSS2 errata
        {
            if (!i)
            {
                // A digit or a hyphen is not allowed as first character of an identifier
                //  therefore we must escape it
                strReturn+='\\'; // start escape
                strReturn+=QString::number(ch.unicode(),16);
                lastEscaped=true;
            }
            else
            {
                strReturn+=ch;
                lastEscaped=false;
            }
        }
        else if ((ch>=QChar(161)) && (getCodec()->canEncode(ch)))
        {
            // Any Unicode character greater or egual to 161 is allowed too, even at start.
            // Except if the encoding cannot write the character
            strReturn+=ch;
            lastEscaped=false;
        }
        else
        {
            // We have a non-acceptable character, so escape it!
            strReturn+='\\'; // start escape
            strReturn+=QString::number(ch.unicode(),16);
            lastEscaped=true;
        }
    }
    // It is supposed that a last escape do not need an extra space
    return strReturn;
}

QString HtmlCssWorker::textFormatToCss(const TextFormatting& formatOrigin,
    const TextFormatting& formatData, const bool force) const
{
    // TODO: as this method comes from the AbiWord filter,
    // TODO:   verify that it is working for HTML

    // TODO: rename variable formatData
    QString strElement; // TODO: rename this variable

    // Font name
    QString fontName = formatData.fontName;
    if (!fontName.isEmpty()
        && (force || (formatOrigin.fontName!=formatData.fontName)))
    {
        strElement+="font-family: ";
        strElement+=escapeHtmlText(fontName); // TODO: add alternative font names
        strElement+="; ";
    }

    if (force || (formatOrigin.italic!=formatData.italic))
    {
        // Font style
        strElement+="font-style: ";
        if ( formatData.italic )
        {
            strElement+="italic";
        }
        else
        {
            strElement+="normal";
        }
        strElement+="; ";
    }

    if (force || ((formatOrigin.weight>=75)!=(formatData.weight>=75)))
    {
        strElement+="font-weight: ";
        if ( formatData.weight >= 75 )
        {
            strElement+="bold";
        }
        else
        {
            strElement+="normal";
        }
        strElement+="; ";
    }

    if (force || (formatOrigin.fontSize!=formatData.fontSize))
    {
        const int size=formatData.fontSize;
        if (size>0)
        {
            // We use absolute font sizes.
            strElement+="font-size: ";
            strElement+=QString::number(size,10);
            strElement+="pt; ";
        }
    }

    if (force || (formatOrigin.fgColor!=formatData.fgColor))
    {
        if ( formatData.fgColor.isValid() )
        {
            // Give colour
            strElement+="color: ";
            strElement+=formatData.fgColor.name();
            strElement+="; ";
        }
    }

    if (force || (formatOrigin.bgColor!=formatData.bgColor))
    {
        if ( formatData.bgColor.isValid() )
        {
            // Give background colour
            strElement+="bgcolor: ";
            strElement+=formatData.bgColor.name();
            strElement+="; ";
        }
    }

    if (force || (formatOrigin.underline!=formatData.underline)
        || (formatOrigin.strikeout!=formatData.strikeout))
    {
        strElement+="text-decoration: ";
        if ( formatData.underline )
        {
            strElement+="underline";
        }
        else if ( formatData.strikeout )
        {
            strElement+="line-through";
        }
        else
        {
            strElement+="none";
        }
        strElement+="; ";
    }

    if (force || (formatOrigin.fontAttribute!=formatData.fontAttribute))
    {
        bool smallcaps=false;
        strElement+="text-transform: ";
        if ( formatData.fontAttribute=="uppercase" )
        {
            strElement+="uppercase";
        }
        else if ( formatData.fontAttribute=="lowercase" )
        {
            strElement+="lowercase";
        }
        else if ( formatData.fontAttribute=="smallcaps" )
        {
            strElement+="none";
            smallcaps=true;
        }
        else
        {
            strElement+="none";
        }
        strElement+="; ";
        // ### TODO: mostly issuing font-variant is not necessary.
        strElement+="font-variant:";
        if (smallcaps)
            strElement+="small-caps";
        else
            strElement+="none";
        strElement+="; ";
    }

    // TODO: As this is the last property, do not put a semi-colon

    return strElement;
}

QString HtmlCssWorker::getStartOfListOpeningTag(const CounterData::Style typeList, bool& ordered)
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
    case CounterData::STYLE_NONE:
        {
            ordered=false;
            strResult="<ul style=\"list-style-type:none\">\n";
            break;
        }
    case CounterData::STYLE_CIRCLEBULLET:
        {
            ordered=false;
            strResult="<ul style=\"list-style-type:circle\">\n";
            break;
        }
    case CounterData::STYLE_SQUAREBULLET:
        {
            ordered=false;
            strResult="<ul style=\"list-style-type:square\">\n";
            break;
        }
    case CounterData::STYLE_DISCBULLET:
        {
            ordered=false;
            strResult="<ul style=\"list-style-type:disc\">\n";
            break;
        }
    case CounterData::STYLE_NUM:
        {
            ordered=true;
            strResult="<ol style=\"list-style-type:decimal\">\n";
            break;
        }
    case CounterData::STYLE_ALPHAB_L:
        {
            ordered=true;
            strResult="<ol style=\"list-style-type:lower-alpha\">\n";
            break;
        }
    case CounterData::STYLE_ALPHAB_U:
        {
            ordered=true;
            strResult="<ol style=\"list-style-type:upper-alpha\">\n";
            break;
        }
    case CounterData::STYLE_ROM_NUM_L:
        {
            ordered=true;
            strResult="<ol style=\"list-style-type:lower-roman\">\n";
            break;
        }
    case CounterData::STYLE_ROM_NUM_U:
        {
            ordered=true;
            strResult="<ol style=\"list-style-type:upper-roman\">\n";
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

QString HtmlCssWorker::layoutToCss(const LayoutData& layoutOrigin,
    const LayoutData& layout, const bool force) const
{
    QString strLayout;

    if (force || (layoutOrigin.alignment!=layout.alignment))
    {
        if ( (layout.alignment=="left") || (layout.alignment== "right")
            || (layout.alignment=="center") || (layout.alignment=="justify"))
        {
            strLayout += QString("text-align:%1; ").arg(layout.alignment);
        }
        else if ( layout.alignment=="auto")
        {
            // Do nothing, the user-agent should be more intelligent than us.
        }
        else
        {
            kdWarning(30503) << "Unknown alignment: " << layout.alignment << endl;
        }
    }

    if ((layout.indentLeft>=0.0)
        && (force || (layoutOrigin.indentLeft!=layout.indentLeft)))
    {
            strLayout += QString("margin-left:%1pt; ").arg(layout.indentLeft);
    }

    if ((layout.indentRight>=0.0)
        && (force || (layoutOrigin.indentRight!=layout.indentRight)))
    {
        strLayout += QString("margin-right:%1pt; ").arg(layout.indentRight);
    }

    if (force || (layoutOrigin.indentLeft!=layout.indentLeft))
    {
        strLayout += QString("text-indent:%1pt; ").arg(layout.indentFirst);
    }

    if ((layout.marginBottom>=0.0)
        && (force || (layoutOrigin.indentRight!=layout.indentRight)))
    {
       strLayout += QString("margin-bottom:%1pt; ").arg(layout.marginBottom);
    }

    if ((layout.marginTop>=0.0)
        && (force || (layoutOrigin.indentRight!=layout.indentRight)))
    {
       strLayout += QString("margin-top:%1pt; ").arg(layout.marginTop);
    }

    // TODO: Konqueror/KHTML does not support "line-height"
    if (!force
        && (layoutOrigin.lineSpacingType==layoutOrigin.lineSpacingType)
        && (layoutOrigin.lineSpacing==layoutOrigin.lineSpacing))
    {
        // Do nothing!
    }
    else if ( !layout.lineSpacingType )
    {
        // We have a custom line spacing (in points)
        strLayout += QString("line-height:%1pt; ").arg(layout.lineSpacing);
    }
    else if ( 15==layout.lineSpacingType  )
    {
        strLayout += "line-height:1.5; "; // One-and-half
    }
    else if ( 20==layout.lineSpacingType  )
    {
        strLayout += "line-height:2.0; "; // Two
    }
    else if ( layout.lineSpacingType!=10  )
    {
        kdWarning(30503) << "Curious lineSpacingType: " << layout.lineSpacingType << " (Ignoring!)" << endl;
    }

    // TODO: Konqueror/KHTML does not support "text-shadow"
    if (!force
        && (layoutOrigin.shadowDirection==layoutOrigin.shadowDirection)
        && (layoutOrigin.shadowDistance==layoutOrigin.shadowDistance))
    {
        // Do nothing!
    }
    else if ((!layout.shadowDirection) || (!layout.shadowDistance))
    {
        strLayout += "text-shadow:";
        strLayout+="none; ";
    }
    else
    {
        double xDistance,yDistance;
        const double distance=layout.shadowDistance;
        switch (layout.shadowDirection)
        {
        case 1: // SD_LEFT_UP
            {
                xDistance= (-distance);
                yDistance= (-distance);
                break;
            }
        case 2: // SD_UP
            {
                xDistance= 0;
                yDistance= (-distance);
                break;
            }
        case 3: // SD_RIGHT_UP
            {
                xDistance= (distance);
                yDistance= (-distance);
                break;
            }
        case 4: // SD_RIGHT
            {
                xDistance= (distance);
                yDistance= 0;
                break;
            }
        case 5: // SD_RIGHT_BOTTOM
            {
                xDistance= (distance);
                yDistance= (distance);
                break;
            }
        case 6: // SD_BOTTOM
            {
                xDistance= 0;
                yDistance= (distance);
                break;
            }
        case 7: // SD_LEFT_BOTTOM
            {
                xDistance= (-distance);
                yDistance= (distance);
                break;
            }
        case 8: // SD_LEFT
            {
                xDistance= (distance);
                yDistance= 0;
                break;
            }
        default:
            {
                xDistance=0;
                yDistance=0;
                break;
            }
        }
        if ( (!xDistance) && (!yDistance) )
        {
            strLayout += "text-shadow:";
            strLayout+="none; ";
        }
        else
        {
            strLayout += "text-shadow:";
            strLayout+=QString("%1 %2pt %3pt; ").arg(layout.shadowColor.name())
                .arg(xDistance,0,'f',0).arg(yDistance,0,'f',0);
                // We do not want any scientific notation or any decimal
        }
    }

    // TODO: borders

    // This must remain last, as the last property does not have a semi-colon
    strLayout+=textFormatToCss(layoutOrigin.formatData.text,
        layout.formatData.text,force);

    return strLayout;
}

void HtmlCssWorker::openParagraph(const QString& strTag,
    const LayoutData& layout, QChar::Direction direction)
{
    const LayoutData& styleLayout=m_styleMap[layout.styleName];

    *m_streamOut << '<' << strTag;

    // Opening elements
    *m_streamOut << " class=\"" << escapeCssIdentifier(layout.styleName);
    *m_streamOut << "\"";

    QString strStyle=layoutToCss(styleLayout,layout,false);
    if (!strStyle.isEmpty())
    {
        *m_streamOut << " style=\"" << strStyle;
        if (direction == QChar::DirRLE) {
            *m_streamOut << "direction: rtl; unicode-bidi: embed; ";
        } else if (direction == QChar::DirRLO) {
            *m_streamOut << "direction: rtl; unicode-bidi: override; ";
        }
        *m_streamOut<< "\"";
    }

    *m_streamOut << ">";

    if ( 1==layout.formatData.text.verticalAlignment )
    {
        *m_streamOut << "<sub>"; //Subscript
    }
    else if ( 2==layout.formatData.text.verticalAlignment )
    {
        *m_streamOut << "<sup>"; //Superscript
    }
}

void HtmlCssWorker::closeParagraph(const QString& strTag,
    const LayoutData& layout)
{
    if ( 2==layout.formatData.text.verticalAlignment )
    {
        *m_streamOut << "</sup>"; //Superscript
    }
    else if ( 1==layout.formatData.text.verticalAlignment )
    {
        *m_streamOut << "</sub>"; //Subscript
    }

    *m_streamOut << "</" << strTag << ">\n";
}

void HtmlCssWorker::openSpan(const FormatData& formatOrigin, const FormatData& format)
{
    *m_streamOut << "<span style=\"";
    *m_streamOut << textFormatToCss(formatOrigin.text,format.text,false);
    *m_streamOut << "\">"; // close span opening tag

    if ( 1==format.text.verticalAlignment )
    {
        *m_streamOut << "<sub>"; //Subscript
    }
    else if ( 2==format.text.verticalAlignment )
    {
        *m_streamOut << "<sup>"; //Superscript
    }
}

void HtmlCssWorker::closeSpan(const FormatData& formatOrigin, const FormatData& format)
{
    if ( 2==format.text.verticalAlignment )
    {
        *m_streamOut << "</sup>"; //Superscript
    }
    else if ( 1==format.text.verticalAlignment )
    {
        *m_streamOut << "</sub>"; //Subscript
    }

    *m_streamOut << "</span>";
}

bool HtmlCssWorker::doFullPaperFormat(const int format,
            const double width, const double height, const int orientation)
{
    QString strWidth, strHeight, strUnits;
    KWEFUtil::GetNativePaperFormat(format, strWidth, strHeight, strUnits);

    if ((strWidth.isEmpty())||(strHeight.isEmpty())||(strUnits.isEmpty()))
    {
        // page format is unknown, so we need the size information
        strUnits="pt";
        strWidth=QString::number(width);
        strHeight=QString::number(height);
    }
    if (orientation==1)
    {
        // Landscape, so we must swap the sizes
        QString strTemp(strWidth);
        strWidth=strHeight;
        strHeight=strTemp;
    }

    m_strPageSize="size: ";
    m_strPageSize+=strWidth;
    m_strPageSize+=strUnits;
    m_strPageSize+=" ";
    m_strPageSize+=strHeight;
    m_strPageSize+=strUnits;
    m_strPageSize+=";";
    return true;
}

bool HtmlCssWorker::doFullPaperBorders (const double top, const double left,
    const double bottom, const double right)
{
    m_strPaperBorders="  margin-top: ";
    m_strPaperBorders+=QString::number(top);
    m_strPaperBorders+="pt;\n";
    m_strPaperBorders+="  margin-left: ";
    m_strPaperBorders+=QString::number(left);
    m_strPaperBorders+="pt;\n";
    m_strPaperBorders+="  margin-bottom: ";
    m_strPaperBorders+=QString::number(bottom);
    m_strPaperBorders+="pt;\n";
    m_strPaperBorders+="  margin-right: ";
    m_strPaperBorders+=QString::number(right);
    m_strPaperBorders+="pt;\n";

    return true;
}

bool HtmlCssWorker::doOpenStyles(void)
{
    *m_streamOut << "<style type=\"text/css\">\n";
    if (!isXML())
    {
        // Put the style under comment to increase the compatibility with old browsers
        // However in XHTML 1.0, you cannot put the style definition into HTML comments
        *m_streamOut << "<!--\n";
    }
    // TODO: does KWord gives a paper colour?
    *m_streamOut << "BODY\n{\n  background-color: #FFFFFF\n}\n";

    return true;
}

bool HtmlCssWorker::doFullDefineStyle(LayoutData& layout)
{
    //Register style in the style map
    m_styleMap[layout.styleName]=layout;

    // We do not limit (anymore) any style to <h1> ... <h6>, because
    //   the style could be forced on <p> by the layout.

    *m_streamOut << "." << escapeCssIdentifier(layout.styleName);
    *m_streamOut << "\n{\n  " << layoutToCss(layout,layout,true) << "\n}\n";

    return true;
}

bool HtmlCssWorker::doCloseStyles(void)
{
    if (!m_strPageSize.isEmpty())
    {
        *m_streamOut << "@page\n{\n  ";
        *m_streamOut << m_strPageSize;
        *m_streamOut << "\n";
        *m_streamOut << m_strPaperBorders; // ends with a LF
        *m_streamOut << "}\n";
    }

    if (!isXML())
    {
        // Put the style under comment to increase the compatibility with old browsers
        // However in XHTML 1.0, you cannot put the style definition into HTML comments
        *m_streamOut << "-->\n";
    }
    *m_streamOut << "</style>\n";

    return true;
}

