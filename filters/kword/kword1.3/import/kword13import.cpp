//

/* This file is part of the KDE project
   Copyright (C) 2004 Nicolas GOUTTE <goutte@kde.org>

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

#include <qxml.h>
#include <qdom.h>

#include <kdebug.h>
#include <kgenericfactory.h>
#include <kimageio.h>
#include <ktempfile.h>

#include <koFilterChain.h>
#include <koStoreDevice.h>

#include "kword13parser.h"
#include "kword13document.h"
#include "kword13oasisgenerator.h"
#include "kword13import.h"

typedef KGenericFactory<KWord13Import, KoFilter> KWord13ImportFactory;
K_EXPORT_COMPONENT_FACTORY( libkwordkword1dot3import, KWord13ImportFactory( "kofficefilters" ) )


KWord13Import::KWord13Import(KoFilter */*parent*/, const char */*name*/, const QStringList &)
     : KoFilter()
{
}

bool KWord13Import::parseInfo( QIODevice* io, KWord13Document& kwordDocument )
{
    kdDebug(30520) << "Starting KWord13Import::parseInfo" << endl;
    QDomDocument doc;
    // Error variables for QDomDocument::setContent
    QString errorMsg;
    int errorLine, errorColumn;
    if ( ! doc.setContent( io, &errorMsg, &errorLine, &errorColumn ) )
    {
        kdError(30520) << "Parsing error in documentinfo.xml! Aborting!" << endl
            << " In line: " << errorLine << ", column: " << errorColumn << endl
            << " Error message: " << errorMsg << endl;
        // ### TODO: user message
        return false;
    }
    QDomElement docElement( doc.documentElement() );
    // In documentinfo.xml, the text data is in the grand-children of the document element
    for ( QDomNode node = docElement.firstChild(); !node.isNull(); node = node.nextSibling() )
    {
        kdDebug(30520) << "Child " << node.nodeName() << endl;
        if ( !node.isElement() )
            continue; // Comment, PI...
        const QString nodeName( node.nodeName() );
        for ( QDomNode node2 = node.firstChild(); !node2.isNull(); node2 = node2.nextSibling() )
        {
            kdDebug(30520) << "Grand-child " << node2.nodeName() << endl;
            if ( !node2.isElement() )
                continue;
            const QString nodeName2 ( nodeName + ':' + node2.nodeName() );
            QDomElement element( node2.toElement() );
            kwordDocument.m_documentInfo[ nodeName2 ] = element.text();
        }
    }
    kdDebug(30520) << "Quitting KWord13Import::parseInfo" << endl;
    return true;
}

bool KWord13Import::parseRoot( QIODevice* io, KWord13Document& kwordDocument )
{
    KWord13Parser handler( &kwordDocument );

    QXmlSimpleReader reader;
    reader.setContentHandler( &handler );
    reader.setErrorHandler( &handler );

    QXmlInputSource source( io ); // Read the file
    
    if (!reader.parse( source ))
    {
        kdWarning(30520) << "Parse Error" << endl;
        return false;
    }
    return true;
}

KoFilter::ConversionStatus KWord13Import::convert( const QCString& from, const QCString& to )
{
    if ( to != "application/vnd.sun.xml.writer"  // ### TODO: OASIS
        || from != "application/x-kword" )
    {
        return KoFilter::NotImplemented;
    }

    // We need KimageIO's help in OOWriterWorker::convertUnknownImage
    KImageIO::registerFormats();

    KWord13Document kwordDocument;
    
    KoStoreDevice* subFile;

    subFile = m_chain->storageFile( "documentinfo.xml", KoStore::Read );
    kdDebug (30520) << "Processing documentinfo... " << ((void*) subFile) << endl;
    if ( ! parseInfo ( subFile, kwordDocument ) )
    {
        kdWarning(30520) << "Opening documentinfo.xml has failed. Ignoring!" << endl;
    }

    subFile = m_chain->storageFile( "root", KoStore::Read );
    kdDebug (30520) << "Processing root... " << ((void*) subFile) << endl;
    if ( parseRoot ( subFile, kwordDocument ) )
    {
        subFile = m_chain->storageFile( "preview.png", KoStore::Read );
        if ( subFile )
        {
            kdDebug(30520) << "Preview found!" << endl;
            const QByteArray image ( subFile->readAll() );
            if ( image.isNull() )
            {
                kdWarning(30520) << "Loading of preview failed! Ignoring!" << endl;
            }
            else
            {
                kwordDocument.m_previewFile = new KTempFile( QString::null, ".png" );
                // ### TODO check KTempFile
                kwordDocument.m_previewFile->setAutoDelete( true );
                QFile file( kwordDocument.m_previewFile->name() );
                // ### TODO: check if file is correctly written
                file.open( IO_WriteOnly );
                file.writeBlock( image );
                file.close();
            }
        }
        else
        {
            kdDebug(30520) << "No preview found!" << endl;
        }
    }
    else
    {
        kdWarning(30520) << "Opening root has failed. Trying raw XML file!" << endl;

        const QString filename( m_chain->inputFile() );
        if ( filename.isEmpty() )
        {
            kdError(30520) << "Could not open document as raw XML! Aborting!" << endl;
            return KoFilter::StupidError;
        }
        else
        {
            QFile file( filename );
            file.open( IO_ReadOnly );
            if ( ! parseRoot( &file, kwordDocument ) )
            {
                kdError(30520) << "Could not process document! Aborting!" << endl;
                file.close();
                return KoFilter::StupidError;
            }
            file.close();
        }
    }

    // ### TODO: do post-parsing data processing (table groups, load pictures...)
    
    
    KWord13OasisGenerator generator;
        
    if ( ! generator.prepare( kwordDocument ) )
    {
        kdError(30520) << "Could not prepare the OASIS document! Aborting!" << endl;
        return KoFilter::StupidError;
    }
    
    const QString filenameOut ( m_chain->outputFile() );
    
    if ( filenameOut.isEmpty() )
    {
        kdError(30520) << "Empty file name for saving as OASIS! Aborting!" << endl;
        return KoFilter::StupidError;
    }
    
    if ( ! generator.generate( filenameOut, kwordDocument ) )
    {
        kdError(30520) << "Could not save as OASIS! Aborting!" << endl;
        return KoFilter::StupidError;
    }
    
    kdDebug(30520) << "Filter has finished!" << endl;
    
    return KoFilter::OK;
}

#include "kword13import.moc"
