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

#include <qstring.h>
#include <qregexp.h>
#include <qtextcodec.h>
#include <qfile.h>

#include <klocale.h>
#include <kdebug.h>

#include <KWEFUtil.h>
#include <KWEFBaseWorker.h>
#include <KWEFKWordLeader.h>

#include "ExportFilter.h"

QString HtmlWorker::escapeHtmlText(const QString& strText) const
{
    // Escape quotes (needed in attributes)
    // Do not escape apostrophs (only allowed in XHTML!)
    return KWEFUtil::EscapeSgmlText(m_codec,strText,true,false);
}


QString HtmlWorker::escapeCssIdentifier(const QString& strText) const
{
    // Reference: section 4.1.3 of the CSS2 recommendation
    // NOTE: when we need to escape, we choose the numerical CSS escape as it is encoding neutral.

    QString strReturn;

    for (uint i=0; i<strText.length(); i++)
    {
        const QChar ch=strText[i];
        if (((ch>='a') && (ch<='z'))
            || ((ch>='A') && (ch<='Z')))
        {
            strReturn+=ch;
        }
        else if (((ch>='0') && (ch<='9'))
            || (ch=='-'))
        {
            if (!i)
            {
                // A digit or a hyphen is not allowed as first character of an identifier
                //  therefore we must escape it
                strReturn+='\\'; // start escape
                strReturn+=QString::number(ch.unicode(),16);
                strReturn+=' '; // end escape (the space is not part of the following text!)
            }
            else
            {
                strReturn+=ch;
            }
        }
        else if ((ch>=QChar(161)) && (m_codec->canEncode(ch)))
        {
            // Any Unicode character greater or egual to 161 is allowed too, even at start.
            // Except if the encoding cannot write the character
            strReturn+=ch;
        }
        else
        {
            // We have a non-acceptable character, so escape it!
            strReturn+='\\'; // start escape
            strReturn+=QString::number(ch.unicode(),16);
            strReturn+=' '; // end escape (the space is not part of the following text!)
        }
    }
    return strReturn;
}

void HtmlWorker::ProcessParagraphData ( QString &paraText, ValueListFormatData &paraFormatDataList, QString &outputText)
{
    if (! paraText.isEmpty() )
    {

        ValueListFormatData::Iterator  paraFormatDataIt;  //Warning: cannot use "->" with it!!

        QString partialText;

        for ( paraFormatDataIt = paraFormatDataList.begin ();
              paraFormatDataIt != paraFormatDataList.end ();
              paraFormatDataIt++ )
        {
            //Retrieve text
            partialText=paraText.mid ( (*paraFormatDataIt).pos, (*paraFormatDataIt).len );

            if ((*paraFormatDataIt).missing)
            {   //Format is not issued from KWord. Therefore is only the layout
                // So it is only the text
                if (partialText==" ")
                {//Just a space as text. Therefore we must use a non-breaking space.
                    outputText += "&nbsp;";
                    // FIXME: only needed for <p>&nbsp;</p>, but not for </span> <span>
                }
                else
                {
                    //Code all possible predefined HTML entities
                    outputText += escapeHtmlText(partialText);
                }
                continue; // And back to the loop
            }

            // TODO: first and last characters of partialText should not be a space (white space problems!)
            // TODO: replace multiples spaces in non-breaking spaces!
            // Opening elements
            outputText+="<span style=\"";

            // Font name
            QString fontName = (*paraFormatDataIt).fontName;
            if ( !fontName.isEmpty() )
            {
                outputText+="font-family: ";
                outputText+=fontName; // TODO: add alternative font names
                outputText+="; ";
            }

            // Font style
            outputText+="font-style: ";
            if ( (*paraFormatDataIt).italic )
            {
                outputText+="italic";
            }
            else
            {
                outputText+="normal";
            }
            outputText+="; ";

            outputText+="font-weight: ";
            if ( (*paraFormatDataIt).weight >= 75 )
            {
                outputText+="bold";
            }
            else
            {
                outputText+="normal";
            }
            outputText+="; ";

            const int size=(*paraFormatDataIt).fontSize;
            if (size>0)
            {
                // We use absolute font sizes.
                outputText+="font-size: ";
                outputText+=QString::number(size,10);
                outputText+="pt; ";
            }

            if ( (*paraFormatDataIt).colour.isValid() )
            {
                // Give colour
                outputText+="color: ";
                // QColor::name() does all the job :)
                outputText+=(*paraFormatDataIt).colour.name();
                outputText+="; ";
            }

            outputText+="text-decoration: ";
            if ( (*paraFormatDataIt).underline )
            {
                outputText+="underline";
            }
            else if ( (*paraFormatDataIt).strikeout )
            {
                outputText+="line-through";
            }
            else
            {
                outputText+="none";
            }
            //outputText+="; ";
            outputText+="\">"; // close span opening tag
            if ( 1==(*paraFormatDataIt).verticalAlignment )
            {
                outputText+="<sub>"; //Subscript
            }
            if ( 2==(*paraFormatDataIt).verticalAlignment )
            {
                outputText+="<sup>"; //Superscript
            }
            if (!(*paraFormatDataIt).linkName.isEmpty())
            {
                outputText+="<a href=\"";
                outputText+=escapeHtmlText((*paraFormatDataIt).linkReference);
                outputText+="\">";
            }

            // The text
            if (outputText==" ")
            {//Just a space as text. Therefore we must use a non-breaking space.
                outputText += "&nbsp;";
            }
            else
            {
                //Code all possible predefined HTML entities
                outputText += escapeHtmlText(partialText);
            }

            // Closing elements
            if (!(*paraFormatDataIt).linkName.isEmpty())
            {
                outputText+="</a>";
            }
            if ( 2==(*paraFormatDataIt).verticalAlignment )
            {
                outputText+="</sup>"; //Superscript
            }
            if ( 1==(*paraFormatDataIt).verticalAlignment )
            {
                outputText+="</sub>"; //Subscript
            }
            outputText+="</span>";
        }
    }
}

QString HtmlWorker::getStartOfListOpeningTag(const CounterData::Style typeList, bool& ordered)
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
    case CounterData::STYLE_NONE:
        {
            m_orderedList=false;
            strResult="<ul style=\"list-style-type:none\">\n";
            break;
        }
    case CounterData::STYLE_CIRCLEBULLET:
        {
            m_orderedList=false;
            strResult="<ul style=\"list-style-type:circle\">\n";
            break;
        }
    case CounterData::STYLE_SQUAREBULLET:
        {
            m_orderedList=false;
            strResult="<ul style=\"list-style-type:square\">\n";
            break;
        }
    case CounterData::STYLE_DISCBULLET:
        {
            m_orderedList=false;
            strResult="<ul style=\"list-style-type:disc\">\n";
            break;
        }
    case CounterData::STYLE_NUM:
        {
            m_orderedList=true;
            strResult="<ol style=\"list-style-type:decimal\">\n";
            break;
        }
    case CounterData::STYLE_ALPHAB_L:
        {
            m_orderedList=true;
            strResult="<ol style=\"list-style-type:lower-alpha\">\n";
            break;
        }
    case CounterData::STYLE_ALPHAB_U:
        {
            m_orderedList=true;
            strResult="<ol style=\"list-style-type:upper-alpha\">\n";
            break;
        }
    case CounterData::STYLE_ROM_NUM_L:
        {
            m_orderedList=true;
            strResult="<ol style=\"list-style-type:lower-roman\">\n";
            break;
        }
    case CounterData::STYLE_ROM_NUM_U:
        {
            m_orderedList=true;
            strResult="<ol style=\"list-style-type:upper-roman\">\n";
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

QString HtmlWorker::layoutToCss(LayoutData& layout) const
{
    QString strElement; // TODO: rename this variable
    // We do not set "left" explicitly, since KWord cannot do bi-di
    if (( layout.alignment== "right") || (layout.alignment=="center") || (layout.alignment=="justify"))
    {
        strElement+=QString("text-align:%1;").arg(layout.alignment);
    }

    if ( layout.indentLeft!=0.0 )
    {
        strElement+=QString("margin-left:%1pt;").arg(layout.indentLeft);
    }

    if ( layout.indentRight!=0.0 )
    {
        strElement+=QString("margin-right:%1pt;").arg(layout.indentRight);
    }

    if ( layout.indentFirst!=0.0 )
    {
        strElement+=QString("text-indent:%1pt;").arg(layout.indentFirst);
    }

    // Font name
    QString fontName = layout.formatData.fontName;
    if ( !fontName.isEmpty() )
    {
        strElement+="font-family: ";
        strElement+=fontName; // TODO: add alternative font names
        strElement+="; ";
    }

    // Font style
    strElement+="font-style: ";
    if ( layout.formatData.italic )
    {
        strElement+="italic";
    }
    else
    {
        strElement+="normal";
    }
    strElement+="; ";

    strElement+="font-weight: ";
    if ( layout.formatData.weight >= 75 )
    {
        strElement+="bold";
    }
    else
    {
        strElement+="normal";
    }
    strElement+="; ";

    const int size=layout.formatData.fontSize;
    if (size>0)
    {
        // We use absolute font sizes.
        strElement+="font-size: ";
        strElement+=QString::number(size,10);
        strElement+="pt; ";
    }

    if ( layout.formatData.colour.isValid() )
    {
        // Give colour
        strElement+="color: ";
        // QColor::name() does all the job :)
        strElement+=layout.formatData.colour.name();
        strElement+="; ";
    }

    strElement+="text-decoration: ";
    if ( layout.formatData.underline )
    {
        strElement+="underline";
    }
    else if ( layout.formatData.strikeout )
    {
        strElement+="line-through";
    }
    else
    {
        strElement+="none";
    }
    return strElement;
}

bool HtmlWorker::doFullParagraph(QString& paraText, LayoutData& layout, ValueListFormatData& paraFormatDataList)
{
    QString strParaText=paraText;
    QString strTag; // Tag that will be written.

    if (strParaText.isEmpty())
    {
        //An empty paragraph is not allowed in HTML, so add a non-breaking space!
        strParaText="&nbsp;";
    }

    // As KWord has only one depth of lists, we can process lists very simply.
    if ( layout.counter.numbering == CounterData::NUM_LIST )
    {
        if (m_inList)
        {
            // We are in a list but does it have the right type?
            if ( layout.counter.style!=m_typeList)
            {
                // No, then close the previous list
                if (m_orderedList)
                {
                    *m_streamOut << "</ol>\n";
                }
                else
                {
                    *m_streamOut << "</ul>\n";
                }
                m_inList=false; // We are not in a list anymore
            }
        }

        // Are we still in a list?
        if (!m_inList)
        {
            // We are not yet part of a list
            m_inList=true;
            *m_streamOut << getStartOfListOpeningTag(layout.counter.style,m_orderedList);
            m_typeList=layout.counter.style;
        }
        // TODO: with Cascaded Style Sheet, we could add the exact counter type we want
        strTag="li";
    }
    else
    {
        if (m_inList)
        {
            // The previous paragraphs were in a list, so we have to close it
            if (m_orderedList)
            {
                *m_streamOut << "</ol>\n";
            }
            else
            {
                *m_streamOut << "</ul>\n";
            }
            m_inList=false;
        }
        if ( layout.counter.numbering == CounterData::NUM_CHAPTER )
        {
            strTag=QString("h%1").arg(layout.counter.depth + 1);
        }
        else
        {
            strTag="p";
        }
    }

    *m_streamOut << '<' << strTag;

    // Opening elements
    *m_streamOut << " class=\"" << escapeCssIdentifier(layout.styleName);
    *m_streamOut << "\" style=\"" << layoutToCss(layout) << "\">";

    if ( 1==layout.formatData.verticalAlignment )
    {
        *m_streamOut << "<sub>"; //Subscript
    }
    if ( 2==layout.formatData.verticalAlignment )
    {
        *m_streamOut << "<sup>"; //Superscript
    }

    QString strText;
    ProcessParagraphData(strParaText, paraFormatDataList, strText);
    *m_streamOut << strText;

    if ( 2==layout.formatData.verticalAlignment )
    {
        *m_streamOut << "</sup>"; //Superscript
    }
    if ( 1==layout.formatData.verticalAlignment )
    {
        *m_streamOut << "</sub>"; //Subscript
    }

    *m_streamOut << "</" << strTag << ">\n";

    return true;
}

bool HtmlWorker::doOpenFile(const QString& filenameOut, const QString& to)
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
    if (!m_ioDevice)
    {
        kdError(30503) << "Could not create output stream! Aborting!" << endl;
        m_ioDevice->close();
        return false;
    }

    // Find out IANA/mime charset name
    if ( isUTF8() )
    {
        m_codec=QTextCodec::codecForName("UTF-8");
    }
    else
    {
        m_codec=QTextCodec::codecForLocale();
    }

    kdDebug(30501) << "Charset used: " << m_codec->name() << endl;

    if (!m_codec)
    {
        kdError(30503) << "Could not create QTextCodec! Aborting" << endl;
        return false;
    }
        
    m_streamOut->setCodec( m_codec );

    // Make the default title
    const int result=filenameOut.findRev("/");
    if (result>=0)
    {
        m_strTitle=filenameOut.mid(result+1);
    }
    else
    {
        m_strTitle=filenameOut;
    }
    return true;
}

bool HtmlWorker::doCloseFile(void)
{
    if (m_ioDevice)
        m_ioDevice->close();
    return (m_ioDevice);
}

bool HtmlWorker::doOpenDocument(void)
{
    // Make the file header

    if (isXML())
    {   //Write out the XML declaration
        *m_streamOut << "<?xml version=\"1.0\" encoding=\""
            << m_codec->mimeName() << "\"?>" << endl;
    }

    // write <!DOCTYPE
    *m_streamOut << "<!DOCTYPE ";
    if (isXML())
    {
        *m_streamOut << "html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"DTD/xhtml1-strict.dtd\">\n";
    }
    else
    {
        *m_streamOut << "HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">\n";
    }

    // No "lang" or "xml:lang" attribute for <html>, as we do not know in which language the document is!
    *m_streamOut << "<html";
    if (isXML())
    {
        // XHTML has an extra attribute defining its namespace (in the <html> opening tag)
        *m_streamOut << " xmlns=\"http://www.w3.org/1999/xhtml\"";
    }
    *m_streamOut << ">\n";
    return true;
}

bool HtmlWorker::doCloseDocument(void)
{
    *m_streamOut << "</html>\n";
    return true;
}

bool HtmlWorker::doFullDocumentInfo(const KWEFDocumentInfo& docInfo)
{
    QString strText=docInfo.title;
    if (!strText.isEmpty())
    {
        m_strTitle=strText; // Set title only if it is not empty!
        kdDebug(30503) << "Found new title " << m_strTitle << endl;
    }
    return true;
}

bool HtmlWorker::doOpenHead(void)
{
    *m_streamOut << "<head>" << endl;

    // Declare what charset we are using
    *m_streamOut << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=";
    *m_streamOut << m_codec->mimeName() << '"';
    *m_streamOut << (isXML()?" /":"") << ">\n" ;

    // Say who we are (with the CVS revision number) in case we have a bug in our filter output!
    QString strVersion("$Revision$");
    // Eliminate the dollar signs
    //  (We don't want that the version number changes if the HTML file is itself put in a CVS storage.)
    *m_streamOut << "<meta name=\"Generator\" content=\"KWord HTML Export Filter Version"
              << strVersion.mid(10).replace(QRegExp("\\$"),"") // Note: double character (one for C++, one for QRegExp!)
              << "\""<< (isXML()?" /":"") // X(HT)ML closes empty elements, HTML not!
              << ">\n";

    if (m_strTitle.isEmpty())
    {
        // Somehow we have still an empty title (this should not happen!)
        kdWarning(30503) << "Title still empty! (HtmlWorker::doOpenHead)" << endl;
        m_strTitle=i18n("Untitled");
    }
    *m_streamOut << "<title>"<< escapeHtmlText(m_strTitle) <<"</title>\n";  // <TITLE> is mandatory!

    //TODO: transform documentinfo.xml into many <META> elements (at least the author!)

    return true;
}

bool HtmlWorker::doCloseHead(void)
{
    *m_streamOut << "</head>\n";
    return true;
}

bool HtmlWorker::doOpenBody(void)
{
    *m_streamOut << "<body>\n";
    return true;
}

bool HtmlWorker::doCloseBody(void)
{
    *m_streamOut << "</body>\n";
    return true;
}

bool HtmlWorker::doOpenTextFrameSet(void)
{
    *m_streamOut << "<div>\n"; // For compatibility with AbiWord's XHTML import filter
    return true;
}

bool HtmlWorker::doCloseTextFrameSet(void)
{
    // Are we still in a list?
    if (m_inList)
    {
        // We are in a list, so close it!
        if (m_orderedList)
        {
            *m_streamOut << "</ol>\n";
        }
        else
        {
            *m_streamOut << "</ul>\n";
        }
        m_inList=false;
    }
    *m_streamOut << "</div>\n"; // For compatibility with AbiWord's XHTML import filter
    return true;
}

bool HtmlWorker::doOpenStyles(void)
{
    *m_streamOut << "<style type=\"text/css\">\n";
    if (!isXML())
    {
        // Put the style under comments to increase the compatibility with old browsers
        // However in XHTML 1.0, you cannot put the style definition into HTML comments
        *m_streamOut << "<!--\n";
    }
    *m_streamOut << "BODY\n{\n background-color: #FFFFFF\n}\n";

    return true;
}

bool HtmlWorker::doFullDefineStyle(LayoutData& layout)
{
    kdDebug(30503) << "Style: " << layout.styleName << endl;

    if ( layout.counter.numbering == CounterData::NUM_CHAPTER )
    {
        *m_streamOut << "H" << QString::number(layout.counter.depth+1,10);
    }

    *m_streamOut << "." << escapeCssIdentifier(layout.styleName);
    kdDebug(30503) << "Class: " << escapeCssIdentifier(layout.styleName) << endl;
    *m_streamOut << "\n{\n " << layoutToCss(layout) << "\n}\n";

}

bool HtmlWorker::doCloseStyles(void)
{
    if (!isXML())
    {
        *m_streamOut << "-->\n";
    }
    *m_streamOut << "</style>\n";
    return true;
}
