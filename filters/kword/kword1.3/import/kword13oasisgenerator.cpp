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

#include <qstring.h>
#include <qtextstream.h>

#include <kdebug.h>
#include <kzip.h>

#include <kofficeversion.h>

#include "kword13document.h"

#include "kword13oasisgenerator.h"

KWord13OasisGenerator::KWord13OasisGenerator( void ) : m_kwordDocument( 0 ), m_zip( 0 ), m_streamOut( 0 )
{
}

KWord13OasisGenerator::~KWord13OasisGenerator( void )
{
}

bool KWord13OasisGenerator::prepare( KWord13Document& kwordDocument )
{
    // ### TODO
    return true;
}


QString KWord13OasisGenerator::escapeOOText(const QString& strText) const
{
    // Escape quotes (needed in attributes)
    // Escape apostrophs (allowed by XML)

    QString strReturn;
    QChar ch;

    for (uint i=0; i<strText.length(); i++)
    {
        ch=strText[i];
        switch (ch.unicode())
        {
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
        default:
            {
                strReturn+=ch;
                break;
            }
        }
    }

    return strReturn;
}


QString KWord13OasisGenerator::escapeOOSpan(const QString& strText) const
// We need not only to escape the classical XML stuff but also to take care of spaces and tabs.
// Also we must take care about not falling into the rules in XML about white space between 2 opening tags or between 2 closing tags
{
    QString strReturn;
    QChar ch;
    int spaceNumber = 0; // How many spaces should be written
    uint spaceSequenceStart = 9999; // Where does the space sequence start (start value must be non-null)

    for (uint i=0; i<strText.length(); i++)
    {
        ch=strText[i];

        if (ch!=' ')
        {
            // The next character is not a space (anymore)
            if ( spaceNumber > 0 )
            {
                if ( spaceSequenceStart )
                {   // Generate a real space only if we are not at start

                    strReturn += ' ';
                    --spaceNumber;
                }
                if ( spaceNumber > 1 )
                {
                    strReturn += "<text:s text:c=\"";
                    strReturn += QString::number( spaceNumber );
                    strReturn += "\"/>";
                }
            }
            spaceNumber = 0;
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
                    spaceSequenceStart = i;
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
        // We do not care about writing a real space (also to avoid to have <tag> </tag>)
        strReturn+="<text:s text:c=\"";
        strReturn+=QString::number(spaceNumber);
        strReturn+="\"/>";
    }

    return strReturn;
}

bool KWord13OasisGenerator::zipPrepareWriting(const QString& name)
{
    if (!m_zip)
        return false;
    m_size=0;
    return m_zip->prepareWriting(name, QString::null, QString::null, 0);
}

bool KWord13OasisGenerator::zipDoneWriting(void)
{
    if (!m_zip)
        return false;
    return m_zip->doneWriting(m_size);
}

bool KWord13OasisGenerator::zipWriteData(const char* str)
{
    if (!m_zip)
        return false;
    const uint size=strlen(str);
    m_size+=size;
    return m_zip->writeData(str,size);
}

bool KWord13OasisGenerator::zipWriteData(const QByteArray& array)
{
    if (!m_zip)
        return false;
    const uint size=array.size();
    m_size+=size;
    return m_zip->writeData(array.data(),size);
}

bool KWord13OasisGenerator::zipWriteData(const QCString& cstr)
{
    if (!m_zip)
        return false;
    const uint size=cstr.length();
    m_size+=size;
    return m_zip->writeData(cstr.data(),size);
}

bool KWord13OasisGenerator::zipWriteData(const QString& str)
{
    return zipWriteData(str.utf8());
}

void KWord13OasisGenerator::writeStartOfFile(const QString& type)
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

void KWord13OasisGenerator::writeStylesXml( void )
{
    if (!m_zip && !m_kwordDocument)
        return;

    zipPrepareWriting("styles.xml");

    writeStartOfFile("styles");

    // ### TODO writeFontDeclaration();

    // ### TODO zipWriteData(m_styles);

    zipWriteData(" <office:automatic-styles>\n");
    zipWriteData("  <style:page-master style:name=\"pm1\">\n"); // ### TODO: verify if style name is unique

    zipWriteData( "   <style:properties" );
    zipWriteData( " style:page-usage=\"all\"" ); // ### TODO: check

    zipWriteData(" fo:page-width=\"");
    zipWriteData( m_kwordDocument->getProperty( "PAPER:width", "PAPER:ptWidth" ) );
    zipWriteData("pt\" fo:page-height=\"");
    zipWriteData( m_kwordDocument->getProperty( "PAPER:height", "PAPER:ptHeight" ) );
    zipWriteData("pt\" ");

    zipWriteData("style:print-orientation=\"");
    if ( m_kwordDocument->getProperty( "PAPER:orientation" ) == "1" )
    {
        zipWriteData("landscape");
    }
    else
    {
        zipWriteData("portrait");
    }
    
    const int firstPageNumber = m_kwordDocument->getProperty( "VARIABLESETTINGS:startingPageNumber" ).toInt();

    zipWriteData("\" fo:margin-top=\"");
    zipWriteData( m_kwordDocument->getProperty( "PAPERBORDERS:top", "PAPERBORDERS:ptTop" ) );
    zipWriteData("pt\" fo:margin-bottom=\"");
    zipWriteData( m_kwordDocument->getProperty( "PAPERBORDERS:bottom", "PAPERBORDERS:ptBottom" ) );
    zipWriteData("pt\" fo:margin-left=\"");
    zipWriteData( m_kwordDocument->getProperty( "PAPERBORDERS:left", "PAPERBORDERS:ptLeft" ) );
    zipWriteData("pt\" fo:margin-right=\"");
    zipWriteData( m_kwordDocument->getProperty( "PAPERBORDERS:right", "PAPERBORDERS:ptRight" ) );
    zipWriteData("pt\" style:first-page-number=\"");
    zipWriteData(QString::number( ( firstPageNumber > 1 ) ? firstPageNumber : 1 ) );
    zipWriteData( "\">\n" );

    const int columns = m_kwordDocument->getProperty( "PAPER:columns" ).toInt();
    if ( columns > 1 )
    {
        zipWriteData( "    <style:columns" );
        zipWriteData( " fo:column-count=\"" );
        zipWriteData( QString::number( columns ) );
        zipWriteData( "\" fo:column-gap=\"" );
        zipWriteData( m_kwordDocument->getProperty( "PAPER:columnspacing", "PAPER:ptColumnspc" ) );
        zipWriteData( "pt\">\n" );

        for (int i=0; i < columns; ++i)
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


void KWord13OasisGenerator::writeContentXml(void)
{
    if (!m_zip)
        return;

    kdDebug(30520) << "content.xml: preparing..." << endl;
    zipPrepareWriting("content.xml");

    kdDebug(30520) << "content.xml: start file..." << endl;
    writeStartOfFile("content");

    kdDebug(30520) << "content.xml: declare fonts..." << endl;
    // ### TODO writeFontDeclaration();

    kdDebug(30520) << "content.xml: writing automatical styles..." << endl;
    zipWriteData(" <office:automatic-styles>\n");

    zipWriteData(m_contentAutomaticStyles);
    m_contentAutomaticStyles = QString::null; // Release memory

    zipWriteData(" </office:automatic-styles>\n");

    kdDebug(30520) << "content.xml: writing body..." << endl;
    zipWriteData("<office:body>\n");
    zipWriteData(m_contentBody);
    m_contentBody.resize( 0 ); // Release memory
    zipWriteData("</office:body>\n");

    zipWriteData( "</office:document-content>\n" );

    kdDebug(30520) << "content.xml: closing file..." << endl;
    zipDoneWriting();
    kdDebug(30520) << "content.xml: done!" << endl;
}

void KWord13OasisGenerator::writeMetaXml(void)
{
    if ( !m_zip || !m_kwordDocument )
        return;

    zipPrepareWriting("meta.xml");

    writeStartOfFile("meta");

    zipWriteData(" <office:meta>\n");

    // Tell who we are in case that we have a bug in our filter output!
    zipWriteData( "  <meta:generator>KOffice " );
    zipWriteData( escapeOOText( KOFFICE_VERSION_STRING ) );
    zipWriteData( " / KWord's OOWriter Export Filter " );
    zipWriteData( QString( "$Revision$" ).remove( '$' ) );

    zipWriteData("</meta:generator>\n");

    // ### TODO
#if 0
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
#endif

    QDateTime dt;
    
    dt = m_kwordDocument->creationDate();
    if ( dt.isValid() )
    {
        zipWriteData("  <meta:creation-date>");
        zipWriteData( escapeOOText( dt.toString( Qt::ISODate) ) );
        zipWriteData("</meta:creation-date>\n");
    }

    dt = m_kwordDocument->modificationDate();
    if ( dt.isValid() )
    {
        zipWriteData("  <dc:date>");
        zipWriteData( escapeOOText( dt.toString( Qt::ISODate ) ) );
        zipWriteData("</dc:date>\n");
    }

    dt = m_kwordDocument->lastPrintingDate();
    if ( dt.isValid() )
    {
        zipWriteData("  <meta:print-date>");
        zipWriteData( escapeOOText( dt.toString( Qt::ISODate ) ) );
        zipWriteData("</meta:print-date>\n");
    }

    zipWriteData( "  <meta:document-statistic" );

    // KWord files coming from import filters mostly do not have no page count
    const int numPages = m_kwordDocument->getProperty( "PAPER:pages" ).toInt();
    if ( numPages > 0 )
    {
        zipWriteData( " meta:page-count=\"" );
        zipWriteData( QString::number ( numPages ) );
        zipWriteData( "\"" );
    }

    zipWriteData( " meta:image-count=\"" ); // This is not specified in the OO specification section 2.1.19, fixed in OASIS (### TODO)
#if 1
    zipWriteData( "0" ); // ### TODO
#else
    zipWriteData( QString::number ( m_pictureNumber ) );
#endif
    zipWriteData( "\"" );

    zipWriteData( " meta:table-count=\"" );
#if 1
    zipWriteData( "0" ); // ### TODO
#else
    zipWriteData( QString::number ( m_tableNumber ) );
#endif
    zipWriteData( "\"" );
    zipWriteData( "/>\n" ); // meta:document-statistic
    
    zipWriteData(" </office:meta>\n");
    zipWriteData("</office:document-meta>\n");

    zipDoneWriting();
}


bool KWord13OasisGenerator::generate ( const QString& fileName, KWord13Document& kwordDocument )
{
    m_streamOut = new QTextStream( m_contentBody, IO_WriteOnly );
    m_streamOut->setEncoding( QTextStream::UnicodeUTF8 );
    
    // ### TODO
    
    if ( m_kwordDocument && ( (void*) m_kwordDocument ) != ( (void*) &kwordDocument ) )
    {
        kdWarning(30520) << "KWord Document is different!" <<endl;
    }
    
    m_kwordDocument = &kwordDocument;
    
    m_zip = new KZip( fileName ); // How to check failure?

    if (!m_zip->open(IO_WriteOnly))
    {
        kdError(30520) << "Could not open ZIP file for writing! Aborting!" << endl;
        delete m_zip;
        m_zip=NULL;
        return false;
    }

    m_zip->setCompression( KZip::NoCompression );
    m_zip->setExtraField( KZip::NoExtraField );

    const QCString appId( "application/vnd.sun.xml.writer" );

    m_zip->writeFile( "mimetype", QString::null, QString::null, appId.length(), appId.data() );

    m_zip->setCompression( KZip::DeflateCompression );

    // ### TODO
    if (m_zip)
    {
        kdDebug(30520) << "Writing content..." << endl;
        writeContentXml();
        kdDebug(30520) << "Writing meta..." << endl;
    // ### TODO        writeMetaXml();
        kdDebug(30520) << "Writing styles..." << endl;
    // ### TODO        writeStylesXml();
        kdDebug(30520) << "Closing ZIP..." << endl;
        m_zip->close();
    }
    kdDebug(30520) << "Deleting ZIP..." << endl;
    delete m_zip;
    m_zip=NULL;
    
    return true;
}

