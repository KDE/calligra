// $Header$

/*
   This file is part of the KDE project
   Copuright 2001 Michael Johnson <mikej@xnet.com>
   Copyright 2001, 2002 Nicolas GOUTTE <nicog@snafu.de>

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
#include <qfileinfo.h>
#include <qfontinfo.h>
#include <qpicture.h>
#include <qregion.h> // for #include <kdebugclasses.h>

#include <klocale.h>
#include <kdebug.h>
#include <kdebugclasses.h>

#include <KWEFUtil.h>
#include <KWEFBaseWorker.h>

#include "ExportFilter.h"

bool RTFWorker::makeTable(const FrameAnchor& anchor)
{

    QValueList<TableCell>::ConstIterator itCell;

    int rowCurrent=0;
    m_textBody += "\\row";
    m_textBody += m_eol;
    m_inTable=true;

    for (itCell=anchor.table.cellList.begin();
        itCell!=anchor.table.cellList.end(); itCell++)
    {
        // ### TODO: rowspan, colspan
        if (rowCurrent!=(*itCell).row)
        {
            rowCurrent=(*itCell).row;
            m_textBody += "\\row";
            m_textBody += m_eol;
        }
        m_textBody += "\\trowd \\trgaph60 \\trleft-60";  // start new row

        if (!doFullAllParagraphs(*(*itCell).paraList))
        {
            return false;
        }

        m_textBody += "\\cell";
    }
    m_inTable=false;

    m_textBody += "\\row\\par";  // delimit last row
    m_textBody += m_eol;

    return true;
}

bool RTFWorker::makeImage(const FrameAnchor& anchor)
{
#if 0
    QString strImageName(anchor.picture.koStoreName);

    QString strImagePath(strImageName);

    QByteArray image;

    kdDebug(30503) << "Image " << anchor.picture.koStoreName << " will be written in " << strImageName << endl;

    if (loadKoStoreFile(anchor.picture.koStoreName,image))
    {
        QFile file(strImagePath);

        if ( !file.open (IO_WriteOnly) )
        {
            kdError(30503) << "Unable to open image output file!" << endl;
            return false;
        }

        file.writeBlock(image);
        file.close();

        m_textBody += "<img "; // This is an empty element!
        m_textBody += "src=\"" + strImageName + "\" ";
        m_textBody += "alt=\"" + anchor.picture.key.filename() + "\"";
        m_textBody += ">\n";
        kdDebug(30503) << "Image written" << endl;
    }
    else
    {
        kdWarning(30503) << "Unable to load image " << anchor.picture.koStoreName << endl;
    }
#endif
    return true;
}

void RTFWorker::formatTextParagraph(const QString& strText,
 const FormatData& formatOrigin, const FormatData& format)
{
    QString strEscaped(escapeRtfText(strText));

    // Replace line feeds by forced line breaks
    int pos;
    QString strBr("\\line "); // ### TODO: verify
    while ((pos=strEscaped.find(QChar(10)))>-1)
    {
        strEscaped.replace(pos,1,strBr);
    }

    if (!format.text.missing)
    {
        // Opening elements
        openSpan(formatOrigin,format);
    }

    m_textBody += strEscaped;

    if (!format.text.missing)
    {
        // Closing elements
        closeSpan(formatOrigin,format);
    }
}

void RTFWorker::ProcessParagraphData (const QString& strTag, const QString &paraText,
    const LayoutData& layout, const ValueListFormatData &paraFormatDataList)
{
    if (paraText.isEmpty())
    {
        // ### TODO: verify if a paragragh can be empty in RTF
        openParagraph(layout);
        closeParagraph(layout);
    }
    else
    {
        bool paragraphNotOpened=true;

        ValueListFormatData::ConstIterator  paraFormatDataIt;

        QString partialText;

        for ( paraFormatDataIt = paraFormatDataList.begin ();
              paraFormatDataIt != paraFormatDataList.end ();
              paraFormatDataIt++ )
        {
            if (1==(*paraFormatDataIt).id)
            {
                // For normal text, we need an opened paragraph
                if (paragraphNotOpened)
                {
                    openParagraph(layout);
                    paragraphNotOpened=false;
                }
                //Retrieve text
                partialText=paraText.mid ( (*paraFormatDataIt).pos, (*paraFormatDataIt).len );
                formatTextParagraph(partialText,layout.formatData,*paraFormatDataIt);
            }
            else if (4==(*paraFormatDataIt).id)
            {
                // For variables, we need an opened paragraph
                if (paragraphNotOpened)
                {
                    openParagraph(layout);
                    paragraphNotOpened=false;
                }
                if (0==(*paraFormatDataIt).variable.m_type) // variable date
                {
                   // ### TODO: fixed date
                   m_textBody += "\\chdate";
                }
                else if (2==(*paraFormatDataIt).variable.m_type) // variable time
                {
                   // ### TODO: fixed time
                   m_textBody += "\\chtime";
                }
                else if (4==(*paraFormatDataIt).variable.m_type)
                {
                    QString strFieldType;
                    if ((*paraFormatDataIt).variable.isPageNumber())
                    {
                        m_textBody += "\\chpgn ";
                    }
#if 0
                    else if ((*paraFormatDataIt).variable.isPageCount())
                    {
                        m_textBody += "page_count";
                    }
#endif
                    else
                    {
                        // Unknown subtype, therefore write out the result
                        m_textBody += escapeRtfText((*paraFormatDataIt).variable.m_text);
                    }
                }
#if 0
                else if (9==(*paraFormatDataIt).variable.m_type)
                {
                    // A link
                    m_textBody += "<a href=\""
                        + escapeRtfText((*paraFormatDataIt).variable.getHrefName())
                        + "\">"
                        + escapeRtfText((*paraFormatDataIt).variable.getLinkName())
                        + "</a>";
                }
#endif
                else
                {
                    // Generic variable
                    m_textBody += escapeRtfText((*paraFormatDataIt).variable.m_text);
                }
            }
            else if (6==(*paraFormatDataIt).id)
            {
                // We have an image, a clipart or a table

                // But first, we must sure that the paragraph is not opened.
                if (!paragraphNotOpened)
                {
                    // The paragraph was opened, so close it.
                    closeParagraph(layout);
                }

                if (6==(*paraFormatDataIt).frameAnchor.type)
                {
                    makeTable((*paraFormatDataIt).frameAnchor);
                }
                else if (2==(*paraFormatDataIt).frameAnchor.type)
                {
                    makeImage((*paraFormatDataIt).frameAnchor);
                }
                else
                {
                    kdWarning(30503) << "Unknown anchor type: "
                        << (*paraFormatDataIt).frameAnchor.type << endl;
                }

                // The paragraph will need to be opened again
                paragraphNotOpened=true;
            }
        }
        if (!paragraphNotOpened)
        {
            // The paragraph was opened, so close it.
            closeParagraph(layout);
        }
    }
}

bool RTFWorker::doFullParagraph(const QString& paraText,
    const LayoutData& layout, const ValueListFormatData& paraFormatDataList)
{
    kdDebug(30503) << "Entering RTFWorker::doFullParagraph" << endl << paraText << endl;
    QString strParaText=paraText;
    QString strTag; // Tag that will be written.

    if ( layout.counter.numbering == CounterData::NUM_LIST )
    {
        const uint layoutDepth=layout.counter.depth+1; // Word's depth starts at 0!
        const uint listDepth=m_listStack.size();
        // We are in a list, but has it the right depth?
        if (layoutDepth>listDepth)
        {
            ListInfo newList;
            newList.m_typeList=layout.counter.style;
            for (uint i=listDepth; i<layoutDepth; i++)
            {
                m_textBody += getStartOfListOpeningTag(layout.counter.style,newList.m_orderedList);
                m_listStack.push(newList);
            }
        }
        else if (layoutDepth<listDepth)
        {
            for (uint i=listDepth; i>layoutDepth; i--)
            {
                ListInfo oldList=m_listStack.pop();
                if (oldList.m_orderedList)
                {
                    m_textBody += "</ol>\n";
                }
                else
                {
                    m_textBody += "</ul>\n";
                }
            }
        }

        // We have a list but does it have the right type?
        if ( layout.counter.style!=m_listStack.top().m_typeList)
        {
            // No, then close the previous list
            ListInfo oldList=m_listStack.pop();
            if (oldList.m_orderedList)
            {
                m_textBody += "</ol>\n";
            }
            else
            {
                m_textBody += "</ul>\n";
            }
            ListInfo newList;
            m_textBody += getStartOfListOpeningTag(layout.counter.style,newList.m_orderedList);
            newList.m_typeList=layout.counter.style;
            m_listStack.push(newList);
        }

        // TODO: with Cascaded Style Sheet, we could add the exact counter type that we want
        strTag="li";
    }
    else
    {
        // Close all open lists first
        if (!m_listStack.isEmpty())
        {
            for (uint i=m_listStack.size(); i>0; i--)
            {
                ListInfo oldList=m_listStack.pop();
                if (oldList.m_orderedList)
                {
                    m_textBody += "</ol>\n";
                }
                else
                {
                    m_textBody += "</ul>\n";
                }
            }
        }
        if ( (layout.counter.numbering == CounterData::NUM_CHAPTER)
            && (layout.counter.depth<6) )
        {
            strTag=QString("h%1").arg(layout.counter.depth + 1); // H1 ... H6
        }
        else
        {
            strTag="p";
        }
    }

    ProcessParagraphData(strTag, strParaText, layout, paraFormatDataList);

    kdDebug(30503) << "Quiting RTFWorker::doFullParagraph" << endl;
    return true;
}

bool RTFWorker::doOpenFile(const QString& filenameOut, const QString& /*to*/)
{
    m_ioDevice=new QFile(filenameOut);

    if (!m_ioDevice)
    {
        kdError(30503) << "No output file! Aborting!" << endl;
        return false;
    }

    if ( !m_ioDevice->open (IO_WriteOnly) )
    {
        kdError(30503) << "Unable to open output file!" << endl;
        return false;
    }

    m_streamOut=new QTextStream(m_ioDevice);

    m_streamOut->setEncoding(QTextStream::UnicodeUTF8); // We want ASCII, so use UTF-8

    m_fileName=filenameOut;

    return true;
}

bool RTFWorker::doCloseFile(void)
{
    kdDebug(30503) << __FILE__ << ":" << __LINE__ << endl;
    delete m_streamOut;
    m_streamOut=NULL;
    if (m_ioDevice)
        m_ioDevice->close();
    return true;
}

bool RTFWorker::doOpenDocument(void)
{
    // Make the file header
    *m_streamOut << "{\\rtf1\\ansi\\ansicpg1200 \\uc0 \\deff0" << m_eol;

    // Default color table
    m_colorList << QColor(0,0,0)     << QColor(0,0,255)     << QColor(0,255,255)
                << QColor(0,255,0)   << QColor(255,0,255)   << QColor(255,0,0)
                << QColor(255,255,0) << QColor(255,255,255) << QColor(0,0,128)
                << QColor(0,128,128) << QColor(0,128,0)     << QColor(128,0,128)
                << QColor(128,0,0)   << QColor(128,128,0)   << QColor(128,128,128);

    return true;
}

void RTFWorker::writeFontData(void)
{
    kdDebug() << "Fonts:" << m_fontList << endl;
    *m_streamOut << "{\\fonttbl";
    uint count;
    QStringList::ConstIterator it;
    for (count=0, it=m_fontList.begin();
        it!=m_fontList.end();
        count++, it++)
    {
        QFont info(*it);
        QString strLower(info.family().lower());
        *m_streamOut << "{\\f" << count;
        if ( (strLower.find("symbol")>-1) || (strLower.find("dingbat")>-1) )
            *m_streamOut << "\\ftech";
        else if ( (strLower.find("script")>-1) || (strLower.find("chancery")>-1) )
            *m_streamOut << "\\fscript";
        else
        {
            switch (info.styleHint())
            {
            case QFont::SansSerif:
            default:
                {
                    *m_streamOut << "\\fswiss";
                    break;
                }
            case QFont::Serif:
                {
                    *m_streamOut << "\\froman";
                    break;
                }
            case QFont::Courier:
                {
                    *m_streamOut << "\\fmodern";
                    break;
                }
            case QFont::OldEnglish:
                {
                    *m_streamOut << "\\fdecor";
                    break;
                }
            }
        }
        *m_streamOut << "\\fcharset0\\fprq2 "; // font definition
        *m_streamOut << escapeRtfText(info.family()); // ### TODO: does RTF allows brackets in the font names?
        *m_streamOut <<  ";}" << m_eol; // end font table entry
    }
    *m_streamOut << "}"; // end of font table
}

void RTFWorker::writeColorData(void)
{
    *m_streamOut << "{\\colortbl";
    uint count;
    QValueList<QColor>::ConstIterator it;
    for (count=0, it=m_colorList.begin();
        it!=m_colorList.end();
        count++, it++)
    {
        *m_streamOut << "\\red" << (*it).red();
        *m_streamOut << "\\green" << (*it).green();
        *m_streamOut << "\\blue" << (*it).blue();
        *m_streamOut <<  ";"; // end of entry
    }
    *m_streamOut << "}"; // end of color table
}

void RTFWorker::writeStyleData(void)
{
    *m_streamOut << "{\\stylesheet" << m_eol;

    uint count;
    QValueList<LayoutData>::ConstIterator it;
    for (count=0, it=m_styleList.begin();
        it!=m_styleList.end();
        count++, it++)
    {
        if (count>0) // \s0 is not written out
            *m_streamOut << "\\s" << count;

        // Find the number of the following style
        uint counter=0;  // counts position in style table starting at 0
        QValueList < LayoutData > ::ConstIterator it2;
        for( it2 =  m_styleList.begin(); it2 != m_styleList.end(); counter++, it2++ )
        {
            if ( (*it2).styleName == (*it).styleFollowing )
            {
                *m_streamOut << "\\snext" << counter;
                break;
            }
        }

        *m_streamOut << layoutToRtf((*it),(*it),true);
        *m_streamOut << " " << (*it).styleName << ";";
        *m_streamOut << m_eol;
    }

    *m_streamOut << "}";
}

bool RTFWorker::doCloseDocument(void)
{

    writeFontData();
    writeColorData();
    writeStyleData();

    *m_streamOut << m_textDocInfo;   // add document author, title, operator
    *m_streamOut << "\\paperw" << m_paperWidth;
    *m_streamOut << "\\paperh" << m_paperHeight;
    if (1==m_paperOrientation)
        *m_streamOut << "\\landscape";
    *m_streamOut << "\\margl" << m_paperMarginLeft;
    *m_streamOut << "\\margr" << m_paperMarginRight;
    *m_streamOut << "\\margt" << m_paperMarginTop;
    *m_streamOut << "\\margb" << m_paperMarginBottom;
    *m_streamOut << m_textPage;  // add page size, margins, etc.
    *m_streamOut << "\\widowctrl\\ftnbj\\aenddoc\\formshade \\fet0\\sectd\n";
    *m_streamOut << "\\linex0\\endnhere\\plain";
    *m_streamOut << m_textBody;

    *m_streamOut << "}" << m_eol;
    return true;
}

bool RTFWorker::doFullDocumentInfo(const KWEFDocumentInfo& docInfo)
{
    m_textDocInfo += "{\\info ";  // string of document information markup

    if ( !docInfo.title.isEmpty() )
    {
        m_textDocInfo += "{\\title ";
        m_textDocInfo += docInfo.title;
        m_textDocInfo += "}";
    }

    if ( !docInfo.fullName.isEmpty() )
    {
        m_textDocInfo += "{\\author ";
        m_textDocInfo += docInfo.fullName;
        m_textDocInfo += "}";
        m_textDocInfo += "{\\operator ";
        m_textDocInfo += docInfo.fullName;
        m_textDocInfo += "}";
    }

    if ( !docInfo.abstract.isEmpty() )
    {
        m_textDocInfo += "{\\subject ";
        m_textDocInfo += docInfo.abstract;
        m_textDocInfo += "}";
    }

    if ( !docInfo.company.isEmpty() )
    {
        m_textDocInfo += "{\\company ";
        m_textDocInfo += docInfo.company;
        m_textDocInfo += "}";
    }

    m_textDocInfo += "}";

    return true;
}

bool RTFWorker::doOpenTextFrameSet(void)
{
    return true;
}

bool RTFWorker::doCloseTextFrameSet(void)
{
    if (!m_listStack.isEmpty())
    {
        for (uint i=m_listStack.size(); i>0; i--)
        {
            ListInfo oldList=m_listStack.pop();
            if (oldList.m_orderedList)
            {
                m_textBody += "</ol>\n";
            }
            else
            {
                m_textBody += "</ul>\n";
            }
        }
    }
    return true;
}

void RTFWorker::openParagraph(const LayoutData& layout)
{
    m_textBody += "\\pard";
    if (m_inTable)
        m_textBody += "\\intbl";
    m_textBody += "{";
    LayoutData styleLayout;
    m_textBody += lookupStyle(layout.styleName, styleLayout);
    m_textBody += layoutToRtf(styleLayout,layout,false);

    if ( 1==layout.formatData.text.verticalAlignment )
    {
        m_textBody += "\\sub"; //Subscript
    }
    else if ( 2==layout.formatData.text.verticalAlignment )
    {
        m_textBody += "\\super"; //Superscript
    }
    m_textBody += " ";
}

void RTFWorker::closeParagraph(const LayoutData& layout)
{
    m_textBody += m_eol;
    m_textBody += "\\par";

    m_textBody += "}";
}

void RTFWorker::openSpan(const FormatData& formatOrigin, const FormatData& format)
{
    m_textBody += "{";
    m_textBody += textFormatToRtf(formatOrigin.text,format.text,false);

    if ( 1==format.text.verticalAlignment )
    {
        m_textBody += "\\sub"; //Subscript
    }
    else if ( 2==format.text.verticalAlignment )
    {
        m_textBody += "\\super"; //Superscript
    }

    m_textBody += " ";
}

void RTFWorker::closeSpan(const FormatData& formatOrigin, const FormatData& format)
{
    m_textBody += "}";
}

QString RTFWorker::getStartOfListOpeningTag(const CounterData::Style typeList, bool& ordered)
{
    return QString::null;
}

// The following function encodes the kword unicode characters into
// RTF seven bit ASCII. This affects any 8 bit characters.
// They are encoded either with \' or with \u
QString RTFWorker::escapeRtfText ( const QString& text ) const
{
    // initialize strings
    QString escapedText;
    const uint length = text.length();
    for ( uint i = 0; i < length; i++ )
    {
        QChar QCh ( text.at( i ) );  // get out one unicode char from the string
        const ushort ch = QCh.unicode();  // take unicode value of the char

        if ( ch >= 32 && ch <= 126) // ASCII character
            escapedText += QCh;
        else if ( QCh == '\\' )  escapedText += "\\\\"; // back-slash
        else if ( QCh == '{' )   escapedText += "\\{";
        else if ( QCh == '}' )   escapedText += "\\}";
        else if ( ch == 0x0009 ) escapedText += "\\tab "; // tabulator
        else if ( ch == 0x00a0 ) escapedText += "\\~"; // Non-breaking space
        else if ( ch == 0x00ad ) escapedText += "\\-"; // Soft hyphen
        else if ( ch == 0x00b7 ) escapedText += "\\|";
        else if ( ch == 0x2011 ) escapedText += "\\_"; // Non-breaking hyphen
        else if ( ch == 0x2002 ) escapedText += "\\enspace ";
        else if ( ch == 0x2003 ) escapedText += "\\emspace ";
        else if ( ch == 0x2004 ) escapedText += "\\qmspace ";
        else if ( ch == 0x200c ) escapedText += "\\zwnj ";
        else if ( ch == 0x200d ) escapedText += "\\zwj ";
        else if ( ch == 0x200e ) escapedText += "\\ltrmark ";
        else if ( ch == 0x200f ) escapedText += "\\rtrmark ";
        else if ( ch == 0x2013 ) escapedText += "\\endash ";
        else if ( ch == 0x2014 ) escapedText += "\\emdash ";
        else if ( ch == 0x2018 ) escapedText += "\\lquote ";
        else if ( ch == 0x2019 ) escapedText += "\\rquote ";
        else if ( ch == 0x201c ) escapedText += "\\ldblquote ";
        else if ( ch == 0x201d ) escapedText += "\\rdblquote ";
        else if ( ch == 0x2022 ) escapedText += "\\bullet ";
        else if ( false ) // check if the (non-ASCII) character would be in the codepage
        {
            // ### TODO: how do we check that we can use \'
            escapedText += "\\\'";   // escape upper page character to 7 bit
            escapedText += QString::number ( ch, 16 );
        }
        else if ( ch >= 127 ) // check for a non-ASCII character (127 is already non-ASCII)
        {
            escapedText += "\\u";
            escapedText += QString::number ( ch, 10 );
            escapedText += " ";
        }
        else
            escapedText += QCh ;

    }

    return escapedText;

}

bool RTFWorker::doFullPaperFormat(const int /*format*/,
    const double width, const double height, const int orientation)
{
    m_paperWidth=width*20;
    m_paperHeight=height*20;
    m_paperOrientation=orientation;
    return true;
}

bool RTFWorker::doFullPaperBorders (const double top, const double left,
    const double bottom, const double right)
{
    m_paperMarginTop=top*20;
    m_paperMarginLeft=left*20;
    m_paperMarginBottom=bottom*20;
    m_paperMarginRight=right*20;
    return true;
}

bool RTFWorker::doFullDefineStyle(LayoutData& layout)
{
    //Register the new style in the style list
    m_styleList << layout;

    // Now we must register a few things (with help of the lookup methofs.)
    lookupFont(layout.formatData.text.fontName);
    lookupColor(QString::null, layout.formatData.text.fgColor);
    lookupColor(QString::null, layout.formatData.text.bgColor);

    return true;
}

QString RTFWorker::textFormatToRtf(const TextFormatting& formatOrigin,
    const TextFormatting& formatData, const bool force)
{
    // TODO: rename variable formatData
    QString strElement; // TODO: rename this variable

    // Font name // ### TODO: lookup
    const QString fontName(formatData.fontName);
    if (!fontName.isEmpty()
        && (force || (formatOrigin.fontName!=formatData.fontName)))
    {
        strElement+=lookupFont(fontName);
    }

    if (force || (formatOrigin.fontSize!=formatData.fontSize))
    {
        const int size=formatData.fontSize;
        if (size>0)
        {
            // We use absolute font sizes.
            strElement+="\\fs";
            strElement+=QString::number(2*size,10);
        }
    }

    if (force || (formatOrigin.italic!=formatData.italic))
    {
        // Font style
        if ( formatData.italic )
        {
            strElement+="\\i1";
        }
        else
        {
            strElement+="\\i0";
        }
    }

    if (force || ((formatOrigin.weight>=75)!=(formatData.weight>=75)))
    {
        if ( formatData.weight >= 75 )
        {
            strElement+="\\b1";
        }
        else
        {
            strElement+="\\b0";
        }
    }

    if (force || (formatOrigin.fgColor!=formatData.fgColor))
    {
        if ( formatData.fgColor.isValid() )
        {
            strElement+=lookupColor("\\cf", fontName);
        }
    }

    if (force || (formatOrigin.bgColor!=formatData.bgColor))
    {
        if ( formatData.bgColor.isValid() )
        {
            strElement+=lookupColor("\\cb", fontName);
        }
    }

    if (force || (formatOrigin.underline!=formatData.underline))
    {
        if ( formatData.underline )
        {
            strElement+="\\ul1";
        }
        else
        {
            strElement+="\\ul0";
        }
    }

    if (force || (formatOrigin.strikeout!=formatData.strikeout))
    {
        if ( formatData.strikeout )
        {
            strElement+="\\strike1";
        }
        else
        {
            strElement+="\\strike0";
        }
    }

    return strElement;
}

QString RTFWorker::layoutToRtf(const LayoutData& layoutOrigin,
    const LayoutData& layout, const bool force)
{
    QString strLayout;


    if (force || (layoutOrigin.alignment!=layout.alignment))
    {
        if (layout.alignment=="left")
            strLayout += "\\ql";
        else if (layout.alignment== "right")
            strLayout += "\\qr";
        else if (layout.alignment=="center")
            strLayout += "\\qc";
        else if (layout.alignment=="justify")
            strLayout += "\\qj";
        else if ( layout.alignment=="auto")
        {
            // ### TODO: what for BIDI?
            strLayout += "\\ql";
        }
        else
        {
            kdWarning(30503) << "Unknown alignment: " << layout.alignment << endl;
        }
    }

    if ((layout.indentLeft>=0.0)
        && (force || (layoutOrigin.indentLeft!=layout.indentLeft)))
    {
       strLayout += "\\li";
       strLayout += QString::number(int(layout.indentLeft)*20, 10);
    }

    if ((layout.indentRight>=0.0)
        && (force || (layoutOrigin.indentRight!=layout.indentRight)))
    {
       strLayout += "\\ri";
       strLayout += QString::number(int(layout.indentRight)*20, 10);
    }

    if (force || (layoutOrigin.indentLeft!=layout.indentLeft))
    {
       strLayout += "\\fi";
       strLayout += QString::number(int(layout.indentFirst)*20, 10);
    }
    if ((layout.marginBottom>=0.0)
        && (force || (layoutOrigin.indentRight!=layout.indentRight)))
    {
       strLayout += "\\sa";
       strLayout += QString::number(int(layout.marginBottom)*20 ,10);
    }

    if ((layout.marginTop>=0.0)
        && (force || (layoutOrigin.indentRight!=layout.indentRight)))
    {
       strLayout += "\\sb";
       strLayout += QString::number(int(layout.marginTop)*20, 10);
    }
#if 0
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
#endif
    // TODO: borders

    // This must remain last, as the last property does not have a semi-colon
    strLayout+=textFormatToRtf(layoutOrigin.formatData.text,
        layout.formatData.text,force);

    return strLayout;
}


QString RTFWorker::lookupFont(const QString& fontName)
{
    kdDebug() << "RTFWorker::lookupFont " << fontName << endl;
    if (fontName.isEmpty())
        return QString::null;

    uint counter=0;  // counts position in font table (starts at 0)
    QString strFont("\\f"); // markup for font selection
    QStringList::ConstIterator it;

    // search font table for this font
    for( it = m_fontList.begin(); it != m_fontList.end(); counter++, it++ )
    {
        if((*it) == fontName)  // check for match
        {
            strFont += QString::number(counter);
            kdDebug() << strFont << endl;
            return strFont;
        }
    }  // end for()

    kdDebug() << "New font: " << fontName << " count: " << counter << endl;
    m_fontList << fontName;

    strFont += QString::number(counter);
    return strFont;
}

QString RTFWorker::lookupColor(const QString& markup, const QColor& color)
{
    if (!color.isValid())
        return QString::null;

    uint counter=1;  // counts position in color table starting at 1
    QString strColor(markup);  // Holds RTF markup for the color

    QValueList < QColor > ::ConstIterator it;

    // search color table for this color
    for( it =  m_colorList.begin(); it != m_colorList.end(); counter++, it++ )
    {
        if ( (*it) == color )
        {
            strColor += QString::number(counter);
            return strColor;
        }
    }

    kdDebug() << "New color: " << color.name() << " count: " << counter << endl;
    m_colorList << color;

    strColor += QString::number(counter);
    return strColor;
}

QString RTFWorker::lookupStyle(const QString& styleName, LayoutData& returnLayout)
{
    if (styleName.isEmpty())
        return QString::null;

    uint counter=0;  // counts position in style table starting at 0
    QString strMarkup("\\s");  // Holds RTF markup for the style

    QValueList < LayoutData > ::ConstIterator it;

    // search color table for this color
    for( it =  m_styleList.begin(); it != m_styleList.end(); counter++, it++ )
    {
        if ( (*it).styleName == styleName )
        {
            strMarkup += QString::number(counter);
            returnLayout=(*it);
            return strMarkup;
        }
    }

    kdDebug() << "New style: " << styleName << " count: " << counter << endl;
    LayoutData layout;
    m_styleList << layout;
    returnLayout=layout;

    strMarkup += QString::number(counter);
    return strMarkup;
}
