/*
   This file is part of the KDE project
   Copyright (C) 2001, 2002, 2004 Nicolas GOUTTE <goutte@kde.org>

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
#include <QFileInfo>
#include <QDir>
#include <q3picture.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <QTextStream>

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

    Q3ValueList<TableCell>::ConstIterator itCell;

    int rowCurrent=0;
    *m_streamOut << "<tr>\n";


    for (itCell=anchor.table.cellList.begin();
        itCell!=anchor.table.cellList.end(); itCell++)
    {
        if (rowCurrent!=(*itCell).row)
        {
            rowCurrent=(*itCell).row;
            *m_streamOut << "</tr>\n<tr>\n";
        }

        *m_streamOut << "<td";
        if ( (*itCell).m_rows > 1 )
            *m_streamOut << " rowspan=\"" << (*itCell).m_rows << "\"";
        if ( (*itCell).m_cols > 1 )
            *m_streamOut << " colspan=\"" << (*itCell).m_cols << "\"";
        *m_streamOut << ">\n";

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

QString HtmlWorker::getAdditionalFileName(const QString& additionalName)
{
    kDebug(30503) << "HtmlWorker::getAdditionalFileName " << additionalName << endl;

    QDir dir(m_strFileDir);
    kDebug(30503) << "Base directory: " << m_strFileDir << endl;

    if (!dir.exists(m_strSubDirectoryName))
    {
        // Make the directory, as it does not exist yet!
        kDebug(30503) << "Creating directory: " << m_strSubDirectoryName << endl;
        dir.mkdir(m_strSubDirectoryName);
    }

    QString strFileName(m_strSubDirectoryName);
    strFileName+="/";
    const int result=additionalName.findRev("/");
    if (result>=0)
    {
        strFileName+=additionalName.mid(result+1);
    }
    else
    {
        strFileName+=additionalName;
    }

    // Now, we have to create a backup file.

    QString strBackupName(strFileName);
    strBackupName+="~";
    kDebug(30503) << "Remove backup file: " << strBackupName << endl;
    // We need to remove the backup file, as not all filesystems or ports can do it themselves on a rename.
    dir.remove(strBackupName);
    kDebug(30503) << "Moving file: " << additionalName << " => " << strBackupName << endl;
    dir.rename(strFileName,strBackupName);

    return strFileName;
}

bool HtmlWorker::makeImage(const FrameAnchor& anchor)
{
    const QString strImageName(getAdditionalFileName(anchor.picture.koStoreName));

    QString strImagePath(m_strFileDir);
    strImagePath+='/';
    strImagePath+=strImageName;

    QByteArray image;

    kDebug(30503) << "Image " << anchor.picture.koStoreName << " will be written in " << strImageName << endl;

    if (loadSubFile(anchor.picture.koStoreName,image))
    {
        bool writePicture = false;

        const double height = anchor.frame.bottom - anchor.frame.top;
        const double width  = anchor.frame.right  - anchor.frame.left;

        const int pos = anchor.picture.koStoreName.findRev( '.' );
        QString extension;
        if ( pos > -1 )
            extension = anchor.picture.koStoreName.mid( pos+1 ).lower();

        if ( extension == "png" || extension == "jpeg" || extension == "jpg" || extension == "gif"
            || extension == "bmp" ) // A few file types known by all HTML user agents
        {
            *m_streamOut << "<img "; // This is an empty element!
            *m_streamOut << "src=\"" << escapeHtmlText(strImageName) << "\" ";
            *m_streamOut << "alt=\"" << escapeHtmlText(anchor.picture.key.filename()) << "\"";
            *m_streamOut << (isXML()?"/>":">");
            writePicture = true;
        }
        else if ( extension == "svg" )
        {
            // Save picture as SVG
            *m_streamOut << "<object data=\"" << escapeHtmlText(strImageName) << "\"";
            *m_streamOut << " type=\"image/svg+xml\"";
            *m_streamOut << " height=\"" << height << "\" width=\"" << width << "\">\n";
            *m_streamOut << "</object>"; // <object> is *not* an empty element in HTML!
            writePicture = true;
        }
        else if ( extension == "qpic" )
        {

            Q3Picture picture;

            QIODevice* io=getSubFileDevice(anchor.picture.koStoreName);
            if (!io)
            {
                // NO message error, as there must be already one
                return false;
            }

            // TODO: if we have alreasy SVG, do *not* go through QPicture!
            if (picture.load(io))
            {

                // Save picture as SVG
                *m_streamOut << "<object data=\"" << escapeHtmlText(strImageName) << "\"";
                *m_streamOut << " type=\"image/svg+xml\"";
                *m_streamOut << " height=\"" << height << "\" width=\"" << width << "\">\n";
                *m_streamOut << "</object>"; // <object> is *not* an empty element in HTML!
                // TODO: other props for image

                kDebug(30506) << "Trying to save clipart to " << strImageName << endl;
                if (!picture.save(strImagePath,"svg"))
                {
                    kError(30506) << "Could not save clipart: "  << anchor.picture.koStoreName
                        << " to " << strImageName << endl;
                    return false;
                }

            }
        }
        else
        {
            // ### TODO: avoid loading the picture 2 times
            image.resize( 0 );
            if ( ! loadAndConvertToImage( anchor.picture.koStoreName, extension, "PNG", image ) )
            {
                kWarning(30503) << "Could not convert picture to PNG!" << endl;
                return false;
            }
            *m_streamOut << "<img "; // This is an empty element!
            *m_streamOut << "src=\"" << escapeHtmlText(strImageName) << "\" ";
            *m_streamOut << "alt=\"" << escapeHtmlText(anchor.picture.key.filename()) << "\"";
            *m_streamOut << (isXML()?"/>":">");
            writePicture = true;
        }

        // Do we still need to write the original picture?
        if ( writePicture )
        {
            QFile file(strImagePath);

            if ( !file.open (QIODevice::WriteOnly) )
            {
                kError(30503) << "Unable to open image output file!" << endl;
                return false;
            }

            file.write(image);
            file.close();
        }
    }
    else
    {
        kWarning(30503) << "Unable to load picture " << anchor.picture.koStoreName << endl;
    }

    return true;
}

void HtmlWorker::formatTextParagraph(const QString& strText,
 const FormatData& formatOrigin, const FormatData& format)
{
    QString strEscaped(escapeHtmlText(strText));

    // Replace line feeds by line breaks
    int pos;
    QString strBr(isXML()?QString("<br />"):QString("<br>"));
    while ((pos=strEscaped.find(QChar(10)))>-1)
    {
        strEscaped.replace(pos,1,strBr);
    }

    if (!format.text.missing)
    {
        // Opening elements
        openSpan(formatOrigin,format);
    }

    // TODO: first and last characters of partialText should not be a space (white space problems!)
    // TODO: replace multiples spaces by non-breaking spaces!

    if (strText==" ")
    {//Just a space as text. Therefore we must use a non-breaking space.
        *m_streamOut << "&nbsp;";
        // TODO/FIXME: only needed for <p>&nbsp;</p>, but not for </span> <span>
    }
    else
    {
        *m_streamOut << strEscaped;
    }

    if (!format.text.missing)
    {
        // Closing elements
        closeSpan(formatOrigin,format);
    }
}

void HtmlWorker::ProcessParagraphData (const QString& strTag, const QString &paraText,
    const LayoutData& layout, const ValueListFormatData &paraFormatDataList)
{
    if (paraText.isEmpty() && paraFormatDataList.first().id != 6)
    {
        openParagraph(strTag,layout);
        *m_streamOut << "&nbsp;" ; // A paragraph can never be empty in HTML
        closeParagraph(strTag,layout);
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
                //Retrieve text
                partialText=paraText.mid ( (*paraFormatDataIt).pos, (*paraFormatDataIt).len );
                // For normal text, we need an opened paragraph
                if (paragraphNotOpened)
                {
                    openParagraph(strTag,layout,partialText.ref(0).direction());
                    paragraphNotOpened=false;
                }
                formatTextParagraph(partialText,layout.formatData,*paraFormatDataIt);
            }
            else if (4==(*paraFormatDataIt).id)
            {
                // For variables, we need an opened paragraph
                if (paragraphNotOpened)
                {
                    openParagraph(strTag,layout);
                    paragraphNotOpened=false;
                }
                if (9==(*paraFormatDataIt).variable.m_type)
                {
                    // A link
                    *m_streamOut << "<a href=\""
                        << escapeHtmlText((*paraFormatDataIt).variable.getHrefName())
                        << "\">"
                        << escapeHtmlText((*paraFormatDataIt).variable.getLinkName())
                        << "</a>";
                }
                else
                {
                    // Generic variable
                    *m_streamOut << escapeHtmlText((*paraFormatDataIt).variable.m_text);
                }
            }
            else if (6==(*paraFormatDataIt).id)
            {
                // We have an image, a clipart or a table

                if (6==(*paraFormatDataIt).frameAnchor.type)
                {
                    // We have a table
                    // But first, we must sure that the paragraph is not opened.
                    if (!paragraphNotOpened)
                    {
                        // The paragraph was opened, so close it.
                        closeParagraph(strTag,layout);
                    }
                    makeTable((*paraFormatDataIt).frameAnchor);
                    // The paragraph will need to be opened again
                    paragraphNotOpened=true;

                }

                else if ( ( 2 == (*paraFormatDataIt).frameAnchor.type )
                    || ( 5 == (*paraFormatDataIt).frameAnchor.type ) )
                {
                    // <img> and <object> need to be in a paragraph
                    if (paragraphNotOpened)
                    {
                        openParagraph( strTag, layout,partialText.ref(0). direction() );
                        paragraphNotOpened=false;
                    }
                    makeImage((*paraFormatDataIt).frameAnchor);
                }
                else
                {
                    kWarning(30503) << "Unknown anchor type: "
                        << (*paraFormatDataIt).frameAnchor.type << endl;
                }
            }
        }
        if (!paragraphNotOpened)
        {
            // The paragraph was opened, so close it.
            closeParagraph(strTag,layout);
        }
    }
}

bool HtmlWorker::doFullParagraph(const QString& paraText,
    const LayoutData& layout, const ValueListFormatData& paraFormatDataList)
{
    kDebug(30503) << "Entering HtmlWorker::doFullParagraph" << endl << paraText << endl;
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
                *m_streamOut << getStartOfListOpeningTag(layout.counter.style,newList.m_orderedList);
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
                    *m_streamOut << "</ol>\n";
                }
                else
                {
                    *m_streamOut << "</ul>\n";
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
                *m_streamOut << "</ol>\n";
            }
            else
            {
                *m_streamOut << "</ul>\n";
            }
            ListInfo newList;
            *m_streamOut << getStartOfListOpeningTag(layout.counter.style,newList.m_orderedList);
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
                    *m_streamOut << "</ol>\n";
                }
                else
                {
                    *m_streamOut << "</ul>\n";
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

    kDebug(30503) << "Quiting HtmlWorker::doFullParagraph" << endl;
    return true;
}

bool HtmlWorker::doOpenFile(const QString& filenameOut, const QString& /*to*/)
{
    m_ioDevice=new QFile(filenameOut);

    if (!m_ioDevice)
    {
        kError(30503) << "No output file! Aborting!" << endl;
        return false;
    }

    if ( !m_ioDevice->open (QIODevice::WriteOnly) )
    {
        kError(30503) << "Unable to open output file!" << endl;
        return false;
    }

    m_streamOut=new QTextStream(m_ioDevice);

    if (!getCodec())
    {
        kError(30503) << "Could not create QTextCodec! Aborting" << endl;
        return false;
    }

    kDebug(30503) << "Charset used: " << getCodec()->name() << endl;

    m_streamOut->setCodec( getCodec() );

    m_fileName=filenameOut;
    QFileInfo base(m_fileName);
    m_strFileDir=base.dirPath();
    m_strTitle=base.fileName();
    m_strSubDirectoryName=base.fileName();
    m_strSubDirectoryName+=".dir";

    return true;
}

bool HtmlWorker::doCloseFile(void)
{
    kDebug(30503) << __FILE__ << ":" << __LINE__ << endl;
    delete m_streamOut;
    m_streamOut=NULL;
    if (m_ioDevice)
        m_ioDevice->close();
    return true;
}

void HtmlWorker::writeDocType(void)
{
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
    writeDocType();

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
    kDebug(30503) << __FILE__ << ":" << __LINE__ << endl;
    *m_streamOut << "</html>\n";
    return true;
}

bool HtmlWorker::doFullDocumentInfo(const KWEFDocumentInfo& docInfo)
{
    QString strText=docInfo.title;
    if (!strText.isEmpty())
    {
        m_strTitle=strText; // Set title only if it is not empty!
        kDebug(30503) << "Found new title " << m_strTitle << endl;
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
              << strVersion.mid( 10 ).remove( '$' )
              << "\""<< (isXML()?" /":"") // X(HT)ML closes empty elements, HTML not!
              << ">\n";

    if (m_strTitle.isEmpty())
    {
        // Somehow we have still an empty title (this should not happen!)
        kWarning(30503) << "Title still empty! (HtmlWorker::doOpenHead)" << endl;
        m_strTitle=i18n("Untitled Document");
    }
    *m_streamOut << "<title>"<< escapeHtmlText(m_strTitle) <<"</title>\n";  // <TITLE> is mandatory!

    if( !customCSSURL().isEmpty() )
    {
      *m_streamOut << "<link ref=\"stylesheet\" type=\"text/css\" href=\"" << customCSSURL() << "\" title=\"Style\" >\n" << endl;
    }

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
    return true;
}

bool HtmlWorker::doCloseTextFrameSet(void)
{
    if (!m_listStack.isEmpty())
    {
        for (uint i=m_listStack.size(); i>0; i--)
        {
            ListInfo oldList=m_listStack.pop();
            if (oldList.m_orderedList)
            {
                *m_streamOut << "</ol>\n";
            }
            else
            {
                *m_streamOut << "</ul>\n";
            }
        }
    }
    return true;
}
