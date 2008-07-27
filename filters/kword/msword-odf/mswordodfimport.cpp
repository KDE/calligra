/* This file is part of the KOffice project
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2002 David Faure <faure@kde.org>
   Copyright (C) 2008 Benjamin Cail <cricketc@gmail.com>

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
};

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

    kDebug(30513) <<"######################## MSWordOdfImport::convert ########################";

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

    //create KoGenStyles for writing styles while we're parsing
    KoGenStyles* mainStyles = new KoGenStyles();

    //create a writer for meta.xml
    QBuffer buf;
    buf.open(QIODevice::WriteOnly);
    KoXmlWriter metaWriter(&buf);

    //open contentWriter & bodyWriter
    KoXmlWriter* contentWriter = oasisStore.contentWriter();
    KoXmlWriter* bodyWriter = oasisStore.bodyWriter();
    if ( !bodyWriter || !contentWriter )
    {
	delete d->document;
	delete storeout;
	return KoFilter::CreationError; //not sure if this is the right error to return
    }

    kDebug(30513) <<"created contentWriter and bodyWriter.";

    //open tags in bodyWriter
    bodyWriter->startElement("office:body");
    bodyWriter->startElement("office:text");

    //create our document object, writing to the temporary buffers
    d->document = new Document(QFile::encodeName( d->inputFile ).data(), m_chain, bodyWriter, mainStyles, &metaWriter);
    
    //check that we can parse the document?
    if ( !d->document->hasParser() )
    {
	delete d->document;
	delete storeout;
        return KoFilter::WrongFormat;
    }

    //actual parsing & action
    if ( !d->document->parse() ) //parse file into the queues?
    {
	delete d->document;
	delete storeout;
	return KoFilter::CreationError;
    }
    d->document->processSubDocQueue(); //process the queues we've created?
    d->document->finishDocument(); //process footnotes, pictures, ...
    if ( !d->document->bodyFound() )
    {
	delete d->document;
	delete storeout;
	return KoFilter::WrongFormat;
    }
    
    kDebug(30513) <<"finished parsing.";

    //save the office:automatic-styles & and fonts in content.xml
    mainStyles->saveOdfFontFaceDecls(contentWriter);
    mainStyles->saveOdfAutomaticStyles(contentWriter, false);

    //close tags in bodyWriter
    bodyWriter->endElement();//office:text
    bodyWriter->endElement();//office:body

    //now close content & body writers
    if ( !oasisStore.closeContentWriter() )
    {
	kWarning() << "Error closing content.";
	delete d->document;
	delete storeout;
	return KoFilter::CreationError;
    }
    
    kDebug(30513) <<"closed content & body writers.";

    //create the manifest file
    KoXmlWriter* manifestWriter = oasisStore.manifestWriter( "application/vnd.oasis.opendocument.text" );
    //create the styles.xml file
    mainStyles->saveOdfStylesDotXml( storeout, manifestWriter );
    manifestWriter->addManifestEntry( "content.xml", "text/xml" );

    kDebug(30513) <<"created manifest and styles.xml";

    //create meta.xml
    if(!storeout->open("meta.xml")) {
	delete d->document;
	delete storeout;
	delete mainStyles;
	return KoFilter::CreationError;
    }
    KoStoreDevice metaDev(storeout);
    KoXmlWriter* meta = KoOdfWriteStore::createOasisXmlWriter(&metaDev, "office:document-meta");
    meta->startElement("office:meta");
    meta->addCompleteElement(&buf);
    meta->endElement(); //office:meta
    meta->endElement(); //office:document-meta
    meta->endDocument();
    if(!storeout->close()) {
	delete d->document;
	delete storeout;
	delete mainStyles;
	return KoFilter::CreationError;
    }
    manifestWriter->addManifestEntry("meta.xml", "text/xml");
    oasisStore.closeManifestWriter();

    //done, so cleanup now
    delete d->document;
    delete storeout;
    delete mainStyles;
    d->inputFile.clear();
    d->outputFile.clear();
    d->document = 0;

    kDebug(30513) <<"######################## MSWordOdfImport::convert done ####################";
    return KoFilter::OK;
}

#include <mswordodfimport.moc>
