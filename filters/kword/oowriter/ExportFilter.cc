// 

/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003, 2004 Nicolas GOUTTE <goutte@kde.org>

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

#include <qmap.h>
#include <qiodevice.h>
#include <qtextstream.h>
#include <qdom.h>

#include <kdebug.h>
#include <klocale.h>
#include <kzip.h>

#include <koGlobal.h>
#include <koPictureKey.h>

#include <KWEFStructures.h>
#include <KWEFUtil.h>
#include <KWEFBaseWorker.h>
#include <KWEFKWordLeader.h>

#include "ExportFilter.h"

OOWriterWorker::OOWriterWorker(void) : m_streamOut(NULL),
    m_paperBorderTop(0.0),m_paperBorderLeft(0.0),
    m_paperBorderBottom(0.0),m_paperBorderRight(0.0), m_zip(NULL), m_pictureNumber(0),
    m_automaticParagraphStyleNumber(0), m_automaticTextStyleNumber(0),
    m_footnoteNumber(0)
{
}

QString OOWriterWorker::escapeOOText(const QString& strText) const
{
    // Escape quotes (needed in attributes)
    // Escape apostrophs (allowed by XML)
    return KWEFUtil::EscapeSgmlText(NULL,strText,true,true);
}

QString OOWriterWorker::escapeOOSpan(const QString& strText) const
// We need not only to escape the classical XML stuff but also to take care of spaces and tabs.
{
    QString strReturn;
    QChar ch;
    int spaceNumber = 0; // How many spaces should be written
    
    for (uint i=0; i<strText.length(); i++)
    {
        ch=strText[i];

        if (ch!=' ')
        {
            // The next character is not a space (anymore)
            if ( spaceNumber > 0 )
            {
                strReturn += ' ';
                --spaceNumber;
                if ( spaceNumber > 0 )
                {
                    strReturn += "<text:s text:c=\"";
                    strReturn += QString::number( spaceNumber );
                    strReturn += "\"/>";
                }
                spaceNumber = 0;
            }
        }

        // ### TODO: would be switch/case or if/elseif the best?
        switch (ch.unicode())
        {
        case 9: // Tab
            {
                strReturn+="<text:tab-stop/>";
                break;
            }
        case 10: // Line-feed
            {
                strReturn+="<text:line-break/>";
                break;
            }
        case 32: // Space
            {
                if ( spaceNumber > 0 )
                {
                    ++spaceNumber;
                }
                else
                {
                    spaceNumber = 1;
                }
                break;
            }
        case 38: // &
            {
                strReturn+="&amp;";
                break;
            }
        case 60: // <
            {
                strReturn+="&lt;";
                break;
            }
        case 62: // >
            {
                strReturn+="&gt;";
                break;
            }
        case 34: // "
            {
                strReturn+="&quot;";
                break;
            }
        case 39: // '
            {
                strReturn+="&apos;";
                break;
            }
        case 1: // (Non-XML-compatible) replacement character from KWord 0.8
            {
                strReturn += '#'; //use KWord 1.[123] replacement character instead
                break;
            }
        // Following characters are not allowed in XML (but some files from KWord 0.8 have some of them.)
        case  0: case  2 ... 8: case 11: case 12: case 14 ... 31:
            {
                kdWarning(30518) << "Not allowed XML character: " << ch.unicode() << endl;
                strReturn += '?';
                break;
            }
        case 13: // ### TODO: what to do with it?
        default:
            {
                strReturn+=ch;
                break;
            }
        }
    }

    if ( spaceNumber > 0 )
    {
        // The last characters were spaces
        strReturn += ' ';
        --spaceNumber;
        if ( spaceNumber > 0 )
        {
            strReturn += "<text:s text:c=\"";
            strReturn += QString::number( spaceNumber );
            strReturn += "\"/>";
        }
        spaceNumber = 0;
    }

    return strReturn;
}

bool OOWriterWorker::doOpenFile(const QString& filenameOut, const QString& )
{
    kdDebug(30518) << "Opening file: " << filenameOut
        << " (in OOWriterWorker::doOpenFile)" << endl;

    m_zip=new KZip(filenameOut); // How to check failure?

    if (!m_zip->open(IO_WriteOnly))
    {
        kdError(30518) << "Could not open ZIP file for writing! Aborting!" << endl;
        delete m_zip;
        m_zip=NULL;
        return false;
    }

    m_zip->setCompression( KZip::NoCompression );
    m_zip->setExtraField( KZip::NoExtraField );

    const QCString appId( "application/vnd.sun.xml.writer" );

    m_zip->writeFile( "mimetype", QString::null, QString::null, appId.length(), appId.data() );

    m_zip->setCompression( KZip::DeflateCompression );

    m_streamOut=new QTextStream(m_contentBody, IO_WriteOnly);

    m_streamOut->setEncoding( QTextStream::UnicodeUTF8 );

    return true;
}

bool OOWriterWorker::zipPrepareWriting(const QString& name)
{
    if (!m_zip)
        return false;
    m_size=0;
    return m_zip->prepareWriting(name, QString::null, QString::null, 0);
}

bool OOWriterWorker::zipDoneWriting(void)
{
    if (!m_zip)
        return false;
    return m_zip->doneWriting(m_size);
}

bool OOWriterWorker::zipWriteData(const char* str)
{
    if (!m_zip)
        return false;
    const uint size=strlen(str);
    m_size+=size;
    return m_zip->writeData(str,size);
}

bool OOWriterWorker::zipWriteData(const QByteArray& array)
{
    if (!m_zip)
        return false;
    const uint size=array.size();
    m_size+=size;
    return m_zip->writeData(array.data(),size);
}

bool OOWriterWorker::zipWriteData(const QCString& cstr)
{
    if (!m_zip)
        return false;
    const uint size=cstr.length();
    m_size+=size;
    return m_zip->writeData(cstr.data(),size);
}

bool OOWriterWorker::zipWriteData(const QString& str)
{
    return zipWriteData(str.utf8());
}

void OOWriterWorker::writeStartOfFile(const QString& type)
{
    const bool noType=type.isEmpty();
    zipWriteData("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");

    zipWriteData("<!DOCTYPE office:document");
    if (!noType)
    {
        // No type might happen for raw XML documents (which this filter does not support yet.)
        zipWriteData("-");
        zipWriteData(type);
    }
    zipWriteData(" PUBLIC \"-//OpenOffice.org//DTD OfficeDocument 1.0//EN\"");
    zipWriteData(" \"office.dtd\"");
    zipWriteData(">\n");

    zipWriteData("<office:document");
    if (!noType)
    {
        zipWriteData("-");
        zipWriteData(type);
    }

    // The name spaces used by OOWriter (those not used by this filter are commented out)

    // General namespaces
    zipWriteData(" xmlns:office=\"http://openoffice.org/2000/office\"");
    zipWriteData(" xmlns:xlink=\"http://www.w3.org/1999/xlink\"");

    // Namespaces for context.xml and style.xml
    if ( type == "content" || type == "styles" || type.isEmpty() )
    {
        zipWriteData(" xmlns:style=\"http://openoffice.org/2000/style\"");
        zipWriteData(" xmlns:text=\"http://openoffice.org/2000/text\"");
        zipWriteData(" xmlns:table=\"http://openoffice.org/2000/table\"");
        zipWriteData(" xmlns:draw=\"http://openoffice.org/2000/drawing\"");
        zipWriteData(" xmlns:fo=\"http://www.w3.org/1999/XSL/Format\"");

        //zipWriteData(" xmlns:number=\"http://openoffice.org/2000/datastyle\"");
        zipWriteData(" xmlns:svg=\"http://www.w3.org/2000/svg\"");
        //zipWriteData(" xmlns:chart=\"http://openoffice.org/2000/chart\"");
        //zipWriteData(" xmlns:dr3d=\"http://openoffice.org/2000/dr3d\"");
        //zipWriteData(" xmlns:math=\"http://www.w3.org/1998/Math/MathML"");
        //zipWriteData(" xmlns:form=\"http://openoffice.org/2000/form\"");
        //zipWriteData(" xmlns:script=\"http://openoffice.org/2000/script\"");
    }

    // Namespaces For meta.xml
    if ( type == "meta" || type.isEmpty() )
    {
        zipWriteData(" xmlns:dc=\"http://purl.org/dc/elements/1.1/\"");
        zipWriteData(" xmlns:meta=\"http://openoffice.org/2000/meta\"");
    }

    zipWriteData(" office:class=\"text\"");

 
#ifdef STRICT_OOWRITER_VERSION_1
    zipWriteData(" office:version=\"1.0\"");
#else
    // We are using an (rejected draft OASIS) extension compared to version 1.0, so we cannot write the version string.
    // (We do not even write it for context.xml and meta.xml, as OOWriter 1.0.1 does not like it in this case.)
#endif

    zipWriteData(">\n");
}

void OOWriterWorker::writeFontDeclaration(void)
{
    zipWriteData( " <office:font-decls>\n");
    for (QMap<QString,QString>::ConstIterator it=m_fontNames.begin(); it!=m_fontNames.end(); it++)
    {
        const bool space=(it.key().find(' ')>=0); // Does the font has at least a space in its name
        const QString fontName(escapeOOText(it.key()));
        zipWriteData("  <style:font-decl style:name=\"");
        zipWriteData(fontName);
        zipWriteData("\" fo:font-family=\"");
        if (space)
        { // It has a space, so (simple) quote it
            zipWriteData("&apos;");
            zipWriteData(fontName);
            zipWriteData("&apos;");
        }
        else
        { // The font has no space in its name, so it can be written normally.
            zipWriteData(fontName);
        }
        zipWriteData("\" ");
        zipWriteData(it.data()); // already in XML, so do not escape
        zipWriteData(" />\n");
    }
    zipWriteData(" </office:font-decls>\n");
}

void OOWriterWorker::writeStylesXml(void)
{
    if (!m_zip)
        return;

    zipPrepareWriting("styles.xml");

    writeStartOfFile("styles");

    writeFontDeclaration();

    zipWriteData(m_styles);

    zipWriteData(" <office:automatic-styles>\n");
    zipWriteData("  <style:page-master style:name=\"pm1\">\n"); // ### TODO: verify if style name is unique

    zipWriteData("   <style:properties ");

    zipWriteData(" fo:page-width=\"");
    zipWriteData(QString::number(m_paperWidth));
    zipWriteData("pt\" fo:page-height=\"");
    zipWriteData(QString::number(m_paperHeight));
    zipWriteData("pt\" ");

    zipWriteData("style:print-orientation=\"");
    if (1==m_paperOrientation)
    {
        zipWriteData("landscape");
    }
    else
    {
        zipWriteData("portrait");
    }

    zipWriteData("\" fo:margin-top=\"");
    zipWriteData(QString::number(m_paperBorderTop));
    zipWriteData("pt\" fo:margin-bottom=\"");
    zipWriteData(QString::number(m_paperBorderBottom));
    zipWriteData("pt\" fo:margin-left=\"");
    zipWriteData(QString::number(m_paperBorderLeft));
    zipWriteData("pt\" fo:margin-right=\"");
    zipWriteData(QString::number(m_paperBorderRight));
    zipWriteData("pt\" style:first-page-number=\"");
    zipWriteData(QString::number(m_varSet.startingPageNumber));
    zipWriteData("\"/>\n");

    zipWriteData("  </style:page-master>\n");
    zipWriteData(" </office:automatic-styles>\n");

    zipWriteData(" <office:master-styles>\n");
    zipWriteData("  <style:master-page style:name=\"Standard\" style:page-master-name=\"pm1\" />\n");
    zipWriteData(" </office:master-styles>\n");

    zipWriteData( "</office:document-styles>\n" );

    zipDoneWriting();
}

void OOWriterWorker::writeContentXml(void)
{
    if (!m_zip)
        return;

    zipPrepareWriting("content.xml");

    writeStartOfFile("content");

    writeFontDeclaration();

    zipWriteData(" <office:automatic-styles>\n");
    zipWriteData(m_contentAutomaticStyles);
    m_contentAutomaticStyles = QString::null; // Release memory
    
    zipWriteData(" </office:automatic-styles>\n");

    zipWriteData(m_contentBody);
    m_contentBody.resize( 0 ); // Release memory

    zipWriteData( "</office:document-content>\n" );

    zipDoneWriting();
}

void OOWriterWorker::writeMetaXml(void)
{
    if (!m_zip)
        return;

    zipPrepareWriting("meta.xml");

    writeStartOfFile("meta");

    zipWriteData(" <office:meta>\n");

    // Tell who we are in case that we have a bug in our filter output!
    zipWriteData("  <meta:generator>KWord's OOWriter Export Filter");
    zipWriteData(QString("$Revision$").mid(10).remove('$')); // has a leading and a trailing space.

    zipWriteData("</meta:generator>\n");

    if (!m_docInfo.title.isEmpty())
    {
        zipWriteData("  <dc:title>");
        zipWriteData(escapeOOText(m_docInfo.title));
        zipWriteData("</dc:title>\n");
    }
    if (!m_docInfo.abstract.isEmpty())
    {
        zipWriteData("  <dc:description>");
        zipWriteData(escapeOOText(m_docInfo.abstract));
        zipWriteData("</dc:description>\n");
    }

    if (m_varSet.creationTime.isValid())
    {
        zipWriteData("  <meta:creation-date>");
        zipWriteData(escapeOOText(m_varSet.creationTime.toString(Qt::ISODate)));
        zipWriteData("</meta:creation-date>\n");
    }

    if (m_varSet.modificationTime.isValid())
    {
        zipWriteData("  <dc:date>");
        zipWriteData(escapeOOText(m_varSet.modificationTime.toString(Qt::ISODate)));
        zipWriteData("</dc:date>\n");
    }

    if (m_varSet.printTime.isValid())
    {
        zipWriteData("  <meta:print-date>");
        zipWriteData(escapeOOText(m_varSet.printTime.toString(Qt::ISODate)));
        zipWriteData("</meta:print-date>\n");
    }

    zipWriteData(" </office:meta>\n");
    zipWriteData("</office:document-meta>\n");

    zipDoneWriting();
}

bool OOWriterWorker::doCloseFile(void)
{
    kdDebug(30518)<< "OOWriterWorker::doCloseFile" << endl;
    if (m_zip)
    {
        writeContentXml();
        writeMetaXml();
        writeStylesXml();
        m_zip->close();
    }

    delete m_zip;
    m_zip=NULL;
    return true;
}

bool OOWriterWorker::doOpenDocument(void)
{
    kdDebug(30518)<< "OOWriterWorker::doOpenDocument" << endl;

    *m_streamOut << " <office:body>\n";

    return true;
}

bool OOWriterWorker::doCloseDocument(void)
{
    *m_streamOut << " </office:body>\n";
    return true;
}

QString OOWriterWorker::textFormatToStyle(const TextFormatting& formatOrigin,
    const TextFormatting& formatData, const bool force, QString& key)
{
    // TODO: rename variable formatData
    QString strElement; // TODO: rename this variable

    // Font name
    QString fontName = formatData.fontName;
    declareFont(fontName);
    if ( !fontName.isEmpty()
        && (force || (formatOrigin.fontName!=formatData.fontName)))
    {
        strElement+="style:font-name=\"";
        strElement+= escapeOOText(fontName);
        strElement+="\" ";
        key += fontName;
    }

    key += ",";

    if (force || (formatOrigin.italic!=formatData.italic))
    {
        // Font style
        strElement+="fo:font-style=\"";
        if ( formatData.italic )
        {
            strElement+="italic";
            key+='I';
        }
        else
        {
            strElement+="normal";
            key+='N';
        }
        strElement+="\" ";
    }

    key += ",";

    if (force || ((formatOrigin.weight>=75)!=(formatData.weight>=75)))
    {
        strElement+="fo:font-weight=\"";
        if ( formatData.weight >= 75 )
        {
            strElement+="bold";
            key+='B';
        }
        else
        {
            strElement+="normal";
            key+='N';
        }
        strElement+="\" ";
    }

    key += ",";

    if (force || (formatOrigin.fontSize!=formatData.fontSize))
    {
        const int size=formatData.fontSize;
        if (size>0)
        {
            strElement+="fo:font-size=\"";
            strElement+=QString::number(size,10);
            strElement+="pt\" ";
            key+=QString::number(size,10);
        }
    }

    key += ",";

    if (force || (formatOrigin.fgColor!=formatData.fgColor))
    {
        if ( formatData.fgColor.isValid() )
        {
            strElement+="fo:color=\"";
            strElement+=formatData.fgColor.name();
            strElement+="\" ";
            key+=formatData.fgColor.name();
        }
    }

    key += ",";

    if (force || (formatOrigin.bgColor!=formatData.bgColor))
    {
        if ( formatData.bgColor.isValid() )
        {
            strElement+="style:text-background-color=\"";  // ### what is fo:background-color ?
            strElement+=formatData.bgColor.name();
            strElement+="\" ";
            key+=formatData.bgColor.name();
        }
    }

    key += ';'; // Another separator

    if ( force || ( formatOrigin.underline != formatData.underline )
        || ( formatOrigin.underlineColor != formatData.underlineColor )
        || ( formatOrigin.underlineValue != formatData.underlineValue )
        || ( formatOrigin.underlineStyle != formatData.underlineStyle ) )
    {
        strElement+="style:text-underline=\"";
        if ( formatData.underline )
        {
            QString underlineValue ( formatData.underlineValue );
            QString underlineStyle ( formatData.underlineStyle );

            if ( underlineStyle.isEmpty() )
                underlineStyle = "solid";
            if  ( underlineValue == "1" )
                underlineValue = "single";

            if ( underlineValue == "single" )
            {
                if ( underlineStyle == "dash" )
                {
                    strElement += "dash";
                    key += "DA";
                }
                else if ( underlineStyle == "dot" )
                {
                    strElement += "dotted";
                    key += "DO";
                }
                else if ( underlineStyle == "dashdot" )
                {
                    strElement += "dot-dash";
                    key += "DDA";
                }
                else if ( underlineStyle == "dashdotdot" )
                {
                    strElement += "dot-dot-dash";
                    key += "DDDA";
                }
                else
                {
                    strElement += "single";
                    key += "1";
                }
            }
            else if ( underlineValue == "double" )
            {
                strElement += "double";
                key += "2";
            }
            else if ( underlineValue == "single-bold" )
            {
                strElement += "bold";
                key += "BL";
            }
            else if ( underlineValue == "wave" )
            {
                strElement += "wave";
                key += "WV";
            }
            else
            {
                strElement += "single";
                key += "?";
            }
        }
        else
        {
            strElement+="none";
            key += 'N';
        }
        strElement += "\" ";

        if ( formatData.underline && formatData.underlineColor.isValid() )
        {
            const QString colorName( formatData.underlineColor.name() );
            strElement += "style:text-underline-color=\"";
            strElement += colorName;
            strElement += "\" ";
            key += colorName;
        }

    }

    key += ',';

    if ( force
        || (formatOrigin.strikeout != formatData.strikeout )
        || (formatOrigin.strikeoutType != formatData.strikeoutType ) )
    {
        // OOWriter can only do single, double, thick (and slash and X that KWord cannot do.)
        //  So no dash, dot and friends.

        strElement+="style:text-crossing-out=\"";
        if ( ( formatData.strikeoutType == "single" ) || ( formatData.strikeoutType == "1" ) )
        {
            strElement+="single-line";
            key += "1";
        }
        else if ( formatData.strikeoutType == "double" )
        {
            strElement+="double-line";
            key += "2";
        }
        else if ( formatData.strikeoutType == "single-bold" )
        {
            strElement+="thick";
            key += "T";
        }
        else
        {
            strElement+="none";
            key += 'N';
        }
        strElement+="\" ";
    }

    key += ',';

    // It seems that OOWriter 1.1 does have problems with word-by-word (OO Issue #11873, #25187)
    // It is supposed to be fixed in development versions of OO
    if (force || ( formatOrigin.underlineWord != formatData.underlineWord )
        || (formatOrigin.strikeoutWord != formatData.strikeoutWord ) )
    {
        // Strikeout and underline can only have one word-by-word behaviour in OO
        // (OO Issue #????? ; will not be changed.)
        strElement+="fo:score-spaces=\""; // Are space processed?
        if ( formatData.underlineWord || formatData.strikeoutWord )
        {
            strElement += "false";
            key += 'W';
        }
        else
        {
            strElement += "true";
            key += 'N';
        }
        strElement += "\" ";
    }

    return strElement.stripWhiteSpace(); // Remove especially trailing spaces
}

bool OOWriterWorker::makeTable(const FrameAnchor& anchor)
{
#if 0
    *m_streamOut << "</abiword:p>\n"; // Close previous paragraph ### TODO: do it correctly like for HTML
    *m_streamOut << "<table:table>\n";
    // ### TODO: table:column
    // ### TODO: automatic styles

    QValueList<TableCell>::ConstIterator itCell;

    for (itCell=anchor.table.cellList.begin();
        itCell!=anchor.table.cellList.end(); itCell++)
    {
        // ### TODO: rowspan, colspan

        // AbiWord seems to work by attaching to the cell borders
        *m_streamOut << "<table:table-cell Table:value-type=\"string\">\n";

        if (!doFullAllParagraphs(*(*itCell).paraList))
        {
            return false;
        }

        *m_streamOut << "</table:table-cell>\n";
    }

    *m_streamOut << "</table:table>\n";
    *m_streamOut << "<abiword:p>\n"; // Re-open the "previous" paragraph ### TODO: do it correctly like for HTML
#endif
    return true;
}

bool OOWriterWorker::makePicture(const FrameAnchor& anchor)
{
    kdDebug(30518) << "New picture: " << anchor.picture.koStoreName
        << " , " << anchor.picture.key.toString() << endl;

    QString koStoreName(anchor.picture.koStoreName);

    QByteArray image;

    QString strExtension(koStoreName.lower());
    const int result=koStoreName.findRev(".");
    if (result>=0)
    {
        strExtension=koStoreName.mid(result+1);
    }

    bool isImageLoaded=false;

    if (strExtension=="png")
    {
        isImageLoaded=loadSubFile(koStoreName,image);
    }
    else if ((strExtension=="jpg") || (strExtension=="jpeg"))
    {
        isImageLoaded=loadSubFile(koStoreName,image);
        strExtension="jpg"; // ### TODO: verify
    }
    else if ((strExtension=="tif") || (strExtension=="tiff"))
    {
        isImageLoaded=loadSubFile(koStoreName,image);
        strExtension="tif"; // ### TODO: verify
    }
    else if ((strExtension=="gif") || (strExtension=="wmf"))
        // ### TODO: Which other image formats does OOWriter support directly?
    {
        isImageLoaded=loadSubFile(koStoreName,image);
    }
    else
    {
        // All other picture types must be converted to PNG
        isImageLoaded=loadAndConvertToImage(koStoreName,strExtension,"PNG",image);
        strExtension="png";
    }

    if (!isImageLoaded)
    {
        kdWarning(30518) << "Unable to load picture: " << koStoreName << endl;
        return true;
    }

    kdDebug(30518) << "Picture loaded: " << koStoreName << endl;

    const double height=anchor.frame.bottom - anchor.frame.top;
    const double width =anchor.frame.right  - anchor.frame.left;

     // We need a 32 digit hex value of the picture number
     // Please note: it is an exact 32 digit value, truncated if the value is more than 512 bits wide. :-)
    QString number;
    number.fill('0',32);
    number += QString::number(++m_pictureNumber,16); // in hex

    QString ooName("Pictures/");
    ooName += number.right(32);
    ooName += '.';
    ooName += strExtension;

    kdDebug(30518) << "Picture " << koStoreName << " => " << ooName << endl;

    // TODO: we are only using the filename, not the rest of the key
    // TODO:  (bad if there are two images of the same name, but of a different key)
    *m_streamOut << "<draw:image draw:name=\"" << anchor.picture.key.filename() << "\"";
    *m_streamOut << " draw:style-name=\"Graphics\""; // ### TODO: should be an automatic "graphic" style name instead
    *m_streamOut << " text:anchor-type=\"paragraph\"";
    *m_streamOut << " svg:height=\"" << height << "pt\" svg:width=\"" << width << "pt\"";
    *m_streamOut << " draw:z-index=\"0\" xlink:href=\"#" << ooName << "\"";
    *m_streamOut << " xlink:type=\"simple\" xlink:show=\"embed\" xlink:actuate=\"onLoad\"";
    *m_streamOut << "/>"; // NO end of line!

    if (m_zip)
    {
#if 0
        // ### FIXME Why is the following line not working? (It makes unzip having problems with meta.xml)
        m_zip->writeFile(ooName,QString::null, QString::null, image.size(), image.data());
#else
        zipPrepareWriting(ooName);
        zipWriteData( image );
        zipDoneWriting();
#endif
    }

    return true;
}

void OOWriterWorker::processNormalText ( const QString &paraText,
    const TextFormatting& formatLayout,
    const FormatData& formatData)
{
    // Retrieve text and escape it (and necessary space, tabs and line-break tags)
    QString partialText(escapeOOSpan(paraText.mid(formatData.pos,formatData.len)));

    if (formatData.text.missing)
    {
        // It's just normal text, so we do not need a <text:span> element!
        *m_streamOut << partialText;
    }
    else
    { // Text with properties, so use a <text:span> element!
        *m_streamOut << "<text:span";

        QString styleKey;
        QString props ( textFormatToStyle(formatLayout,formatData.text,false,styleKey) ); // ### TODO: make const

        QMap<QString,QString>::ConstIterator it ( m_mapTextStyleKeys.find(styleKey) );
        kdDebug(30518) << "Searching text key: " << styleKey << endl;

        QString automaticStyle;
        if (it==m_mapTextStyleKeys.end())
        {
            // We have not any match, so we need a new automatic text style
            automaticStyle=makeAutomaticStyleName("T", m_automaticTextStyleNumber);
            kdDebug(30518) << "Creating automatic text style: " << automaticStyle << " key: " << styleKey << endl;
            m_mapTextStyleKeys[styleKey]=automaticStyle;

            m_contentAutomaticStyles += "  <style:style";
            m_contentAutomaticStyles += " style:name=\"" + escapeOOText(automaticStyle) + "\"";
            m_contentAutomaticStyles += " style:family=\"text\"";
            m_contentAutomaticStyles += ">\n";
            m_contentAutomaticStyles += "   <style:properties ";
            m_contentAutomaticStyles += props;
            m_contentAutomaticStyles += "/>\n";
            m_contentAutomaticStyles += "  </style:style>\n";
        }
        else
        {
            // We have a match, so use the already defined automatic text style
            automaticStyle=it.data();
            kdDebug(30518) << "Using automatic text style: " << automaticStyle << " key: " << styleKey << endl;
        }

        *m_streamOut << " text:style-name=\"" << escapeOOText(automaticStyle) << "\" ";

        *m_streamOut << ">" << partialText << "</text:span>";
    }
}

void OOWriterWorker::processFootnote( const VariableData& variable )
{
    // Footnote
    const QValueList<ParaData> *paraList = variable.getFootnotePara();
    if( paraList )
    {
        const QString value ( variable.getFootnoteValue() );
        //const bool automatic = formatData.variable.getFootnoteAuto();
        const bool flag = variable.getFootnoteType();

        if ( flag )
        {
            *m_streamOut << "<text:footnote text:id=\"ft";
            *m_streamOut << (++m_footnoteNumber);
            *m_streamOut << "\">";
            *m_streamOut << "<text:footnote-citation>" << escapeOOText( value ) << "</text:footnote-citation>";
            *m_streamOut << "<text:footnote-body>\n";

            doFullAllParagraphs( *paraList );

            *m_streamOut << "\n</text:footnote-body>";
            *m_streamOut << "</text:footnote>";
        }
        else
        {
            *m_streamOut << "<text:endnote text:id=\"ft";
            *m_streamOut << (++m_footnoteNumber);
            *m_streamOut << "\">";
            *m_streamOut << "<text:endnote-citation>" << escapeOOText( value ) << "</text:endnote-citation>";
            *m_streamOut << "<text:endnote-body>\n";

            doFullAllParagraphs( *paraList );

            *m_streamOut << "\n</text:endnote-body>";
            *m_streamOut << "</text:endnote>";
        }
    }
}

void OOWriterWorker::processNote( const VariableData& variable )
{
    // KWord 1.3's annotations are anonymous and undated,
    //  however the OO specification tells that author and date are mandatory (even if OOWriter 1.1 consider them optional)

    *m_streamOut << "<office:annotation office:create-date=\"";

    // We use the document creation date as creation date for the annotation
    // (OOWriter uses only the date part, there is no time part)
    if ( m_varSet.creationTime.isValid() )
        *m_streamOut << escapeOOText( m_varSet.creationTime.date().toString( Qt::ISODate ) );
    else
        *m_streamOut << "1970-01-01";

    *m_streamOut << "\" office:author=\"";

    // We try to use the document author's name as annotation author
    if ( m_docInfo.fullName.isEmpty() )
        *m_streamOut << escapeOOText( i18n( "Pseudo-author for annotations", "KWord 1.3" ) );
    else
        *m_streamOut << escapeOOText( m_docInfo.fullName );

    *m_streamOut << "\">\n";
    *m_streamOut << "<text:p>"
        << escapeOOSpan( variable.getGenericData( "note" ) )
        << "</text:p>\n"
        << "</office:annotation>";
}

void OOWriterWorker::processVariable ( const QString&,
    const TextFormatting& formatLayout,
    const FormatData& formatData)
{
    if (0==formatData.variable.m_type)
    {
        *m_streamOut << "<text:date/>"; // ### TODO: parameters
    }
    else if (2==formatData.variable.m_type)
    {
        *m_streamOut << "<text:time/>"; // ### TODO: parameters
    }
    else if (4==formatData.variable.m_type)
    {
        // ### TODO: the other under-types, other parameters
        if (formatData.variable.isPageNumber())
        {
            *m_streamOut << "<text:page-number text:select-page=\"current\"/>";
        }
        else if (formatData.variable.isPageCount())
        {
            *m_streamOut << "<text:page-count/>";
        }
        else
        {
            // Unknown subtype, therefore write out the result
            *m_streamOut << formatData.variable.m_text;
        }
    }
    else if (9==formatData.variable.m_type)
    {
        // A link
        *m_streamOut << "<text:a xlink:href=\""
            << escapeOOText(formatData.variable.getHrefName())
            << "\" xlink:type=\"simple\">"
            << escapeOOText(formatData.variable.getLinkName())
            << "</text:a>";
    }
    else if ( 10 == formatData.variable.m_type )
    {   // Note (OOWriter: annotation)
        processNote ( formatData.variable );
    }
    else if (11==formatData.variable.m_type)
    {
        // Footnote
        processFootnote ( formatData.variable );
    }
    else
    {
        // Generic variable
        *m_streamOut << formatData.variable.m_text;
    }
}

void OOWriterWorker::processAnchor ( const QString&,
    const TextFormatting& /*formatLayout*/, //TODO
    const FormatData& formatData)
{
    // We have an image or a table
    if ( (2==formatData.frameAnchor.type) // <IMAGE> or <PICTURE>
        || (5==formatData.frameAnchor.type) ) // <CLIPART>
    {
        makePicture(formatData.frameAnchor);
    }
    else if (6==formatData.frameAnchor.type)
    {
        makeTable(formatData.frameAnchor);
    }
    else
    {
        kdWarning(30518) << "Unsupported anchor type: "
            << formatData.frameAnchor.type << endl;
    }
}

void OOWriterWorker::processParagraphData ( const QString &paraText,
    const TextFormatting& formatLayout,
    const ValueListFormatData &paraFormatDataList)
{
    if ( paraText.length () > 0 )
    {
        ValueListFormatData::ConstIterator  paraFormatDataIt;

        for ( paraFormatDataIt = paraFormatDataList.begin ();
              paraFormatDataIt != paraFormatDataList.end ();
              paraFormatDataIt++ )
        {
            if (1==(*paraFormatDataIt).id)
            {
                processNormalText(paraText, formatLayout, (*paraFormatDataIt));
            }
            else if (4==(*paraFormatDataIt).id)
            {
                processVariable(paraText, formatLayout, (*paraFormatDataIt));
            }
            else if (6==(*paraFormatDataIt).id)
            {
                processAnchor(paraText, formatLayout, (*paraFormatDataIt));
            }
        }
    }
}

QString OOWriterWorker::layoutToParagraphStyle(const LayoutData& layoutOrigin,
    const LayoutData& layout, const bool force, QString& styleKey)
{
    QString props; // Props has to remain empty, if there is no difference.

    styleKey += layout.styleName;
    styleKey += ',';

    if (force || (layoutOrigin.alignment!=layout.alignment))
    {
        // NOTE: OO 1.0.x uses start and left like left and right (section 3.11.4)
        // Unfortunately in XSL-FO's text-align, they are really supposed to be the start and the end.
        if (layout.alignment == "left")
        {
            props += "fo:text-align=\"start\" ";
            styleKey += 'L';
        }
        else if (layout.alignment == "right")
        {
            props += "fo:text-align=\"end\" ";
            styleKey += 'R';
        }
        else if (layout.alignment == "center")
        {
            props += "fo:text-align=\"center\" ";
            styleKey += 'C';
        }
        else if (layout.alignment == "justify")
        {
            props += "fo:text-align=\"justify\" ";
            styleKey += 'J';
        }
        else if (layout.alignment == "auto")
        {
            props += "fo:text-align=\"left\" ";
            props += "style:text-auto-align=\"true\" "; // rejected draft OASIS extension
            styleKey += 'A';
        }
        else
        {
            kdWarning(30518) << "Unknown alignment: " << layout.alignment << endl;
        }
    }

    styleKey += ',';

    if ((layout.indentLeft>=0.0)
        && (force || (layoutOrigin.indentLeft!=layout.indentLeft)))
    {
        props += QString("fo:margin-left=\"%1pt\" ").arg(layout.indentLeft);
        styleKey += QString::number(layout.indentLeft);
    }

    styleKey += ',';

    if ((layout.indentRight>=0.0)
        && (force || (layoutOrigin.indentRight!=layout.indentRight)))
    {
        props += QString("fo:margin-right=\"%1pt\" ").arg(layout.indentRight);
        styleKey += QString::number(layout.indentRight);
    }

    styleKey += ',';

    if (force || (layoutOrigin.indentLeft!=layout.indentLeft))
    {
        props += "fo:text-indent=\"";
        props += QString::number(layout.indentFirst);
        props += "\" ";
        styleKey += QString::number(layout.indentFirst);
    }

    styleKey += ',';

    if ((layout.marginBottom>=0.0)
        && (force || (layoutOrigin.indentRight!=layout.indentRight)))
    {
       props += QString("fo:margin-bottom=\"%1pt\" ").arg(layout.marginBottom);
       styleKey += QString::number(layout.marginBottom);
    }

    styleKey += ',';

    if ((layout.marginTop>=0.0)
        && (force || (layoutOrigin.indentRight!=layout.indentRight)))
    {
       props += QString("fo:margin-top=\"%1pt\" ").arg(layout.marginTop);
       styleKey += QString::number(layout.marginTop);
    }

    styleKey += ',';

    // ### TODO: add support of at least, multiple...
    if (!force
        && (layoutOrigin.lineSpacingType==layoutOrigin.lineSpacingType)
        && (layoutOrigin.lineSpacing==layoutOrigin.lineSpacing))
    {
        // Do nothing!
    }
    else if (!layout.lineSpacingType)
    {
        // We have a custom line spacing (in points)
        props += QString("fo:line-height=\"%1pt\" ").arg(layout.lineSpacing);
        styleKey += QString::number(layout.lineSpacing);
    }
    // ### FIXME: it seems that it should be fo:line-height="normal"
    else if ( 10==layout.lineSpacingType  )
    {
        styleKey += "100%"; // One
    }
    else if ( 15==layout.lineSpacingType  )
    {
        props += "fo:line-height=\"150%\" "; // One-and-half
        styleKey += "150%";
    }
    else if ( 20==layout.lineSpacingType  )
    {
        props += "fo:line-height=\"200%\" "; // Two
        styleKey += "200%";
    }
    else
    {
        kdWarning(30518) << "Curious lineSpacingType: " << layout.lineSpacingType << " (Ignoring!)" << endl;
    }

    styleKey += ',';

    if ( layout.pageBreakBefore )
    {
        // We have a page break before the paragraph
        props += "fo:page-break-before=\"page\" ";
        styleKey += 'B';
    }

    styleKey += ',';

    if ( layout.pageBreakAfter )
    {
        // We have a page break after the paragraph
        props += "fo:page-break-after=\"page\" ";
        styleKey += 'A';
    }

    styleKey += '@'; // A more visible seperator

    props += textFormatToStyle(layoutOrigin.formatData.text,layout.formatData.text,force,styleKey);

    props += ">";

    styleKey += '@'; // A more visible seperator

    // ### TODO/FIXME: what if all tabulators must be erased?
    if (!layout.tabulatorList.isEmpty()
        && (force || (layoutOrigin.tabulatorList!=layout.tabulatorList) ))
    {
        props += "\n    <style:tab-stops>\n";
        TabulatorList::ConstIterator it;
        for (it=layout.tabulatorList.begin();it!=layout.tabulatorList.end();it++)
        {
            props+="     <style:tab-stop style:position=\"";
            props += QString::number((*it).m_ptpos);
            props += "pt\"";
            styleKey += QString::number((*it).m_ptpos);
            switch ((*it).m_type)
            {
                case 0:  props += " style:type=\"left\""; styleKey += "L"; break;
                case 1:  props += " style:type=\"center\""; styleKey += "C"; break;
                case 2:  props += " style:type=\"right\""; styleKey += "R"; break;
                case 3:  props += " style:type=\"char\" style:char=\".\""; styleKey += "D"; break; // decimal
                default: props += " style:type=\"left\""; styleKey += "L"; break;
            }
            switch ((*it).m_filling) // ### TODO: check if the characters are right
            {
                case TabulatorData::TF_NONE: break;
                case TabulatorData::TF_DOT:  props += " style:leader-char=\".\""; break;
                case TabulatorData::TF_LINE: props += " style:leader-char=\"_\""; break;

                case TabulatorData::TF_DASH:
                case TabulatorData::TF_DASHDOT:
                case TabulatorData::TF_DASHDOTDOT: props += " style:leader-char=\"-\""; break;

                default: break;
            }
            props += "/>\n";
            styleKey +='/';
        }
        props += "    </style:tab-stops>\n   ";
    }

    return props;
}

bool OOWriterWorker::doFullParagraph(const QString& paraText, const LayoutData& layout,
    const ValueListFormatData& paraFormatDataList)
{
    const bool header = ( (layout.counter.numbering == CounterData::NUM_CHAPTER)
        && (layout.counter.depth<10) ); // ### TODO: Does OOWriter really limits to 10?

    if (header)
    {
        *m_streamOut << "  <text:h text:level=\"";
        *m_streamOut << QString::number(layout.counter.depth+1,10);
        *m_streamOut << "\" ";
    }
    else
        *m_streamOut << "  <text:p ";

    const LayoutData& styleLayout=m_styleMap[layout.styleName];

    QString styleKey;
    const QString props(layoutToParagraphStyle(styleLayout,layout,false,styleKey));

    QString actualStyle(layout.styleName);
    if (!props.isEmpty())
    {
        QMap<QString,QString>::ConstIterator it ( m_mapParaStyleKeys.find(styleKey) );
        kdDebug(30518) << "Searching paragraph key: " << styleKey << endl;

        QString automaticStyle;

        if (it==m_mapParaStyleKeys.end())
        {
            // We have additional properties, so we need an automatic style for the paragraph
            automaticStyle = makeAutomaticStyleName("P", m_automaticParagraphStyleNumber);
            kdDebug(30518) << "Creating automatic paragraph style: " << automaticStyle << endl;
            m_mapParaStyleKeys[styleKey]=automaticStyle;

            m_contentAutomaticStyles += "  <style:style";
            m_contentAutomaticStyles += " style:name=\"" + escapeOOText(automaticStyle) + "\"";
            m_contentAutomaticStyles += " style:parent-style-name=\"" + escapeOOText(layout.styleName) + "\"";
            m_contentAutomaticStyles += " style:family=\"paragraph\" style:class=\"text\"";
            m_contentAutomaticStyles += ">\n";
            m_contentAutomaticStyles += "   <style:properties ";
            m_contentAutomaticStyles += props;
            m_contentAutomaticStyles += "</style:properties>\n";
            m_contentAutomaticStyles += "  </style:style>\n";
        }
        else
        {
            // We have a match, so use the already defined automatic paragraph style
            automaticStyle=it.data();
            kdDebug(30518) << "Using automatic paragraph style: " << automaticStyle << " key: " << styleKey << endl;
        }

        actualStyle=automaticStyle;
    }

    if (!actualStyle.isEmpty())
    {
        *m_streamOut << "text:style-name=\"" << escapeOOText(actualStyle) << "\" ";
    }
    else
    {   // SHould not happen
        kdWarning(30518) << "No style for a paragraph!" << endl;
    }

    *m_streamOut << ">";

    processParagraphData(paraText, layout.formatData.text, paraFormatDataList);

    // Before closing the paragraph, test if we have a page break

    if (header)
        *m_streamOut << "</text:h>\n";
    else
        *m_streamOut << "</text:p>\n";

    return true;
}

bool OOWriterWorker::doOpenStyles(void)
{
    m_styles += " <office:styles>\n";
    m_styles += "  <style:style style:name=\"Graphics\" style:family=\"graphics\">"; // ### TODO: what if Grpahics is a normal style
    m_styles += "   <style:properties/>";
    m_styles += "  </style:style>";
    return true;
}

bool OOWriterWorker::doFullDefineStyle(LayoutData& layout)
{
    //Register style in the style map
    m_styleMap[layout.styleName]=layout;

    m_styles += "  <style:style";

    m_styles += " style:name=\"" + EscapeXmlText(layout.styleName,true,true) + "\"";
    m_styles += " style:next-style-name=\"" + EscapeXmlText(layout.styleFollowing,true,true) + "\"";
    m_styles += " style:family=\"paragraph\" style:class=\"text\"";
    m_styles += ">\n";
    m_styles += "   <style:properties ";

    QString dummyKey; // Not needed
    m_styles += layoutToParagraphStyle(layout,layout,true,dummyKey);

    m_styles += "</style:properties>\n";
    m_styles += "  </style:style>\n";

    return true;
}

bool OOWriterWorker::doCloseStyles(void)
{
    m_styles += " </office:styles>\n";
    return true;
}


bool OOWriterWorker::doFullPaperFormat(const int format,
            const double width, const double height, const int orientation)
{
    m_paperFormat=format;
    m_paperWidth=width;
    m_paperHeight=height;
    m_paperOrientation=orientation;
    return true;
}

bool OOWriterWorker::doFullPaperBorders (const double top, const double left,
    const double bottom, const double right)
{
    m_paperBorderTop=top;
    m_paperBorderLeft=left;
    m_paperBorderBottom=bottom;
    m_paperBorderRight=right;
    return true;
}

bool OOWriterWorker::doFullDocumentInfo(const KWEFDocumentInfo& docInfo)
{
    m_docInfo=docInfo;

    return true;
}

bool OOWriterWorker::doVariableSettings(const VariableSettingsData& vs)
{
    m_varSet=vs;

    return true;
}

void OOWriterWorker::declareFont(const QString& fontName)
{
    if (fontName.isEmpty())
        return;
        
    if (m_fontNames.find(fontName)==m_fontNames.end())
    {
        QString props;

        // Disabled, as QFontInfo::styleHint() cannot guess
#if 0
        QFont font(fontName);
        QFontInfo info(font);
        props+="style:font-family-generic=\""
        switch (info.styleHint())
        {
        case QFont::SansSerif:
        default:
            {
                props += "swiss";
                break;
            }
        case QFont::Serif:
            {
                props +=  "roman";
                break;
            }
        case QFont::Courier:
            {
                props +=  "modern";
                break;
            }
        case QFont::OldEnglish:
            {
                props +=  "decorative";
                break;
            }
        }
        props +="\" ";
#endif

        props +="style:font-pitch=\"variable\""; // ### TODO: check if font is variable or fixed
        // New font, so register it
        m_fontNames[fontName]=props;
    }
}

QString OOWriterWorker::makeAutomaticStyleName(const QString& prefix, ulong& counter) const
{
    const QString str (prefix + QString::number(++counter,10));

    // Checks if the automatic style has not the same name as a user one.
    // If it is the case, change it!

    if (m_styleMap.find(str)==m_styleMap.end())
        return str; // Unique, so let's go!

    QString str2(str+"_bis");
    if (m_styleMap.find(str2)==m_styleMap.end())
        return str2;

    str2 = str+"_ter";
    if (m_styleMap.find(str2)==m_styleMap.end())
        return str2;

    // If it is still not unique, try a time stamp.
    const QDateTime dt(QDateTime::currentDateTime(Qt::UTC));

    str2 = str + "_" + QString::number(dt.toTime_t(),16);
    if (m_styleMap.find(str2)==m_styleMap.end())
        return str2;

    kdWarning(30518) << "Could not make an unique style name: " << str2 << endl;
    return str2; // Still return, as we have nothing better
}

