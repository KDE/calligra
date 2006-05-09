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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <qxml.h>
#include <qdom.h>
//Added by qt3to4:
#include <Q3CString>

#include <kdebug.h>
#include <kgenericfactory.h>
#include <kimageio.h>
#include <ktempfile.h>

#include <KoFilterChain.h>
#include <KoStoreDevice.h>

#include "kword13parser.h"
#include "kword13document.h"
#include "kword13oasisgenerator.h"
#include "kword13postparsing.h"
#include "kword13import.h"

typedef KGenericFactory<KWord13Import> KWord13ImportFactory;
K_EXPORT_COMPONENT_FACTORY( libkwordkword1dot3import, KWord13ImportFactory( "kofficefilters" ) )


KWord13Import::KWord13Import(QObject* parent, const QStringList &)
     : KoFilter(parent)
{
}

bool KWord13Import::parseInfo( QIODevice* io, KWord13Document& kwordDocument )
{
    kDebug(30520) << "Starting KWord13Import::parseInfo" << endl;
    QDomDocument doc;
    // Error variables for QDomDocument::setContent
    QString errorMsg;
    int errorLine, errorColumn;
    if ( ! doc.setContent( io, &errorMsg, &errorLine, &errorColumn ) )
    {
        kError(30520) << "Parsing error in documentinfo.xml! Aborting!" << endl
            << " In line: " << errorLine << ", column: " << errorColumn << endl
            << " Error message: " << errorMsg << endl;
        // ### TODO: user message
        return false;
    }
    QDomElement docElement( doc.documentElement() );
    // In documentinfo.xml, the text data is in the grand-children of the document element
    for ( QDomNode node = docElement.firstChild(); !node.isNull(); node = node.nextSibling() )
    {
        kDebug(30520) << "Child " << node.nodeName() << endl;
        if ( !node.isElement() )
            continue; // Comment, PI...
        const QString nodeName( node.nodeName() );
        for ( QDomNode node2 = node.firstChild(); !node2.isNull(); node2 = node2.nextSibling() )
        {
            kDebug(30520) << "Grand-child " << node2.nodeName() << endl;
            if ( !node2.isElement() )
                continue;
            const QString nodeName2 ( nodeName + ':' + node2.nodeName() );
            QDomElement element( node2.toElement() );
            kwordDocument.m_documentInfo[ nodeName2 ] = element.text();
        }
    }
    kDebug(30520) << "Quitting KWord13Import::parseInfo" << endl;
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
        kWarning(30520) << "Parse Error" << endl;
        return false;
    }
    return true;
}

bool KWord13Import::postParse( KoStore* store, KWord13Document& doc )
{
    KWord13PostParsing post;
    return post.postParse( store, doc );
}

KoFilter::ConversionStatus KWord13Import::convert( const QByteArray& from, const QByteArray& to )
{
    if ( to != "application/vnd.oasis.opendocument.text"
        || from != "application/x-kword" )
    {
        return KoFilter::NotImplemented;
    }

    // We need KimageIO's help in OOWriterWorker::convertUnknownImage


    KWord13Document kwordDocument;

    const QString fileName( m_chain->inputFile() );
    if ( fileName.isEmpty() )
    {
        kError(30520) << "No input file name!" << endl;
        return KoFilter::StupidError;
    }

    KoStore* store = KoStore::createStore( fileName, KoStore::Read );
    if ( store && store->hasFile( "maindoc.xml" ) )
    {
        kDebug(30520) << "Maindoc.xml found in KoStore!" << endl;

        // We do not really care about errors while reading/parsing documentinfo
        store->open( "documentinfo.xml" );
        KoStoreDevice ioInfo( store );
        ioInfo.open( QIODevice::ReadOnly );
        kDebug (30520) << "Processing document info... " <<  endl;
        if ( ! parseInfo ( &ioInfo, kwordDocument ) )
        {
            kWarning(30520) << "Parsing documentinfo.xml has failed. Ignoring!" << endl;
        }
        ioInfo.close();
        store->close();

        // ### TODO: error return values
        if ( ! store->open( "maindoc.xml" ) )
        {
            kError(30520) << "Opening root has failed" << endl;
            delete store;
            return KoFilter::StupidError;
        }
        KoStoreDevice ioMain( store );
        ioMain.open( QIODevice::ReadOnly );
        kDebug (30520) << "Processing root... " <<  endl;
        if ( ! parseRoot ( &ioMain, kwordDocument ) )
        {
            kWarning(30520) << "Parsing maindoc.xml has failed! Aborting!" << endl;
            delete store;
            return KoFilter::StupidError;
        }
        ioMain.close();
        store->close();

        if ( store->open( "preview.png" ) )
        {

            kDebug(30520) << "Preview found!" << endl;
            KoStoreDevice ioPreview( store );
            ioPreview.open( QIODevice::ReadOnly );
            const QByteArray image ( ioPreview.readAll() );
            if ( image.isNull() )
            {
                kWarning(30520) << "Loading of preview failed! Ignoring!" << endl;
            }
            else
            {
                kwordDocument.m_previewFile = new KTempFile( QString::null, ".png" );
                // ### TODO check KTempFile
                kwordDocument.m_previewFile->setAutoDelete( true );
                QFile file( kwordDocument.m_previewFile->name() );
                // ### TODO: check if file is correctly written
                file.open( QIODevice::WriteOnly );
                file.write( image );
                file.close();
            }
            ioPreview.close();
            store->close();
        }
        else
        {
            kDebug(30520) << "No preview found!" << endl;
        }
    }
    else
    {
        kWarning(30520) << "Opening store has failed. Trying raw XML file!" << endl;
        // Be sure to undefine store
        delete store;
        store = 0;

        QFile file( fileName );
        file.open( QIODevice::ReadOnly );
        if ( ! parseRoot( &file, kwordDocument ) )
        {
            kError(30520) << "Could not process document! Aborting!" << endl;
            file.close();
            return KoFilter::StupidError;
        }
        file.close();
    }

    if ( ! postParse( store, kwordDocument ) )
    {
        kError(30520) << "Error during post-parsing! Aborting!" << endl;
        return  KoFilter::StupidError;
    }

    // We have finished with the input store/file, so close the store (already done for a raw XML file)
    kDebug(30520) << "Deleting input store..." << endl;
    delete store;
    store = 0;
    kDebug(30520) << "Input store deleted!" << endl;

    KWord13OasisGenerator generator;

    kDebug(30520) << __FILE__ << ":" << __LINE__ << endl;

    if ( ! generator.prepare( kwordDocument ) )
    {
        kError(30520) << "Could not prepare the OASIS document! Aborting!" << endl;
        return KoFilter::StupidError;
    }

    const QString filenameOut ( m_chain->outputFile() );

    if ( filenameOut.isEmpty() )
    {
        kError(30520) << "Empty file name for saving as OASIS! Aborting!" << endl;
        return KoFilter::StupidError;
    }

    if ( ! generator.generate( filenameOut, kwordDocument ) )
    {
        kError(30520) << "Could not save as OASIS! Aborting!" << endl;
        return KoFilter::StupidError;
    }

    kDebug(30520) << "Filter has finished!" << endl;

    return KoFilter::OK;
}

#include "kword13import.moc"
