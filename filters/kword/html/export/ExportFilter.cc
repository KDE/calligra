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

QString HtmlWorker::textFormatToCss(const TextFormatting& formatData) const
{
    // TODO: as this method comes from the AbiWord filter,
    // TODO:   verify that it is working for HTML

    // TODO: rename variable formatData
    QString strElement; // TODO: rename this variable

    // Font name
    QString fontName = formatData.fontName;
    if ( !fontName.isEmpty() )
    {
        strElement+="font-family: ";
        strElement+=fontName; // TODO: add alternative font names
        strElement+="; ";
    }

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
    if ( formatData.bgColor.isValid() )
    {
        // Give background colour
        strElement+="bgcolor: ";
        strElement+=formatData.bgColor.name();
        strElement+="; ";
    }

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
    // As this is the last property, do not put a semi-colon
    //strElement+="; ";

    // TODO: KWord's <SHADOW>

    return strElement;
}

bool HtmlWorker::makeTable(const FrameAnchor& anchor)
{
    *m_streamOut << "<table>\n";
    *m_streamOut << "<tbody>\n";

    QValueList<TableCell>::ConstIterator itCell;

    int rowCurrent=0;
    *m_streamOut << "<tr>\n";


    for (itCell=anchor.table.cellList.begin();
        itCell!=anchor.table.cellList.end(); itCell++)
    {
        // TODO: rowspan, colspan
        if (rowCurrent!=(*itCell).row)
        {
            rowCurrent=(*itCell).row;
            *m_streamOut << "</tr>\n<tr>\n";
        }

        *m_streamOut << "<td>\n";
        
        if (!doFullAllParagraphs(*(*itCell).paraList))
        {   
            return false;
        }

        *m_streamOut << "</td>\n";
    }

    *m_streamOut << "</tr>\n";
    *m_streamOut << "</tbody>\n";
    *m_streamOut << "</table>\n";
    
    return true;
}

bool HtmlWorker::makeImage(const FrameAnchor& anchor)
{
    *m_streamOut << "<img "; // This is an emüty element!

    QString strImageName=m_fileName;
    strImageName+='.';

    const int result=anchor.picture.koStoreName.findRev("/");
    if (result>=0)
    {
        strImageName+=anchor.picture.koStoreName.mid(result+1);
    }
    else
    {
        strImageName+=anchor.picture.koStoreName;
    }

    QByteArray image;

    // TODO: verify return value!
    if (m_kwordLeader)
        m_kwordLeader->loadKoStoreFile(anchor.picture.koStoreName,image);

    // TODO: test if file is empty and abort!
    QFile file(strImageName);

    if ( !file.open (IO_WriteOnly) )
    {
        kdError(30503) << "Unable to open image output file!" << endl;
        return false;
    }

    file.writeBlock(image);
    file.close();

    *m_streamOut << "src=\"" << strImageName << "\" ";
    *m_streamOut << "alt=\"" << escapeHtmlText(anchor.picture.key) << "\"";
    *m_streamOut << (isXML()?"/>":">") << "\n";

    return true;
}

QString HtmlWorker::getFormatTextParagraph(const QString& strText, const FormatData& format)
{
    QString outputText;
    if (format.text.missing)
    {
        //Format is not issued from KWord. Therefore it is only the layout
        // So it is only the text
        if (strText==" ")
        {//Just a space as text. Therefore we must use a non-breaking space.
            outputText += "&nbsp;";
            // FIXME: only needed for <p>&nbsp;</p>, but not for </span> <span>
        }
        else
        {
            outputText += escapeHtmlText(strText);
        }
    }
    else
    {
        // TODO: first and last characters of partialText should not be a space (white space problems!)
        // TODO: replace multiples spaces by non-breaking spaces!
        // Opening elements
        outputText+="<span style=\"";

        outputText+=textFormatToCss(format.text);

        outputText+="\">"; // close span opening tag

        if ( 1==format.text.verticalAlignment )
        {
            outputText+="<sub>"; //Subscript
        }
        if ( 2==format.text.verticalAlignment )
        {
            outputText+="<sup>"; //Superscript
        }
        if (!format.text.linkName.isEmpty())
        {
            outputText+="<a href=\"";
            outputText+=escapeHtmlText(format.text.linkReference);
            outputText+="\">";
        }

        // The text
        if (strText==" ")
        {//Just a space as text. Therefore we must use a non-breaking space.
            outputText += "&nbsp;";
        }
        else
        {
            outputText += escapeHtmlText(strText);
        }

        // Closing elements
        if (!format.text.linkName.isEmpty())
        {
            outputText+="</a>";
        }
        if ( 2==format.text.verticalAlignment )
        {
            outputText+="</sup>"; //Superscript
        }
        if ( 1==format.text.verticalAlignment )
        {
            outputText+="</sub>"; //Subscript
        }
        outputText+="</span>";
    }
    return outputText;
}

void HtmlWorker::ProcessParagraphData (const QString& strTag, const QString &paraText,
    const LayoutData& layout, const ValueListFormatData &paraFormatDataList)
{
    if (! paraText.isEmpty() )
    {

        ValueListFormatData::ConstIterator  paraFormatDataIt;

        QString partialText;

        for ( paraFormatDataIt = paraFormatDataList.begin ();
              paraFormatDataIt != paraFormatDataList.end ();
              paraFormatDataIt++ )
        {
            if (1==(*paraFormatDataIt).id)
            {
                //Retrieve text
                partialText=paraText.mid ( (*paraFormatDataIt).pos, (*paraFormatDataIt).len );
                *m_streamOut << getFormatTextParagraph(partialText,*paraFormatDataIt);
            }
            else if (6==(*paraFormatDataIt).id)
            {
                // We have an image or a table
                // But first, we have a problem, as we must close the paragraph
                //     like in layout (very annoying (TODO: common code))
                if ( 2==layout.formatData.text.verticalAlignment )
                {
                    *m_streamOut << "</sup>"; //Superscript
                }
                if ( 1==layout.formatData.text.verticalAlignment )
                {
                    *m_streamOut << "</sub>"; //Subscript
                }
                *m_streamOut << "</" << strTag << ">\n";

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

                // And re-open everything like in layout (very annoying (TODO: common code))
                *m_streamOut << "<" << strTag;
                *m_streamOut << " class=\"" << escapeCssIdentifier(layout.styleName);
                *m_streamOut << "\" style=\"" << layoutToCss(layout) << "\">";
                if ( 1==layout.formatData.text.verticalAlignment )
                {
                    *m_streamOut << "<sub>"; //Subscript
                }
                if ( 2==layout.formatData.text.verticalAlignment )
                {
                    *m_streamOut << "<sup>"; //Superscript
                }
            }
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

QString HtmlWorker::layoutToCss(const LayoutData& layout) const
{
    QString strLayout;

    // We do not set "left" explicitly, since KWord cannot do bi-di
    //  (FIXME/TODO: H'm, that is no more true!)
    if (( layout.alignment== "right") || (layout.alignment=="center") || (layout.alignment=="justify"))
    {
        strLayout+=QString("text-align:%1; ").arg(layout.alignment);
    }

    // FIXME/TODO: H'm, why is 0.0 not a valid value?

    if ( layout.indentLeft!=0.0 )
    {
        strLayout+=QString("margin-left:%1pt; ").arg(layout.indentLeft);
    }

    if ( layout.indentRight!=0.0 )
    {
        strLayout+=QString("margin-right:%1pt; ").arg(layout.indentRight);
    }

    if ( layout.indentFirst!=0.0 )
    {
        strLayout+=QString("text-indent:%1pt; ").arg(layout.indentFirst);
    }

    if( layout.marginBottom!=0.0)
    {
        strLayout += QString("margin-bottom:%1pt; ").arg(layout.marginBottom);
    }
    if( layout.marginTop!=0.0  )
    {
        strLayout += QString("margin-top:%1pt; ").arg(layout.marginTop);
    }

    if ( !layout.lineSpacingType )
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

    // This must remain last, as the last property does not have a semi-colon
    strLayout+=textFormatToCss(layout.formatData.text);

    return strLayout;
}

bool HtmlWorker::doFullParagraph(const QString& paraText,
    const LayoutData& layout, const ValueListFormatData& paraFormatDataList)
{
    QString strParaText=paraText;
    QString strTag; // Tag that will be written.

    if (strParaText.isEmpty())
    {
        //An empty paragraph is not allowed in HTML, so add a non-breaking space!
        strParaText=QChar(160);
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
        // TODO: with Cascaded Style Sheet, we could add the exact counter type that we want
        strTag="li";
    }
    else
    {
        if (m_inList)
        {
            // The previous paragraphs were in a list, so we have to close the list
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

    *m_streamOut << '<' << strTag;

    // Opening elements
    *m_streamOut << " class=\"" << escapeCssIdentifier(layout.styleName);
    *m_streamOut << "\" style=\"" << layoutToCss(layout) << "\">";

    if ( 1==layout.formatData.text.verticalAlignment )
    {
        *m_streamOut << "<sub>"; //Subscript
    }
    if ( 2==layout.formatData.text.verticalAlignment )
    {
        *m_streamOut << "<sup>"; //Superscript
    }

    ProcessParagraphData(strTag, strParaText, layout, paraFormatDataList);

    if ( 2==layout.formatData.text.verticalAlignment )
    {
        *m_streamOut << "</sup>"; //Superscript
    }
    if ( 1==layout.formatData.text.verticalAlignment )
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

    m_fileName=filenameOut;

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
        m_strTitle=i18n("Untitled Document");
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
        // Put the style under comment to increase the compatibility with old browsers
        // However in XHTML 1.0, you cannot put the style definition into HTML comments
        *m_streamOut << "<!--\n";
    }
    // TODO: does KWord gives a paper colour?
    *m_streamOut << "BODY\n{\n  background-color: #FFFFFF\n}\n";

    return true;
}

bool HtmlWorker::doFullDefineStyle(LayoutData& layout)
{
    // We do not limit (anymore) any style to <h1> ... <h6>, because
    //   the style could be forced on <p> by the layout.

    *m_streamOut << "." << escapeCssIdentifier(layout.styleName);
    *m_streamOut << "\n{\n  " << layoutToCss(layout) << "\n}\n";

    return true;
}

bool HtmlWorker::doCloseStyles(void)
{
    if (!isXML())
    {
        // Put the style under comment to increase the compatibility with old browsers
        // However in XHTML 1.0, you cannot put the style definition into HTML comments
        *m_streamOut << "-->\n";
    }
    *m_streamOut << "</style>\n";
    return true;
}
