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

#include <KWEFUtil.h>
#include <KWEFBaseWorker.h>

#include "ExportFilter.h"

QString HtmlWorker::escapeHtmlText(const QString& strText) const
{
    // Escape quotes (needed in attributes)
    // Do not escape apostrophs (only allowed in XHTML!)
    return KWEFUtil::EscapeSgmlText(getCodec(),strText,true,false);
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

    if (loadKoStoreFile(anchor.picture.koStoreName,image))
    {
        QFile file(strImageName);

        if ( !file.open (IO_WriteOnly) )
        {
            kdError(30503) << "Unable to open image output file!" << endl;
            return false;
        }

        file.writeBlock(image);
        file.close();

        *m_streamOut << "<img "; // This is an emüty element!
        *m_streamOut << "src=\"" << strImageName << "\" ";
        *m_streamOut << "alt=\"" << escapeHtmlText(anchor.picture.key) << "\"";
        *m_streamOut << (isXML()?"/>":">") << "\n";
    }
    else
    {
        kdDebug(30503) << "Unable to load image " << anchor.picture.koStoreName << endl;
    }

    return true;
}

void HtmlWorker::formatTextParagraph(const QString& strText, const FormatData& format)
{
    if (format.text.missing)
    {
        //Format is not issued from KWord. Therefore it is only the layout
        // So it is only the text
        if (strText==" ")
        {//Just a space as text. Therefore we must use a non-breaking space.
            *m_streamOut << "&nbsp;";
            // FIXME: only needed for <p>&nbsp;</p>, but not for </span> <span>
        }
        else
        {
            *m_streamOut << escapeHtmlText(strText);
        }
    }
    else
    {
        // TODO: first and last characters of partialText should not be a space (white space problems!)
        // TODO: replace multiples spaces by non-breaking spaces!
        // Opening elements
        openSpan(format);

        if (!format.text.linkName.isEmpty())
        {
            *m_streamOut << "<a href=\"";
            *m_streamOut << escapeHtmlText(format.text.linkReference);
            *m_streamOut << "\">";
        }

        // The text
        if (strText==" ")
        {//Just a space as text. Therefore we must use a non-breaking space.
            *m_streamOut << "&nbsp;";
        }
        else
        {
            *m_streamOut << escapeHtmlText(strText);
        }

        // Closing elements
        if (!format.text.linkName.isEmpty())
        {
            *m_streamOut << "</a>";
        }

        closeSpan(format);
    }
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
                formatTextParagraph(partialText,*paraFormatDataIt);
            }
            else if (6==(*paraFormatDataIt).id)
            {
                // We have an image or a table

                // But first, we have a problem, as we must close the paragraph
                closeParagraph(strTag,layout);

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

                // We can re-open the paragraph
                openParagraph(strTag,layout);
            }
        }
    }
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

    openParagraph(strTag,layout);
    ProcessParagraphData(strTag, strParaText, layout, paraFormatDataList);
    closeParagraph(strTag,layout);

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

    if (!getCodec())
    {
        kdError(30503) << "Could not create QTextCodec! Aborting" << endl;
        return false;
    }

    kdDebug(30503) << "Charset used: " << getCodec()->name() << endl;

    m_streamOut->setCodec( getCodec() );

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
            << getCodec()->mimeName() << "\"?>" << endl;
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
    *m_streamOut << getCodec()->mimeName() << '"';
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
