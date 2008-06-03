/* This file is part of the KOffice project
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2002 David Faure <faure@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   version 2 of the License, or (at your option) version 3 or,
   at the discretion of KDE e.V (which shall act as a proxy as in
   section 14 of the GPLv3), any later version..

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
#include <QBuffer>
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
    bool initStyles( KoXmlWriter* stylesWriter );
    bool createManifest( KoOdfWriteStore* store );
    //bool createContent( KoXmlWriter* contentWriter, KoXmlWriter* bodyWriter );
};

bool MSWordOdfImport::Private::initStyles(KoXmlWriter* stylesWriter )
{
    //write some default stuff to styles.xml
    stylesWriter->startDocument( "office:document-styles" );
    stylesWriter->startElement( "office:document-styles" );
    stylesWriter->addAttribute( "xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0" );
    stylesWriter->addAttribute( "xmlns:style", "urn:oasis:names:tc:opendocument:xmlns:style:1.0" );
    stylesWriter->addAttribute( "xmlns:text", "urn:oasis:names:tc:opendocument:xmlns:text:1.0" );
    stylesWriter->addAttribute( "xmlns:table", "urn:oasis:names:tc:opendocument:xmlns:table:1.0" );
    stylesWriter->addAttribute( "xmlns:draw", "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0" );
    stylesWriter->addAttribute( "xmlns:fo", "urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0" );
    stylesWriter->addAttribute( "xmlns:svg","urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0" );
    stylesWriter->addAttribute( "office:version","1.1" );
    stylesWriter->startElement( "office:font-face-decls" );
    stylesWriter->endElement(); //office:font-face-decls
    stylesWriter->startElement( "office:automatic-styles" );
    stylesWriter->endElement(); //office:automatic-styles
    //need to close office:document-styles & endDocument() below!
}

bool MSWordOdfImport::Private::createManifest(KoOdfWriteStore* store)
{
    KoXmlWriter* manifestWriter = store->manifestWriter( "application/vnd.oasis.opendocument.text" );
    manifestWriter->addManifestEntry( "styles.xml", "text/xml" );
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
    //if ( !d->createStyles( &oasisStore ) )
    //{
	//kWarning() << "Couldn't open the file 'styles.xml'.";
	//delete d->document;
	//delete storeout;
	//return KoFilter::CreationError;
    //}

    //kDebug(30513) <<"created styles.";
    
    //create the manifest file
    if ( !d->createManifest( &oasisStore ) )
    {
	kWarning() << "Couldn't open the file 'META-INF/manifest.xml'.";
	delete d->document;
	delete storeout;
	return KoFilter::CreationError;
    }

    kDebug(30513) <<"created manifest";

    //create temporary KoXmlWriter*'s to write to while we're parsing
    //then we'll dump those into the real files
    QBuffer tmpStylesDotXmlBuffer, tmpBodyBuffer, tmpContentBuffer;
    KoXmlWriter* tmpStylesDotXmlWriter = new KoXmlWriter( &tmpStylesDotXmlBuffer );
    KoXmlWriter* tmpBodyWriter = new KoXmlWriter( &tmpBodyBuffer );
    KoXmlWriter* tmpContentWriter = new KoXmlWriter( &tmpContentBuffer );

    //create our document object, writing to the temporary buffers
    d->document = new Document( QFile::encodeName( d->inputFile ).data(), m_chain, tmpContentWriter, tmpBodyWriter, tmpStylesDotXmlWriter );
    
    //check that we can parse the document?
    if ( !d->document->hasParser() )
        return KoFilter::WrongFormat;


    //this is where the action happens
    if ( !createContent( tmpContentWriter, tmpBodyWriter ) )
    {
	kWarning() << "Couldn't create content.";
	delete tmpStylesDotXmlWriter;
	delete tmpBodyWriter;
	delete tmpContentWriter;
	delete d->document;
	delete storeout;
	return KoFilter::CreationError;
    }
    
    kDebug(30513) <<"created content.";

    //open stylesWriter
    if ( !oasisStore.store()->open( "styles.xml" ) )
	return KoFilter::CreationError;
    KoStoreDevice dev( oasisStore.store() );
    KoXmlWriter* stylesWriter = new KoXmlWriter( &dev );

    kDebug(30513) <<"created stylesWriter.";

    d->initStyles( stylesWriter );
    //write information from tmp writer to styles.xml
    stylesWriter->addCompleteElement( tmpStylesDotXmlWriter->device() );
    stylesWriter->endElement(); //office:document-styles
    stylesWriter->endDocument();

    //clean up stylesWriter
    delete stylesWriter;
    delete tmpStylesDotXmlWriter;
    if ( !oasisStore.store()->close() )
    {
	kWarning() << "Error closing styles.xml.";
	delete tmpBodyWriter;
	delete tmpContentWriter;
	delete d->document;
	delete storeout;
	return KoFilter::CreationError;
    }

    //open contentWriter & bodyWriter
    KoXmlWriter* contentWriter = oasisStore.contentWriter();
    KoXmlWriter* bodyWriter = oasisStore.bodyWriter();
    if ( !bodyWriter || !contentWriter )
    {
	delete tmpBodyWriter;
	delete tmpContentWriter;
	delete d->document;
	delete storeout;
	return KoFilter::CreationError; //not sure if this is the right error to return
    }

    kDebug(30513) <<"created contentWriter and bodyWriter.";

    //write information from tmp writers to content.xml
    contentWriter->addCompleteElement( tmpContentWriter->device() );
    bodyWriter->addCompleteElement( tmpBodyWriter->device() );

    //clean up contentWriter & bodyWriter
    delete tmpBodyWriter;
    delete tmpContentWriter;
    if ( !oasisStore.closeContentWriter() )
    {
	kWarning() << "Error closing content.";
	delete d->document;
	delete storeout;
	return KoFilter::CreationError;
    }
    
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
    contentWriter->addAttribute( "svg:font-family", "Times New Roman" );
    contentWriter->endElement(); // style:font-face
    contentWriter->endElement(); // office:font-face-decls
    contentWriter->startElement( "office:automatic-styles" ); //open automatic styles for writing during the parsing

    //just some constants for now
    bodyWriter->startElement( "office:body" );
    //text & content
    bodyWriter->startElement( "office:text" );
    bodyWriter->addAttribute( "text:use-soft-page-breaks", "true" );
    
    //actual parsing & action
    if ( !d->document->parse() ) //parse file into the queues?
	return false;
    d->document->processSubDocQueue(); //process the queues we've created?
    d->document->finishDocument(); //process footnotes, pictures, ...
    if ( !d->document->bodyFound() )
	return false;
    
    //close & cleanup
    contentWriter->endElement(); //office:automatic-styles
    bodyWriter->endElement(); //office:text
    bodyWriter->endElement(); //office:body

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
