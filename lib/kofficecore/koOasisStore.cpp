#include "koOasisStore.h"

#include "koDocument.h"
#include <koStore.h>
#include <koStoreDevice.h>
#include <koxmlwriter.h>
#include <ktempfile.h>
#include <qfile.h>
#include <kdebug.h>
#include <qxml.h>
#include <klocale.h>

KoOasisStore::KoOasisStore( KoStore* store )
    : m_store( store ),
      m_storeDevice( 0 ), m_contentWriter( 0 ), m_bodyWriter( 0 ),
      m_contentTmpFile( 0 )
{
}

KoOasisStore::~KoOasisStore()
{
    // If both close methods were called, nothing should recontent
    Q_ASSERT( !m_contentWriter );
    Q_ASSERT( !m_bodyWriter );
    Q_ASSERT( !m_storeDevice );
    Q_ASSERT( !m_contentTmpFile );
    delete m_contentWriter;
    delete m_bodyWriter;
    delete m_storeDevice;
    delete m_contentTmpFile;
}

KoXmlWriter* KoOasisStore::contentWriter()
{
    if ( !m_contentWriter ) {
        if ( !m_store->open( "content.xml" ) )
            return 0;
        m_storeDevice = new KoStoreDevice( m_store );
        m_contentWriter = KoDocument::createOasisXmlWriter( m_storeDevice, "office:document-content" );
    }
    return m_contentWriter;
}

KoXmlWriter* KoOasisStore::bodyWriter()
{
    if ( !m_bodyWriter ) {
        Q_ASSERT( !m_contentTmpFile );
        m_contentTmpFile = new KTempFile;
        m_contentTmpFile->setAutoDelete( true );
        m_bodyWriter = new KoXmlWriter( m_contentTmpFile->file(), 1 );
    }
    return m_bodyWriter;
}

bool KoOasisStore::closeContentWriter()
{
    Q_ASSERT( m_bodyWriter );
    Q_ASSERT( m_contentTmpFile );

    delete m_bodyWriter; m_bodyWriter = 0;
    // copy over the contents from the tempfile to the real one
    QFile* tmpFile = m_contentTmpFile->file();
    tmpFile->close();
    m_contentWriter->addCompleteElement( tmpFile );
    m_contentTmpFile->close();
    delete m_contentTmpFile; m_contentTmpFile = 0;

    Q_ASSERT( m_contentWriter );
    m_contentWriter->endElement(); // document-content
    m_contentWriter->endDocument();
    delete m_contentWriter; m_contentWriter = 0;
    delete m_storeDevice; m_storeDevice = 0;
    if ( !m_store->close() ) // done with content.xml
        return false;
    return true;
}

bool KoOasisStore::loadAndParse( const QString& fileName, QDomDocument& doc, QString& errorMessage )
{
    //kdDebug(30003) << "loadAndParse: Trying to open " << filename << endl;

    if (!m_store->open(fileName))
    {
        kdWarning(30003) << "Entry " << fileName << " not found!" << endl;
        errorMessage = i18n( "Could not find %1" ).arg( fileName );
        return false;
    }
    // Error variables for QDomDocument::setContent
    QString errorMsg;
    int errorLine, errorColumn;

    // We need to be able to see the space in <text:span> </text:span>, this is why
    // we activate the "report-whitespace-only-CharData" feature.
    // Unfortunately this leads to lots of whitespace text nodes in between real
    // elements in the rest of the document, watch out for that.
    QXmlInputSource source( m_store->device() );
    // Copied from QDomDocumentPrivate::setContent, to change the whitespace thing
    QXmlSimpleReader reader;
    KoDocument::setupXmlReader( reader, true /*namespaceProcessing*/ );

    bool ok = doc.setContent( &source, &reader, &errorMsg, &errorLine, &errorColumn );
    if ( !ok )
    {
        kdError(30003) << "Parsing error in " << fileName << "! Aborting!" << endl
                       << " In line: " << errorLine << ", column: " << errorColumn << endl
                       << " Error message: " << errorMsg << endl;
        errorMessage = i18n( "Parsing error in the main document at line %1, column %2\nError message: %3" )
                       .arg( errorLine ).arg( errorColumn ).arg( i18n ( "QXml", errorMsg.utf8() ) );
    }
    else
    {
        kdDebug(30003) << "File " << fileName << " loaded and parsed" << endl;
    }
    m_store->close();
    return ok;
}
