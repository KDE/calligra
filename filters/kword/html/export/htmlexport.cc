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

/*
   This file is based on the old file:
    /home/kde/koffice/filters/kword/ascii/asciiexport.cc

   The old file was copyrighted by
    Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
    Copyright (c) 2000 ID-PRO Deutschland GmbH. All rights reserved.
                       Contact: Wolf-Michael Bolle <Bolle@ID-PRO.de>

   The old file was licensed under the terms of the GNU Library General Public
   License version 2.
*/

/*
    Important changes

    04 Feb 2001  Nicolas GOUTTE <nicog@snafu.de>
        Re-write of HTML export
        (Replaces the old HTML export code completely)

*/

#include <qtextcodec.h>
#include <qtextstream.h>
#include <qregexp.h>
#include <qdom.h>

#include <kdebug.h>

#include <htmlexport.h>
#include <htmlexport.moc>

#include <KWEFStructures.h>
#include <TagProcessing.h>
#include <KWEFBaseClass.h>
#include <ProcessDocument.h>

#include "ExportDialog.h"
#include "ExportFilter.h"

static void ProcessParagraphTag ( QDomNode myNode, void *, QString   &outputText, KWEFBaseClass* exportFilter)
{
    AllowNoAttributes (myNode);

    QString paraText;
    ValueListFormatData paraFormatDataList;
    LayoutData paraLayout;
    QValueList<TagProcessing> tagProcessingList;

    tagProcessingList.append ( TagProcessing ( "TEXT",    ProcessTextTag,       (void *) &paraText           ) );
    tagProcessingList.append ( TagProcessing ( "FORMATS", ProcessFormatsTag,    (void *) &paraFormatDataList ) );
    tagProcessingList.append ( TagProcessing ( "LAYOUT",  ProcessLayoutTag,     (void *) &paraLayout         ) );
    tagProcessingList.append ( TagProcessing ( "HARDBRK", NULL,                 NULL) ); // Not documented!
    ProcessSubtags (myNode, tagProcessingList, outputText, exportFilter);

    QString strParaText;

    exportFilter->ProcessParagraphData ( paraText, paraFormatDataList, strParaText);

    if (strParaText.isEmpty())
    {
        //An empty paragraph is not allowed in HTML, so add a non-breaking space!
        strParaText="&nbsp;";
    }

    // As KWord has only one depth of lists, we can process lists very simply.
    if ( paraLayout.counter.numbering == CounterData::NUM_LIST )
    {
        if (exportFilter->inList)
        {
            // We are in a list but does it have the right type?
            if ( paraLayout.counter.style!=exportFilter->typeList)
            {
                // No, then close the previous list
                if (exportFilter->orderedList)
                {
                    outputText+="</ol>\n";
                }
                else
                {
                    outputText+="</ul>\n";
                }
                exportFilter->inList=false; // We are not in a list anymore
            }
        }

        // Are we still in a list?
        if (!exportFilter->inList)
        {
            // We are not yet part of a list
            exportFilter->inList=true;
            outputText+=exportFilter->getStartOfListOpeningTag(paraLayout.counter.style,exportFilter->orderedList);
            exportFilter->typeList=paraLayout.counter.style;
        }
        // TODO: with Cascaded Style Sheet, we could add the exact counter type we want
        outputText += exportFilter->getParagraphElement("li",strParaText,paraLayout);
    }
    else
    {
        if (exportFilter->inList)
        {
            // The previous paragraphs were in a list, so we have to close it
            if (exportFilter->orderedList)
            {
                outputText+="</ol>\n";
            }
            else
            {
                outputText+="</ul>\n";
            }
            exportFilter->inList=false;
        }
        if ( paraLayout.counter.numbering == CounterData::NUM_CHAPTER )
        {
            QString strDepth=QString("h%1").arg(paraLayout.counter.depth + 1);
            outputText += exportFilter->getParagraphElement(strDepth,strParaText,paraLayout);
        }
        else
        {
            outputText += exportFilter->getParagraphElement("p",strParaText,paraLayout);
        }
    }
}


static void ProcessFramesetTag ( QDomNode myNode, void *, QString   &outputText, KWEFBaseClass* exportFilter)
{
    int frameType=-1;
    int frameInfo=-1;

    QValueList<AttrProcessing> attrProcessingList;

    attrProcessingList.append ( AttrProcessing ( "frameType", "int", (void*) &frameType ) );
    attrProcessingList.append ( AttrProcessing ( "frameInfo", "int", (void*) &frameInfo) );
    attrProcessingList.append ( AttrProcessing ( "removable", "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "visible",   "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "name",      "", NULL ) );
    ProcessAttributes (myNode, attrProcessingList);

    if ((1==frameType) && (0==frameInfo))
    {   //Main text

        QValueList<TagProcessing> tagProcessingList;
        tagProcessingList.append ( TagProcessing ( "FRAME",     NULL,                NULL ) );
        tagProcessingList.append ( TagProcessing ( "PARAGRAPH", ProcessParagraphTag, NULL ) );
        ProcessSubtags (myNode, tagProcessingList, outputText, exportFilter);

        // Are we still in a list?
        if (exportFilter->inList)
        {
            // We are in a list, so close it!
            if (exportFilter->orderedList)
            {
                outputText+="</ol>\n";
            }
            else
            {
                outputText+="</ul>\n";
            }
            exportFilter->inList=false;
        }
    }
    //TODO: Treat the other types of frames (frameType)
}


static void ProcessFramesetsTag (QDomNode myNode, void *, QString   &outputText, KWEFBaseClass* exportFilter)
{
    AllowNoAttributes (myNode);

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "FRAMESET", ProcessFramesetTag, NULL ) );
    ProcessSubtags (myNode, tagProcessingList, outputText, exportFilter);
}

static void ProcessDocTag (QDomNode myNode, void *,  QString &outputText, KWEFBaseClass* exportFilter)
{
    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ( "editor",        "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "mime",          "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "syntaxVersion", "", NULL ) );
    ProcessAttributes (myNode, attrProcessingList);

    // Do not process <STYLES>, that is the job of the function ProcessDocTagStylesOnly
    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "PAPER",       NULL,                NULL ) );
    tagProcessingList.append ( TagProcessing ( "ATTRIBUTES",  NULL,                NULL ) );
    tagProcessingList.append ( TagProcessing ( "FOOTNOTEMGR", NULL,                NULL ) );
    // Do not process <STYLES>, that is the job of the function processDocTagStylesOnly
    tagProcessingList.append ( TagProcessing ( "STYLES",      NULL,                NULL ) );
    tagProcessingList.append ( TagProcessing ( "PIXMAPS",     NULL,                NULL ) );
    tagProcessingList.append ( TagProcessing ( "SERIALL",     NULL,                NULL ) );
    tagProcessingList.append ( TagProcessing ( "FRAMESETS",   ProcessFramesetsTag, NULL ) );
    ProcessSubtags (myNode, tagProcessingList, outputText, exportFilter);
}

// ClassExportFilterBase

void ClassExportFilterHtml::helpStyleProcessing(QDomNode myNode,LayoutData* layout)
{
    QString strDummy;

    ProcessLayoutTag(myNode, layout, strDummy, this);

}

bool ClassExportFilterHtml::filter(const QString  &filenameIn, const QString  &filenameOut)
// NOTE: this member function is still define here! (TODO: decide if it should be moved too!)
{

    KoStore koStoreIn (filenameIn, KoStore::Read);

    QByteArray byteArrayIn;

    QString strTitle;
    if ( koStoreIn.open ( "documentinfo.xml" ) )
    {
        byteArrayIn = koStoreIn.read ( koStoreIn.size () );
        koStoreIn.close ();

        QDomDocument qDomDocumentInfo;
        qDomDocumentInfo.setContent (byteArrayIn);

        // Search <title> element (be carefull: there are two of them!)
        QDomNodeList docNodeList = qDomDocumentInfo.elementsByTagName("title");

        // Now find the <title> element that is child of a <about> element
        for (uint i=0; i<docNodeList.count(); i++)
        {
            QDomNode node=docNodeList.item(i);
            kdDebug(30503) << " Parent name: " << node.parentNode().nodeName() << endl;
            if (node.parentNode().nodeName()=="about")
            {
                // We have the one we want!
                //  Therefore retrieve text of element (may be empty!)
                strTitle=node.toElement().text();
                kdDebug(30503) << "Found new title " << strTitle << endl;
            }
        }
    }
    else
    {
        // Note: we do not worry too much if we cannot open the document info!
        kdWarning(30503) << "Unable to open documentinfo.xml sub-file!" << endl;
    }

    if ( !koStoreIn.open ( "root" ) )
    {
        koStoreIn.close ();

        kdError(30503) << "Unable to open input file!" << endl;
        return false;
    }

    byteArrayIn = koStoreIn.read ( koStoreIn.size () );
    koStoreIn.close ();

    // let parse the buffer just read from the file
    qDomDocumentIn.setContent(byteArrayIn);

    QDomElement docNodeIn = qDomDocumentIn.documentElement ();

    //Now all is ready to write to a file
    QFile fileOut (filenameOut);

    if ( !fileOut.open (IO_WriteOnly) )
    {
        fileOut.close ();

        kdError(30503) << "Unable to open output file!" << endl;
        return false;
    }

    // Find out IANA/mime charset name
    QString strCharset;
    if ( isUTF8() )
    {
        strCharset="UTF-8";
    }
    else
    {
        strCharset=QTextCodec::codecForLocale()->mimeName();
    }

    kdDebug(30501) << "Charset used: " << strCharset << endl;

    if (strCharset.isEmpty())
    {
        // We have a (X)HTML file to write and we have a no charset name
        // => change to UTF-8
        setUTF8(true);
        strCharset="UTF-8";
        kdWarning(30503) << "Encoding of (X)HTML file has been forced to UTF-8!"
            << endl;
    }

    //Use a QTextStream, so we do not have a big buffer using plenty of memory.
    QTextStream streamOut(&fileOut);

    // TODO: possibility of choosing other encodings
    if ( isUTF8() )
    {
        streamOut.setEncoding( QTextStream::UnicodeUTF8 );
    }
    else
    {
        streamOut.setEncoding( QTextStream::Locale );
    }

    // Make the file header

    if (isXML())
    {   //Write out the XML declaration
        streamOut << "<?xml version=\"1.0\" encoding=\"" << strCharset << "\"?>" << endl;
    }

    // write <!DOCTYPE
    streamOut << getDocType() << endl;

    // No "lang" or "xml:lang" attribute for <html>, as we do not know in which language the document is!
    streamOut << "<html"<< getHtmlOpeningTagExtraAttributes() << ">" << endl;

    streamOut << "<head>" << endl;

    // Declare what charset we are using
    streamOut << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=";
    streamOut << strCharset << '"';
    streamOut << (isXML()?" /":"") << ">\n" ;

    // Say who we are (with the CVS revision number) in case we have a bug in our filter output!
    QString strVersion("$Revision$");
    // Eliminate the dollar signs
    //  (We don't want that the version number changes if the HTML file is itself put in a CVS storage.)
    streamOut << "<meta name=\"Generator\" content=\"KWord HTML Export Filter Version"
              << strVersion.mid(10).replace(QRegExp("\\$"),"") // Note: double escape character (one for C++, one for QRegExp!)
              << "\""<< (isXML()?" /":"") // X(HT)ML closes empty elements, HTML not!
              << ">" << endl;

    if (strTitle.isEmpty())
    {
        // We have still no title for the document,
        //  therefore make one from the file name
        strTitle=filenameOut;
        const int result=strTitle.findRev("/");
        if (result>=0)
        {
            strTitle=strTitle.mid(result+1);
        }
    }

    streamOut << "<title>"<< escapeText(strTitle) <<"</title>" << endl;  // <TITLE> is mandatory in HTML 4.01 !

    //TODO: transform documentinfo.xml into many <META> elements (at least the author!)

    // Make now the <style> element if needed
    streamOut << processDocTagStylesOnly(docNodeIn); // Includes an end of line at the end if it is not empty.

    streamOut << "</head>" << endl;

    kdDebug(30503) << "<head> writen!" << endl;

    streamOut << "<body" << getBodyOpeningTagExtraAttributes() << ">" << endl;

    streamOut << "<div>\n"; // For compatibility with AbiWord's XHTML import filter

    // Now that we have the header, we can do the real work!
    QString stringBufOut;
    ProcessDocTag (docNodeIn, NULL, stringBufOut, this);
    streamOut << stringBufOut;

    streamOut << "</div>\n"; // For compatibility with AbiWord's XHTML import filter

    // Add the tail of the file
    streamOut << "</body>\n</html>\n";

    fileOut.close();
    return true;

}

//
// HTMLExport
//

HTMLExport::HTMLExport(KoFilter *parent, const char *name) :
                     KoFilter(parent, name) {
}

bool HTMLExport::filter(const QString  &filenameIn,
                        const QString  &filenameOut,
                        const QString  &from,
                        const QString  &to,
                        const QString  &)
{
    if ((from != "application/x-kword") || (to != "text/html"))
    {
        return false;
    }

#if 1
    // Some "security" to see if I have forgotten to run "make install"
    // (Can be deleted when the filter will be stable.)
    kdDebug(30503) << "htmlexport.cc " << __DATE__ " " __TIME__ << " " << "$Revision$" << endl;
#endif

    HtmlExportDialog* dialog = new HtmlExportDialog();

    if (!dialog)
    {
        kdError(30503) << "Dialog has not been created Aborting!" << endl;
        return false;
    }

    if (!dialog->exec())
    {
        kdError(30503) << "Dialog was aborted! Aborting filter!" << endl;
        return false;
    }

    ClassExportFilterHtml* exportFilter=new ClassExportFilterHtml;

    if (!exportFilter)
    {
        kdError(30503) << "No (X)HTML filter created! Aborting! (Memory problem?)" << endl;
        return false;
    }

    exportFilter->setXML(dialog->isXHtml());
    exportFilter->setUTF8(dialog->isEncodingUTF8());

    delete dialog;

    // Do the work!
    const bool result = exportFilter->filter(filenameIn,filenameOut);

    delete exportFilter;

    return result;
}
