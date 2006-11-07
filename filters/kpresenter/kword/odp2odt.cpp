#include "odp2odt.h"

#include <kgenericfactory.h>
#include <KoStoreDevice.h>
#include <KoFilterChain.h>
#include <KoGlobal.h>
#include <KoXmlNS.h>
#include <KoDocument.h>
#include <KoDom.h>
#include <klocale.h>
#include <kdebug.h>

typedef KGenericFactory<Odp2Odt> Odp2OdtFactory;
K_EXPORT_COMPONENT_FACTORY( libkodp2odt, Odp2OdtFactory( "kofficefilters" ) )

Odp2Odt::Odp2Odt(QObject *parent,const QStringList&) :
    KoFilter(parent)
{
}

// This filter can act as an import filter for KWord and as an export
// filter for KPresenter (isn't our architecture really nice ? :)
// This is why we use the file-to-file method, not a QDomDoc one.
KoFilter::ConversionStatus Odp2Odt::convert( const QByteArray& from, const QByteArray& to )
{
    if ( to != "application/vnd.oasis.opendocument.text"
         || from != "application/vnd.oasis.opendocument.presentation" )
        return KoFilter::NotImplemented;

    KoStoreDevice* inpdev = m_chain->storageFile( "content.xml", KoStore::Read );
    if ( !inpdev )
    {
        kError(31000) << "Unable to open input stream" << endl;
        return KoFilter::StorageCreationError;
    }

    // Parse presentation content.xml
    QXmlInputSource source( inpdev );
    QXmlSimpleReader reader;
    KoDocument::setupXmlReader( reader, true /*namespaceProcessing*/ );
    QString errorMsg;
    int errorLine, errorColumn;
    bool ok = doc.setContent( &source, &reader, &errorMsg, &errorLine, &errorColumn );
    if ( !ok )
    {
        kError(31000) << "Parsing error! Aborting!" << endl
                      << " In line: " << errorLine << ", column: " << errorColumn << endl
                      << " Error message: " << errorMsg << endl;
        return KoFilter::ParsingError;
    }

    QDomElement docElem = doc.documentElement();
    QDomElement realBody( KoDom::namedItemNS( docElem, KoXmlNS::office, "body" ) );
    if ( realBody.isNull() )
    {
        kError(31000) << "No office:body found!" << endl;
        //setErrorMessage( i18n( "Invalid OASIS OpenDocument file. No office:body tag found." ) );
        return KoFilter::WrongFormat;
    }

    QDomElement body = KoDom::namedItemNS( realBody, KoXmlNS::office, "presentation" );
    if ( body.isNull() )
    {
        kError(32001) << "No office:text found!" << endl;
        return KoFilter::WrongFormat;
    }

    // Transformations start here:

    fixBodyTagName( body );

    processPages( body );

    copyFile( "styles.xml" );
    copyFile( "settings.xml" );
    copyFile( "meta.xml" );
    // TODO generate the manifest along the way using KoOasisStore

    // Write output file

    KoStoreDevice* out = m_chain->storageFile( "content.xml", KoStore::Write );
    if(!out) {
        kError(30502) << "Unable to open output file!" << endl;
        return KoFilter::StorageCreationError;
    }
    const QByteArray cstring = doc.toByteArray(); // utf-8 already
    //kDebug() << k_funcinfo << QString::fromUtf8( cstring ) << endl;
    const int len = cstring.length();
    const int written = out->write( cstring.constData(), len );
    if ( written != len )
        kError(30502) << "Wrote only " << written << " out of " << len << endl;
    out->close();

    return KoFilter::OK;
}

void Odp2Odt::fixBodyTagName( QDomElement& body )
{
    body.setTagName( "text" ); // the prefix is kept, apparently
}

void Odp2Odt::processPages( QDomElement& body )
{
    // For each page
    QDomElement pageElem;
    forEachElement( pageElem, body ) {
        if ( pageElem.localName() == "page" && pageElem.namespaceURI() == KoXmlNS::draw ) {
            // For each frame
            QDomElement frame;
            forEachElement( frame, pageElem ) {
                // Only intested in text boxes. Images are often used for background effects...
                QDomElement textBox = KoDom::namedItemNS( frame, KoXmlNS::draw, "text-box" );
                if ( textBox.isNull() )
                    continue;
                // Get any text:p or text:numbered-paragraph or list etc.
                QList<QDomElement> textNodes;
                QDomElement textElem;
                forEachElement( textElem, textBox ) {
                    textNodes.append( textElem );
                }
                // Now reparent them up (can't be done while iterating)
                foreach( QDomElement e, textNodes ) {
                    body.appendChild( e );
                }
            }
        }
    }
    // Get rid of the pages
    QDomNode n = body.firstChild();
    for ( ; !n.isNull(); ) {
        QDomNode next = n.nextSibling();
        if ( n.isElement() && n.localName() == "page" && n.namespaceURI() == KoXmlNS::draw ) {
            body.removeChild( n );
        }
        n = next;
    }
    // Get rid of presentation:settings
    n = KoDom::namedItemNS( body, KoXmlNS::presentation, "settings" );
    body.removeChild( n );
}

void Odp2Odt::copyFile( const QString& fileName )
{
    KoStoreDevice* inpdev = m_chain->storageFile( fileName, KoStore::Read );
    if ( !inpdev )
    {
        kError(31000) << "Unable to open styles.xml" << endl;
        return;
    }

    const QByteArray data = inpdev->readAll();

    KoStoreDevice* out = m_chain->storageFile( fileName, KoStore::Write );
    if(!out) {
        kError(30502) << "Unable to open output file!" << endl;
        return;
    }

    out->write( data );
}

#include "odp2odt.moc"
