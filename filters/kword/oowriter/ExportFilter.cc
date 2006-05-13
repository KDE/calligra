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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
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

#include <QMap>
#include <QIODevice>
#include <QBuffer>
#include <QTextStream>
#include <qdom.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <Q3CString>
#include <Q3MemArray>
#include <QBuffer>

#include <kdebug.h>
#include <klocale.h>
#include <kzip.h>

#include <KoPageLayout.h>
#include <KoPictureKey.h>
#include <KoPicture.h>

#include <KWEFStructures.h>
#include <KWEFUtil.h>
#include <KWEFBaseWorker.h>
#include <KWEFKWordLeader.h>

#include "ExportFilter.h"

OOWriterWorker::OOWriterWorker(void) : m_streamOut(NULL),
    m_paperBorderTop(0.0),m_paperBorderLeft(0.0),
    m_paperBorderBottom(0.0),m_paperBorderRight(0.0), m_zip(NULL), m_pictureNumber(0),
    m_automaticParagraphStyleNumber(0), m_automaticTextStyleNumber(0),
    m_footnoteNumber(0), m_tableNumber(0), m_textBoxNumber( 0 ),
    m_columnspacing( 36.0 ), m_columns( 1 )
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
        case  0: 
        case  2:
        case  3:
        case  4:
        case  5:
        case  6:
        case  7:
        case  8:
        case 11: 
        case 12: 
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 28:
        case 29:
        case 30:
        case 31:
            {
                kWarning(30518) << "Not allowed XML character: " << ch.unicode() << endl;
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
    kDebug(30518) << "Opening file: " << filenameOut
        << " (in OOWriterWorker::doOpenFile)" << endl;

    m_zip=new KZip(filenameOut); // How to check failure?

    if (!m_zip->open(QIODevice::WriteOnly))
    {
        kError(30518) << "Could not open ZIP file for writing! Aborting!" << endl;
        delete m_zip;
        m_zip=NULL;
        return false;
    }

    m_zip->setCompression( KZip::NoCompression );
    m_zip->setExtraField( KZip::NoExtraField );

    const Q3CString appId( "application/vnd.sun.xml.writer" );

    m_zip->writeFile( "mimetype", QString::null, QString::null,appId.data(), appId.length() );

    m_zip->setCompression( KZip::DeflateCompression );

    m_streamOut=new QTextStream(m_contentBody, QIODevice::WriteOnly);

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
    return m_zip->finishWriting(m_size);
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

bool OOWriterWorker::zipWriteData(const Q3CString& cstr)
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
	QMap<QString,QString>::ConstIterator end(m_fontNames.end());
    for (QMap<QString,QString>::ConstIterator it=m_fontNames.begin(); it!=end; ++it)
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
    zipWriteData( " style:page-usage=\"all\"" ); // ### TODO: check

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
    zipWriteData( "\">\n" );

    if ( m_columns > 1 )
    {
        zipWriteData( "    <style:columns" );
        zipWriteData( " fo:column-count=\"" );
        zipWriteData( QString::number( m_columns ) );
        zipWriteData( "\" fo:column-gap=\"" );
        zipWriteData( QString::number( m_columnspacing ) );
        zipWriteData( "pt\">\n" );

        for (int i=0; i < m_columns; ++i)
        {
            zipWriteData( "     <style:column style:rel-width=\"1*\" fo:margin-left=\"0cm\" fo:margin-right=\"0cm\"/>\n" );
        }

        zipWriteData( "    </style:columns>\n" );
    }

    zipWriteData("   </style:properties>\n");

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

    zipWriteData( "  <meta:document-statistic" );

    // KWord files coming from import filters mostly do not have no page count
    if ( m_numPages > 0 )
    {
        zipWriteData( " meta:page-count=\"" );
        zipWriteData( QString::number ( m_numPages ) );
        zipWriteData( "\"" );
    }

    zipWriteData( " meta:image-count=\"" ); // This is not specified in the OO specification section 2.1.19
    zipWriteData( QString::number ( m_pictureNumber ) );
    zipWriteData( "\"" );

    zipWriteData( " meta:table-count=\"" );
    zipWriteData( QString::number ( m_tableNumber ) );
    zipWriteData( "\"" );

    zipWriteData( "/>\n" ); // meta:document-statistic
    
    zipWriteData(" </office:meta>\n");
    zipWriteData("</office:document-meta>\n");

    zipDoneWriting();
}

bool OOWriterWorker::doCloseFile(void)
{
    kDebug(30518)<< "OOWriterWorker::doCloseFile" << endl;
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
    kDebug(30518)<< "OOWriterWorker::doOpenDocument" << endl;

    *m_streamOut << " <office:body>\n";

    return true;
}

bool OOWriterWorker::doCloseDocument(void)
{
    *m_streamOut << " </office:body>\n";
    return true;
}

bool OOWriterWorker::doOpenBody(void)
{
    Q3ValueList<FrameAnchor>::Iterator it;

    // We have to process all non-inline pictures
    kDebug(30518) << "=== Processing non-inlined pictures ===" << endl;
    for ( it = m_nonInlinedPictureAnchors.begin(); it != m_nonInlinedPictureAnchors.end(); ++it )
    {
        *m_streamOut << "  ";
        makePicture( *it, AnchorNonInlined );
        *m_streamOut << "\n";
    }
    kDebug(30518) << "=== Non-inlined pictures processed ===" << endl;

    // We have to process all non-inline tables
    kDebug(30518) << "=== Processing non-inlined tables ===" << endl;
    for ( it = m_nonInlinedTableAnchors.begin(); it != m_nonInlinedTableAnchors.end(); ++it )
    {
        *m_streamOut << "  ";
        makeTable( *it, AnchorNonInlined );
        *m_streamOut << "\n";
    }
    kDebug(30518) << "=== Non-inlined tables processed ===" << endl;

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

    key += ',';

    if ( force || ( formatOrigin.language != formatData.language ) )
    {
        const QString lang ( formatData.language );
        if ( ! lang.isEmpty() )
        {
            const int res = lang.find( '_' );

            if ( res >= 0 )
            {
                kDebug(30518) << "Language: " << lang << " => " << lang.left( res ) << " - " << lang.mid( res + 1 ) << endl;
                strElement += "fo:language=\"";
                strElement += lang.left( res );
                strElement += "\" ";
                strElement += "fo:country=\"";
                strElement += lang.mid( res + 1 );
                strElement += "\" ";
            }
            else
            {
                kDebug(30518) << "Language without country: " << lang << endl;
                strElement += "fo:language=\"";
                strElement += lang;
                strElement += "\" ";
            }

            key+=formatData.language;
        }
    }

    key += ",";

    if ( force || ( formatOrigin.fontAttribute != formatData.fontAttribute ) )
    {
        // Note: OOWriter does not like when both fo:text-transform and fo:font-variant exist (except if both are none/normal)
        // (It is documented so, see sections 3.10.1 and 3.10.2)
        if ( formatData.fontAttribute == "uppercase" )
        {
            strElement += "fo:text-transform=\"uppercase\" ";
            key += 'U';
        }
        else if ( formatData.fontAttribute == "lowercase" )
        {
            strElement += "fo:text-transform=\"lowercase\" ";
            key += 'L';
        }
        else if ( formatData.fontAttribute == "smallcaps" )
        {
            strElement += "fo:font-variant=\"small-caps\" ";
            key += 'S';
        }
        else
        {
            strElement += "fo:text-transform=\"none\" ";
            strElement += "fo:font-variant=\"normal\" ";
            key += 'N';
        }
    }

    key += ",";

    if ( force || ( formatOrigin.verticalAlignment != formatData.verticalAlignment ) )
    {
        if ( 1 == formatData.verticalAlignment )
        {
            //Subscript
            strElement += "style:text-position=\"sub\" ";
            key += 'B';
        }
        else if ( 2 == formatData.verticalAlignment )
        {
            //Superscript
            strElement += "style:text-position=\"super\" ";
            key += 'P';
        }
        // ### TODO: how to reset it? "0pt" ?
    }

    return strElement.trimmed(); // Remove especially trailing spaces
}

#define ALLOW_TABLE

QString OOWriterWorker::cellToProperties( const TableCell& cell, QString& key) const
{
#ifdef ALLOW_TABLE
    const FrameData& frame = cell.frame;
    QString properties;

    key += "!L"; // left border
    key += frame.lColor.name();
    key += ",";
    key += QString::number( frame.lWidth );
    properties += " fo:border-left=\"";
    if ( frame.lColor.isValid() && frame.lWidth > 0.0 )
    {
        properties += QString::number( frame.lWidth );
        properties += "pt";
        properties += " solid "; // ### TODO
        properties += frame.lColor.name();
    }
    else
    {
        properties += "0pt none #000000";
    }
    properties += "\"";

    key += "!R"; // right border
    key += frame.rColor.name();
    key += ",";
    key += QString::number( frame.rWidth );
    properties += " fo:border-right=\"";
    if ( frame.rColor.isValid() && frame.rWidth > 0.0 )
    {
        properties += QString::number( frame.rWidth );
        properties += "pt";
        properties += " solid "; // ### TODO
        properties += frame.rColor.name();
    }
    else
    {
        properties += "0pt none #000000";
    }
    properties += "\"";

    key += "!T"; // top border
    key += frame.tColor.name();
    key += ",";
    key += QString::number( frame.tWidth );
    properties += " fo:border-top=\"";
    if ( frame.tColor.isValid() && frame.tWidth > 0.0 )
    {
        properties += QString::number( frame.tWidth );
        properties += "pt";
        properties += " solid "; // ### TODO
        properties += frame.tColor.name();
    }
    else
    {
        properties += "0pt none #000000";
    }
    properties += "\"";

    key += "!B"; // bottom border
    key += frame.bColor.name();
    key += ",";
    key += QString::number( frame.bWidth );
    properties += " fo:border-bottom=\"";
    if ( frame.bColor.isValid() && frame.bWidth > 0.0 )
    {
        properties += QString::number( frame.bWidth );
        properties += "pt";
        properties += " solid "; // ### TODO
        properties += frame.bColor.name();
    }
    else
    {
        properties += "0pt none #000000";
    }
    properties += "\"";

    return properties;
#else
    return QString::null;
#endif
}

bool OOWriterWorker::makeTableRows( const QString& tableName, const Table& table, int firstRowNumber )
{
#ifdef ALLOW_TABLE
    // ### TODO: rows
    // ### TODO:  be careful that covered-cell can be due vertical spanning.
    // ### TODO:  One way to find is the difference between the row variables (or against the last known column)
    // ### TODO:  be careful that fully-covered rows might exist.

    *m_streamOut << "<table:table-row>\n";
    int rowCurrent = firstRowNumber;

    ulong cellNumber = 0L;

    QMap<QString,QString> mapCellStyleKeys;

    for ( Q3ValueList<TableCell>::ConstIterator itCell ( table.cellList.begin() );
        itCell != table.cellList.end(); ++itCell)
    {
        if ( rowCurrent != (*itCell).row )
        {
            rowCurrent = (*itCell).row;
            *m_streamOut << "</table:table-row>\n";
            *m_streamOut << "<table:table-row>\n";
        }

        QString key;
        const QString props ( cellToProperties( (*itCell), key ) );

        QString automaticCellStyle;
        QMap<QString,QString>::ConstIterator it ( mapCellStyleKeys.find( key ) );
        if ( it == mapCellStyleKeys.end() )
        {
            automaticCellStyle = makeAutomaticStyleName( tableName + ".Cell", cellNumber );
            mapCellStyleKeys [ key ] = automaticCellStyle;
            kDebug(30518) << "Creating automatic cell style: " << automaticCellStyle  << " key: " << key << endl;
            m_contentAutomaticStyles += "  <style:style";
            m_contentAutomaticStyles += " style:name=\"" + escapeOOText( automaticCellStyle ) + "\"";
            m_contentAutomaticStyles += " style:family=\"table-cell\"";
            m_contentAutomaticStyles += ">\n";
            m_contentAutomaticStyles += "   <style:properties ";
            m_contentAutomaticStyles += props;
            m_contentAutomaticStyles += "/>\n";
            m_contentAutomaticStyles += "  </style:style>\n";
        }
        else
        {
            automaticCellStyle = it.data();
            kDebug(30518) << "Using automatic cell style: " << automaticCellStyle  << " key: " << key << endl;
        }

        *m_streamOut << "<table:table-cell table:value-type=\"string\" table:style-name=\""
            << escapeOOText( automaticCellStyle)
            << "\"";

        // More than one column width?
        {
            *m_streamOut << " table:number-columns-spanned=\"" << (*itCell).m_cols << "\"";
        }

        *m_streamOut << ">\n";

        if (!doFullAllParagraphs(*(*itCell).paraList))
        {
            return false;
        }

        *m_streamOut << "</table:table-cell>\n";

        if ( (*itCell).m_cols > 1 )
        {
            // We need to add some placeholder for the "covered" cells
            for (int i = 1; i < (*itCell).m_cols; ++i)
            {
                *m_streamOut << "<table:covered-table-cell/>";
            }
        }
    }

    *m_streamOut << "</table:table-row>\n";
    return true;
#else
    return false;
#endif
}

#ifdef ALLOW_TABLE
static uint getColumnWidths( const Table& table, Q3MemArray<double>& widthArray, int firstRowNumber )
{
    bool uniqueColumns = true; // We have not found any horizontally spanned cells yet.
    uint currentColumn = 0;
    int tryingRow = firstRowNumber; // We are trying the first row
    Q3ValueList<TableCell>::ConstIterator itCell;

    for ( itCell = table.cellList.begin();
        itCell != table.cellList.end(); ++itCell )
    {
        kDebug(30518) << "Column: " << (*itCell).col << " (Row: " << (*itCell).row << ")" << endl;

        if ( (*itCell).row != tryingRow )
        {
            if ( uniqueColumns )
            {
                 // We had a full row without any horizontally spanned cell, so we have the needed data
                return currentColumn;
            }
            else
            {
                // No luck in the previous row, so now try this new one
                tryingRow = (*itCell).row;
                uniqueColumns = true;
                currentColumn = 0;
            }
        }

        if ( (*itCell).m_cols > 1 )
        {
            // We have a horizontally spanned cell
            uniqueColumns = false;
            // Do not waste the time to calculate the width
            continue;
        }

        const double width = ( (*itCell).frame.right - (*itCell).frame.left );

        if ( currentColumn >= widthArray.size() )
            widthArray.resize( currentColumn + 4, Q3GArray::SpeedOptim);

        widthArray.at( currentColumn ) = width;
        ++currentColumn;
    }

    // If we are here, it can be:
    // - the table is either empty or there is not any row without horizontally spanned cells
    // - we have needed the last row for getting something usable

    return uniqueColumns ? currentColumn : 0;
}
#endif

#ifdef ALLOW_TABLE
static uint getFirstRowColumnWidths( const Table& table, Q3MemArray<double>& widthArray, int firstRowNumber )
// Get the column widths only by the first row.
// This is used when all table rows have horizontally spanned cells.
{
    uint currentColumn = 0;
    Q3ValueList<TableCell>::ConstIterator itCell;

    for ( itCell = table.cellList.begin();
        itCell != table.cellList.end(); ++itCell )
    {
        kDebug(30518) << "Column: " << (*itCell).col << " (Row: " << (*itCell).row << ")" << endl;
        if ( (*itCell).row != firstRowNumber )
            break; // We have finished the first row

        int cols = (*itCell).m_cols;
        if ( cols < 1)
            cols = 1;

        // ### FIXME: the columns behind a larger cell do not need to be symmetrical
        const double width = ( (*itCell).frame.right - (*itCell).frame.left ) / cols;

        if ( currentColumn + cols > widthArray.size() )
            widthArray.resize( currentColumn + 4, Q3GArray::SpeedOptim);

        for ( int i = 0; i < cols; ++i )
        {
            widthArray.at( currentColumn ) = width;
            ++currentColumn;
        }
    }
    return currentColumn;
}
#endif

bool OOWriterWorker::makeTable( const FrameAnchor& anchor, const AnchorType anchorType )
{
#ifdef ALLOW_TABLE

    // Be careful that while being similar the following 5 strings have different purposes
    const QString automaticTableStyle ( makeAutomaticStyleName( "Table", m_tableNumber ) ); // It also increases m_tableNumber
    const QString tableName( QString( "Table" ) + QString::number( m_tableNumber ) ); // m_tableNumber was already increased
    const QString translatedName( i18nc( "Object name", "Table %1", m_tableNumber ) );
    const QString automaticFrameStyle ( makeAutomaticStyleName( "TableFrame", m_textBoxNumber ) ); // It also increases m_textBoxNumber
    const QString translatedFrameName( i18nc( "Object name", "Table Frame %1", m_textBoxNumber ) );

    kDebug(30518) << "Processing table " << anchor.key.toString() << " => " << tableName << endl;

    const Q3ValueList<TableCell>::ConstIterator firstCell ( anchor.table.cellList.begin() );

    if ( firstCell == anchor.table.cellList.end() )
    {
        kError(30518) << "Table has not any cell!" << endl;
        return false;
    }

    const int firstRowNumber = (*firstCell).row;
    kDebug(30518) << "First row: " << firstRowNumber << endl;

    Q3MemArray<double> widthArray(4);

    uint numberColumns = getColumnWidths( anchor.table, widthArray, firstRowNumber );

    if ( numberColumns <= 0 )
    {
        kDebug(30518) << "Could not get correct column widths, so approximating" << endl;
        // There was a problem, the width array cannot be trusted, so try to do a column width array with the first row
        numberColumns = getFirstRowColumnWidths( anchor.table, widthArray, firstRowNumber );
        if ( numberColumns <= 0 )
        {
            // Still not right? Then it is an error!
            kError(30518) << "Cannot get column widths of table " << anchor.key.toString() << endl;
            return false;
        }
    }

    kDebug(30518) << "Number of columns: " << numberColumns << endl;


    double tableWidth = 0.0; // total width of table
    uint i; // We need the loop variable 2 times
    for ( i=0; i < numberColumns; ++i )
    {
        tableWidth += widthArray.at( i );
    }
    kDebug(30518) << "Table width: " << tableWidth << endl;

    // An inlined table, is an "as-char" text-box
    *m_streamOut << "<draw:text-box";
    *m_streamOut << " style:name=\"" << escapeOOText( automaticFrameStyle ) << "\"";
    *m_streamOut << " draw:name=\"" << escapeOOText( translatedFrameName ) << "\"";
    if ( anchorType == AnchorNonInlined )
    {
        // ### TODO: correctly set a OOWriter frame positioned on the page
        *m_streamOut << " text:anchor-type=\"paragraph\"";
    }
    else
    {
        *m_streamOut << " text:anchor-type=\"as-char\"";
    }
    *m_streamOut << " svg:width=\"" << tableWidth << "pt\""; // ### TODO: any supplement to the width?
    //*m_streamOut << " fo:min-height=\"1pt\"";// ### TODO: a better height (can be calulated from the KWord table frames)
    *m_streamOut << ">\n";

    *m_streamOut << "<table:table table:name=\""
        << escapeOOText( translatedName )
        << "\" table:style-name=\""
        << escapeOOText( automaticTableStyle )
        << "\" >\n";


    // Now we have enough information to generate the style for the table and its frame

    kDebug(30518) << "Creating automatic frame style: " << automaticFrameStyle /* << " key: " << styleKey */ << endl;
    m_contentAutomaticStyles += "  <style:style"; // for frame
    m_contentAutomaticStyles += " style:name=\"" + escapeOOText( automaticFrameStyle ) + "\"";
    m_contentAutomaticStyles += " style:family=\"graphics\"";
    m_contentAutomaticStyles += " style:parent-style-name=\"Frame\""; // ### TODO: parent style needs to be correctly defined
    m_contentAutomaticStyles += ">\n";
    m_contentAutomaticStyles += "   <style:properties "; // ### TODO
    m_contentAutomaticStyles += " text:anchor-type=\"as-char\""; // ### TODO: needed?
    m_contentAutomaticStyles += " fo:padding=\"0pt\" fo:border=\"none\"";
    m_contentAutomaticStyles += " fo:margin-left=\"0pt\"";
    m_contentAutomaticStyles += " fo:margin-top=\"0pt\"";
    m_contentAutomaticStyles += " fo:margin-bottom=\"0pt\"";
    m_contentAutomaticStyles += " fo:margin-right=\"0pt\"";
    m_contentAutomaticStyles += "/>\n";
    m_contentAutomaticStyles += "  </style:style>\n";

    kDebug(30518) << "Creating automatic table style: " << automaticTableStyle /* << " key: " << styleKey */ << endl;
    m_contentAutomaticStyles += "  <style:style"; // for table
    m_contentAutomaticStyles += " style:name=\"" + escapeOOText( automaticTableStyle ) + "\"";
    m_contentAutomaticStyles += " style:family=\"table\"";
    m_contentAutomaticStyles += ">\n";
    m_contentAutomaticStyles += "   <style:properties ";
    m_contentAutomaticStyles += " style:width=\"" + QString::number( tableWidth ) + "pt\" ";
    m_contentAutomaticStyles += "/>\n";
    m_contentAutomaticStyles += "  </style:style>\n";

    Q3ValueList<TableCell>::ConstIterator itCell;

    ulong columnNumber = 0L;

    for ( i=0; i < numberColumns; ++i )
    {
        const QString automaticColumnStyle ( makeAutomaticStyleName( tableName + ".Column", columnNumber ) );
        kDebug(30518) << "Creating automatic column style: " << automaticColumnStyle /* << " key: " << styleKey */ << endl;

        m_contentAutomaticStyles += "  <style:style";
        m_contentAutomaticStyles += " style:name=\"" + escapeOOText( automaticColumnStyle ) + "\"";
        m_contentAutomaticStyles += " style:family=\"table-column\"";
        m_contentAutomaticStyles += ">\n";
        m_contentAutomaticStyles += "   <style:properties ";
        // Despite that some OO specification examples use fo:width, OO specification section 4.19 tells to use style:column-width
        //  and/or the relative variant: style:rel-column-width
        m_contentAutomaticStyles += " style:column-width=\"" + QString::number( widthArray.at( i ) ) + "pt\" ";
        m_contentAutomaticStyles += "/>\n";
        m_contentAutomaticStyles += "  </style:style>\n";

        // ### TODO: find a way how to use table:number-columns-repeated for more that one cell's column(s)
        *m_streamOut << "<table:table-column table:style-name=\""
            << escapeOOText( automaticColumnStyle )
            << "\" table:number-columns-repeated=\"1\"/>\n";
    }

    makeTableRows( tableName, anchor.table, firstRowNumber );

    *m_streamOut << "</table:table>\n";

    *m_streamOut << "</draw:text-box>"; // End of inline

#endif
    return true;
}

bool OOWriterWorker::makePicture( const FrameAnchor& anchor, const AnchorType anchorType )
{
    kDebug(30518) << "New picture: " << anchor.picture.koStoreName
        << " , " << anchor.picture.key.toString() << endl;

    const QString koStoreName(anchor.picture.koStoreName);

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
        kWarning(30518) << "Unable to load picture: " << koStoreName << endl;
        return true;
    }

    kDebug(30518) << "Picture loaded: " << koStoreName << endl;

    double height = 0.0;
    double width = 0.0;

    if ( anchorType == AnchorTextImage )
    {
        // Text image have no frameset, so the only size information is in the picture itself.
        QByteArray tmp(image);
		QBuffer buffer( &tmp ); // Be more safe than sorry and do not allow shallow copy
        KoPicture pic;
        buffer.open( QIODevice::ReadOnly );
        if ( pic.load( &buffer, strExtension ) )
        {
            const QSize size ( pic.getOriginalSize() );
            height = size.height();
            width = size.width();
        }
        else
        {
            kWarning(30518) << "Could not load KoPicture: " << koStoreName << endl;
        }
        buffer.close();
    }
    else
    {
        // Use frame size
        height=anchor.frame.bottom - anchor.frame.top;
        width =anchor.frame.right  - anchor.frame.left;
    }

    if ( height < 1.0 )
    {
        kWarning(30518) << "Silly height for " << koStoreName << " : "  << height << endl;
        height = 72.0;
    }
    if ( width < 1.0 )
    {
        kWarning(30518) << "Silly width for " << koStoreName << " : "  << width << endl;
        width = 72.0;
    }

     // We need a 32 digit hex value of the picture number
     // Please note: it is an exact 32 digit value, truncated if the value is more than 512 bits wide. :-)
    QString number;
    number.fill('0',32);
    number += QString::number(++m_pictureNumber,16); // in hex

    QString ooName("Pictures/");
    ooName += number.right(32);
    ooName += '.';
    ooName += strExtension;

    kDebug(30518) << "Picture " << koStoreName << " => " << ooName << endl;

    // TODO: we are only using the filename, not the rest of the key
    // TODO:  (bad if there are two images of the same name, but of a different key)
    *m_streamOut << "<draw:image draw:name=\"" << anchor.picture.key.filename() << "\"";
    *m_streamOut << " draw:style-name=\"Graphics\""; // ### TODO: should be an automatic "graphic" style name instead
    if ( anchorType == AnchorNonInlined )
    {
        // ### TODO: correctly set a OOWriter frame positioned on the page
        *m_streamOut << " text:anchor-type=\"paragraph\"";
    }
    else
    {
        *m_streamOut << " text:anchor-type=\"as-char\"";
    }
    *m_streamOut << " svg:height=\"" << height << "pt\" svg:width=\"" << width << "pt\"";
    *m_streamOut << " draw:z-index=\"0\" xlink:href=\"#" << ooName << "\"";
    *m_streamOut << " xlink:type=\"simple\" xlink:show=\"embed\" xlink:actuate=\"onLoad\"";
    *m_streamOut << "/>"; // NO end of line!

    if (m_zip)
    {
#if 0
        // ### FIXME Why is the following line not working (at least with KDE 3.1)? (It makes unzip having problems with meta.xml)
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
    const QString partialText( escapeOOSpan( paraText.mid( formatData.pos, formatData.len ) ) );

    if (formatData.text.missing)
    {
        // It's just normal text, so we do not need a <text:span> element!
        *m_streamOut << partialText;
    }
    else
    { // Text with properties, so use a <text:span> element!
        *m_streamOut << "<text:span";

        QString styleKey;
        const QString props ( textFormatToStyle(formatLayout,formatData.text,false,styleKey) );

        QMap<QString,QString>::ConstIterator it ( m_mapTextStyleKeys.find(styleKey) );
        kDebug(30518) << "Searching text key: " << styleKey << endl;

        QString automaticStyle;
        if (it==m_mapTextStyleKeys.end())
        {
            // We have not any match, so we need a new automatic text style
            automaticStyle=makeAutomaticStyleName("T", m_automaticTextStyleNumber);
            kDebug(30518) << "Creating automatic text style: " << automaticStyle << " key: " << styleKey << endl;
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
            kDebug(30518) << "Using automatic text style: " << automaticStyle << " key: " << styleKey << endl;
        }

        *m_streamOut << " text:style-name=\"" << escapeOOText(automaticStyle) << "\" ";

        *m_streamOut << ">" << partialText << "</text:span>";
    }
}

void OOWriterWorker::processFootnote( const VariableData& variable )
{
    // Footnote
    const Q3ValueList<ParaData> *paraList = variable.getFootnotePara();
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
        *m_streamOut << escapeOOText( i18nc( "Pseudo-author for annotations", "KWord 1.3" ) );
    else
        *m_streamOut << escapeOOText( m_docInfo.fullName );

    *m_streamOut << "\">\n";
    *m_streamOut << "<text:p>"
        << escapeOOSpan( variable.getGenericData( "note" ) )
        << "</text:p>\n"
        << "</office:annotation>";
}

void OOWriterWorker::processVariable ( const QString&,
    const TextFormatting& /*formatLayout*/,
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
    // We have a picture or a table
    if ( (2==formatData.frameAnchor.type) // <IMAGE> or <PICTURE>
        || (5==formatData.frameAnchor.type) ) // <CLIPART>
    {
        makePicture( formatData.frameAnchor, AnchorInlined );
    }
    else if (6==formatData.frameAnchor.type)
    {
        makeTable( formatData.frameAnchor, AnchorInlined );
    }
    else
    {
        kWarning(30518) << "Unsupported anchor type: "
            << formatData.frameAnchor.type << endl;
    }
}

void OOWriterWorker::processTextImage ( const QString&,
    const TextFormatting& /*formatLayout*/,
    const FormatData& formatData)
{
    kDebug(30518) << "Text Image: " << formatData.frameAnchor.key.toString() << endl;
    makePicture( formatData.frameAnchor, AnchorTextImage );
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
            else if (2==(*paraFormatDataIt).id)
            {
                processTextImage(paraText, formatLayout, (*paraFormatDataIt));
            }
            else if ( 3 == (*paraFormatDataIt).id )
            {
                // Just a (KWord 0.8) tab stop, nothing else to do!
                *m_streamOut << "<text:tab-stop/>";
            }
            else if (4==(*paraFormatDataIt).id)
            {
                processVariable(paraText, formatLayout, (*paraFormatDataIt));
            }
            else if (6==(*paraFormatDataIt).id)
            {
                processAnchor(paraText, formatLayout, (*paraFormatDataIt));
            }
            else if ( 1001 == (*paraFormatDataIt).id ) // Start of bookmark
            {
                *m_streamOut << "<text:bookmark-start text:name=\""
                    << escapeOOText( (*paraFormatDataIt).variable.m_text )
                    <<"\"/>";
            }
            else if ( 1002 == (*paraFormatDataIt).id ) // End of bookmark
            {
                *m_streamOut << "<text:bookmark-end text:name=\""
                    << escapeOOText( (*paraFormatDataIt).variable.m_text )
                    <<"\"/>";
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
        // NOTE: OO 1.0.x uses start and end like left and right (section 3.11.4)
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
            props += "fo:text-align=\"start\" ";
#ifndef STRICT_OOWRITER_VERSION_1
            props += "style:text-auto-align=\"true\" "; // rejected draft OASIS extension
#endif	    
            styleKey += 'A';
        }
        else
        {
            kWarning(30518) << "Unknown alignment: " << layout.alignment << endl;
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
        && ( force || ( layoutOrigin.marginBottom != layout.marginBottom ) ) )
    {
       props += QString("fo:margin-bottom=\"%1pt\" ").arg(layout.marginBottom);
       styleKey += QString::number(layout.marginBottom);
    }

    styleKey += ',';

    if ((layout.marginTop>=0.0)
        && ( force || ( layoutOrigin.marginTop != layout.marginTop ) ) )
    {
       props += QString("fo:margin-top=\"%1pt\" ").arg(layout.marginTop);
       styleKey += QString::number(layout.marginTop);
    }

    styleKey += ',';

    if (force
        || ( layoutOrigin.lineSpacingType != layout.lineSpacingType )
        || ( layoutOrigin.lineSpacing != layout.lineSpacing ) )
    {
        switch ( layout.lineSpacingType )
        {
        case LayoutData::LS_CUSTOM:
            {
                // We have a custom line spacing (in points)
                const QString height ( QString::number(layout.lineSpacing) ); // ### TODO: rounding?
                props += "style:line-spacing=\"";
                props += height;
                props += "pt\" ";
                styleKey += height;
                styleKey += 'C';
                break;
            }
        case LayoutData::LS_SINGLE:
            {
                props += "fo:line-height=\"normal\" "; // One
                styleKey += "100%"; // One
                break;
            }
        case LayoutData::LS_ONEANDHALF:
            {
                props += "fo:line-height=\"150%\" "; // One-and-half
                styleKey += "150%";
                break;
            }
        case LayoutData::LS_DOUBLE:
            {
                props += "fo:line-height=\"200%\" "; // Two
                styleKey += "200%";
                break;
            }
        case LayoutData::LS_MULTIPLE:
            {
                // OOWriter 1.1 only allows up to 200%
                const QString mult ( QString::number( qRound( layout.lineSpacing * 100 ) ) );
                props += "fo:line-height=\"";
                props += mult;
                props += "%\" ";
                styleKey += mult;
                styleKey += "%";
                break;
            }
        case LayoutData::LS_FIXED:
            {
                // We have a fixed line height (in points)
                const QString height ( QString::number(layout.lineSpacing) ); // ### TODO: rounding?
                props += "fo:line-height=\"";
                props += height;
                props += "pt\" ";
                styleKey += height;
                styleKey += 'F';
                break;
            }
        case LayoutData::LS_ATLEAST:
            {
                // We have a at-least line height (in points)
                const QString height ( QString::number(layout.lineSpacing) ); // ### TODO: rounding?
                props += "style:line-height-at-least=\"";
                props += height;
                props += "pt\" ";
                styleKey += height;
                styleKey += 'A';
                break;
            }
        default:
            {
                kWarning(30518) << "Unsupported lineSpacingType: " << layout.lineSpacingType << " (Ignoring!)" << endl;
                break;
            }
        }
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
		TabulatorList::ConstIterator end(layout.tabulatorList.end());
        for (it=layout.tabulatorList.begin();it!=end;++it)
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
        kDebug(30518) << "Searching paragraph key: " << styleKey << endl;

        QString automaticStyle;

        if (it==m_mapParaStyleKeys.end())
        {
            // We have additional properties, so we need an automatic style for the paragraph
            automaticStyle = makeAutomaticStyleName("P", m_automaticParagraphStyleNumber);
            kDebug(30518) << "Creating automatic paragraph style: " << automaticStyle << " key: " << styleKey << endl;
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
            kDebug(30518) << "Using automatic paragraph style: " << automaticStyle << " key: " << styleKey << endl;
        }

        actualStyle=automaticStyle;
    }

    if (!actualStyle.isEmpty())
    {
        *m_streamOut << "text:style-name=\"" << escapeOOText(actualStyle) << "\" ";
    }
    else
    {   // SHould not happen
        kWarning(30518) << "No style for a paragraph!" << endl;
    }

    *m_streamOut << ">";

    processParagraphData(paraText, layout.formatData.text, paraFormatDataList);

    if (header)
        *m_streamOut << "</text:h>\n";
    else
        *m_streamOut << "</text:p>\n";

    return true;
}

bool OOWriterWorker::doOpenStyles(void)
{
    m_styles += " <office:styles>\n";
    m_styles += "  <style:style style:name=\"Graphics\" style:family=\"graphics\">\n"; // ### TODO: what if Graphics is a normal style
    m_styles += "   <style:properties text:anchor-type=\"paragraph\" style:wrap=\"none\"/>\n";
    m_styles += "  </style:style>\n";
    m_styles += "  <style:style style:name=\"Frame\" style:family=\"graphics\">\n"; // ### TODO: what if Frame is a normal style
    m_styles += "   <style:properties text:anchor-type=\"paragraph\" style:wrap=\"none\"/>\n";
    m_styles += "  </style:style>\n";
    return true;
}

bool OOWriterWorker::doFullDefineStyle(LayoutData& layout)
{
    //Register style in the style map
    m_styleMap[layout.styleName]=layout;

    m_styles += "  <style:style";

    m_styles += " style:name=\"" + escapeOOText( layout.styleName ) + "\"";
    m_styles += " style:next-style-name=\"" + escapeOOText( layout.styleFollowing ) + "\"";
    m_styles += " style:family=\"paragraph\" style:class=\"text\"";
    m_styles += ">\n";
    m_styles += "   <style:properties ";

    QString debugKey; // Not needed
    m_styles += layoutToParagraphStyle(layout,layout,true,debugKey);
    kDebug(30518) << "Defining style: " << debugKey << endl;

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
    if ( ( format < 0 ) // Be careful that 0 is ISO A3
        || ( width < 1.0 )
        || ( height < 1.0 ) )
    {
        kWarning(30518) << "Page size problem: format: " << format << " width: " << width << " height: " << height << endl;
        // Something is wrong with the page size
        KoFormat newFormat = KoFormat ( format );
        if ( ( format < 0 ) || ( format > PG_LAST_FORMAT ) )
        {
            // Bad or unknown format, so assume ISO A4
            newFormat = PG_DIN_A4;
        }
        m_paperWidth = KoPageFormat::width ( newFormat, KoOrientation( orientation ) ) * 72.0 / 25.4 ;
        m_paperHeight = KoPageFormat::height ( newFormat, KoOrientation( orientation ) ) * 72.0 / 25.4 ;
        m_paperFormat = newFormat;
    }
    else
    {
        m_paperFormat=format;
        m_paperWidth=width;
        m_paperHeight=height;
    }
    m_paperOrientation=orientation; // ### TODO: check if OOWriter needs the orignal size (without landscape) or the real size
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

bool OOWriterWorker::doFullPaperFormatOther ( const int columns, const double columnspacing, const int numPages )
{
    m_columns = columns;
    m_columnspacing = columnspacing;
    m_numPages = numPages;
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

bool OOWriterWorker::doDeclareNonInlinedFramesets( Q3ValueList<FrameAnchor>& pictureAnchors, Q3ValueList<FrameAnchor>& tableAnchors )
{
    m_nonInlinedPictureAnchors = pictureAnchors;
    m_nonInlinedTableAnchors = tableAnchors;
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

    kWarning(30518) << "Could not make an unique style name: " << str2 << endl;
    return str2; // Still return, as we have nothing better
}

