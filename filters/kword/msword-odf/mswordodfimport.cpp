/* This file is part of the KOffice project
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2002 David Faure <faure@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License version 2 as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "mswordodfimport.h"

#include <qdom.h>
#include <qfontinfo.h>
#include <QFile>
#include <QString>
//Added by qt3to4:
#include <QByteArray>

#include <kdebug.h>
#include <kgenericfactory.h>

#include <KoFilterChain.h>
#include <KoOdfWriteStore.h>
#include <KoStoreDevice.h>
//#include <KoXmlWriter.h>

#include <document.h>

typedef KGenericFactory<MSWordOdfImport> MSWordOdfImportFactory;
K_EXPORT_COMPONENT_FACTORY( libmswordodf_import, MSWordOdfImportFactory( "kofficefilters" ) )

class MSWordOdfImport::Private
{
public:
    QString inputFile;
    QString outputFile;

    Document* document;

    //void prepareDocument( QDomDocument& mainDocument, QDomElement& framesetsElem );
    bool createStyles(KoOdfWriteStore* store);
    bool createManifest(KoOdfWriteStore* store);
    //bool createContent( KoXmlWriter* contentWriter, KoXmlWriter* bodyWriter );
};

bool MSWordOdfImport::Private::createStyles(KoOdfWriteStore* store)
{
    if ( !store->store()->open( "styles.xml" ) )
	return false;
    KoStoreDevice dev( store->store() );
    KoXmlWriter* stylesWriter = new KoXmlWriter( &dev );

    delete stylesWriter;
    return store->store()->close();
}

bool MSWordOdfImport::Private::createManifest(KoOdfWriteStore* store)
{
    KoXmlWriter* manifestWriter = store->manifestWriter( "application/vnd.oasis.opendocument.text" );
    //manifestWriter->addManifestEntry( "styles.xml", "text/xml" );
    manifestWriter->addManifestEntry( "content.xml", "text/xml" );

    return store->closeManifestWriter();
}


MSWordOdfImport::MSWordOdfImport( QObject *parent, const QStringList& ) : KoFilter(parent)
{
    d = new Private;
}

MSWordOdfImport::~MSWordOdfImport()
{
    delete d;
}

KoFilter::ConversionStatus MSWordOdfImport::convert( const QByteArray& from, const QByteArray& to )
{
    // check for proper conversion
    if ( to != "application/vnd.oasis.opendocument.text" || from != "application/msword" )
        return KoFilter::NotImplemented;

    kDebug(30513) <<"######################## MSWordImport::convert ########################";

    d->inputFile = m_chain->inputFile();
    d->outputFile = m_chain->outputFile();
    
    //create output files
    KoStore* storeout;
    storeout = KoStore::createStore( d->outputFile, KoStore::Write, 
	    "application/vnd.oasis.opendocument.text", KoStore::Zip );
    if ( !storeout ) {
        kWarning() << "Unable to open output file!" << endl;
        return KoFilter::FileNotFound;
    }
    storeout->disallowNameExpansion();
    kDebug(30513) <<"created storeout.";
    KoOdfWriteStore oasisStore( storeout );

    kDebug(30513) <<"created oasisStore.";

    //create the styles file
    if ( !d->createStyles( &oasisStore ) )
    {
	kWarning() << "Couldn't open the file 'styles.xml'.";
	delete d->document;
	delete storeout;
	return KoFilter::CreationError;
    }

    kDebug(30513) <<"created styles.";
    //create the manifest file
    if ( !d->createManifest( &oasisStore ) )
    {
	kWarning() << "Couldn't open the file 'META-INF/manifest.xml'.";
	delete d->document;
	delete storeout;
	return KoFilter::CreationError;
    }

    kDebug(30513) <<"created manifest";

    KoXmlWriter* contentWriter = oasisStore.contentWriter();
    KoXmlWriter* bodyWriter = oasisStore.bodyWriter();
    if ( !bodyWriter || !contentWriter )
	return KoFilter::CreationError; //not sure if this is the right error to return

    kDebug(30513) <<"created contentWriter and bodyWriter.";
    //shouldn't need this stuff...
    //QDomDocument mainDocument;
    //QDomElement framesetsElem;
    //prepareDocument( mainDocument, framesetsElem );

    //QDomDocument documentInfo;
    //documentInfo.appendChild (documentInfo.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );

    d->document = new Document( QFile::encodeName( d->inputFile ).data(), /*mainDocument, documentInfo, framesetsElem,*/ m_chain, bodyWriter );
    //Document document( QFile::encodeName( m_chain->inputFile() ).data(), mainDocument, documentInfo, framesetsElem, m_chain );
    
    //check that we can parse the document?
    if ( !d->document->hasParser() )
        return KoFilter::WrongFormat;


    //this is where the action happens
    if ( !createContent( contentWriter, bodyWriter ) )
    {
	kWarning() << "Couldn't create content.";
	delete d->document;
	delete storeout;
	return KoFilter::CreationError;
    }
    
    kDebug(30513) <<"created content.";
    if ( !oasisStore.closeContentWriter() )
    {
	kWarning() << "Error closing content.";
	delete d->document;
	delete storeout;
	return KoFilter::CreationError;
    }
    
    //KoStoreDevice* out = m_chain->storageFile( "root", KoStore::Write );
    
    //QByteArray cstr = mainDocument.toByteArray();
    // WARNING: we cannot use KoStore::write(const QByteArray&) because it gives an extra NULL character at the end.
    //out->write( cstr, cstr.length() );
    //out->close();

    //out = m_chain->storageFile( "documentinfo.xml", KoStore::Write );
    //if ( !out ) {
    //	return KoFilter::StorageCreationError;
    //}

    //cstr = documentInfo.toByteArray();
    //out->write( cstr, cstr.length() );
    //out->close();
    
    //done, so cleanup now
    delete d->document;
    delete storeout;
    d->inputFile.clear();
    d->outputFile.clear();
    d->document = 0;

    kDebug(30513) <<"######################## MSWordImport::convert done ####################";
    return KoFilter::OK;
}

bool MSWordOdfImport::createContent( KoXmlWriter* contentWriter, KoXmlWriter* bodyWriter )
{
    
    // FIXME this is dummy and hardcoded, replace with real font names
    contentWriter->startElement( "office:font-face-decls" );
    contentWriter->startElement( "style:font-face" );
    contentWriter->addAttribute( "style:name", "Arial" );
    contentWriter->addAttribute( "svg:font-family", "Arial" );
    contentWriter->endElement(); // style:font-face
    contentWriter->startElement( "style:font-face" );
    contentWriter->addAttribute( "style:name", "Times New Roman" );
    contentWriter->addAttribute( "svg:font-family", "&apos;Times New Roman&apos;" );
    contentWriter->endElement(); // style:font-face
    contentWriter->endElement(); // office:font-face-decls

    //just some constants for now
    bodyWriter->startElement( "office:body" );
    //text & content
    bodyWriter->startElement( "office:text" );
    //bodyWriter->startElement( "text:sequence-decls" );
    //bodyWriter->endElement(); //text:sequence-decls
    //bodyWriter->startElement( "text:p" );
    //bodyWriter->addTextNode( "Hello World" );
    //bodyWriter->endElement(); //text:p
    
    //actual parsing & action
    if ( !d->document->parse() ) //parse file into the queues?
        return KoFilter::ParsingError;
    d->document->processSubDocQueue(); //process the queues we've created?
    d->document->finishDocument(); //process footnotes, pictures, ...
    if ( !d->document->bodyFound() )
        return KoFilter::WrongFormat;
    
    //close & cleanup
    bodyWriter->endElement(); //office:text
    bodyWriter->endElement(); //office:body

    //return store->closeContentWriter(); //closes bodyWriter, too
    return true;
}
/*void MSWordImport::prepareDocument( QDomDocument& mainDocument, QDomElement& framesetsElem )
{
    mainDocument.appendChild( mainDocument.createProcessingInstruction( "xml","version=\"1.0\" encoding=\"UTF-8\"" ) );

    QDomElement elementDoc;
    elementDoc=mainDocument.createElement("DOC");
    elementDoc.setAttribute("editor","KWord's MS Word Import Filter");
    elementDoc.setAttribute("mime","application/x-kword");
    elementDoc.setAttribute("syntaxVersion",2);
    mainDocument.appendChild(elementDoc);

    framesetsElem=mainDocument.createElement("FRAMESETS");
    mainDocument.documentElement().appendChild(framesetsElem);
}*/

#include <mswordodfimport.moc>
