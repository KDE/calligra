#include "kodom.h"
#include "koxmlns.h"

#include <qapplication.h>
#include <assert.h>

//static void debugElemNS( const QDomElement& elem )
//{
//    qDebug( "nodeName=%s tagName=%s localName=%s prefix=%s namespaceURI=%s", elem.nodeName().latin1(), elem.tagName().latin1(), elem.localName().latin1(), elem.prefix().latin1(), elem.namespaceURI().latin1() );
//}

void testQDom( const QDomDocument& doc )
{
    QDomElement docElem = doc.documentElement();
    //debugElemNS( docElem );
    assert( docElem.nodeName() == "o:document-content" );
    assert( docElem.tagName() == "document-content" );
    assert( docElem.localName() == "document-content" );
    assert( docElem.prefix() == "o" );
    assert( docElem.namespaceURI() == KoXmlNS::office );

    // WARNING, elementsByTagNameNS is recursive!!!
    QDomNodeList docElemChildren = docElem.elementsByTagNameNS( KoXmlNS::office, "body" );
    assert( docElemChildren.length() == 1 );

    QDomElement elem = docElemChildren.item( 0 ).toElement();
    //debugElemNS( elem );
    assert( elem.tagName() == "body" );
    assert( elem.localName() == "body" );
    assert( elem.prefix() == "o" );
    assert( elem.namespaceURI() == KoXmlNS::office );

    QDomNode n = elem.firstChild();
    for ( ; !n.isNull() ; n = n.nextSibling() ) {
        if ( !n.isElement() )
            continue;
        QDomElement e = n.toElement();
        //debugElemNS( e );
        assert( e.tagName() == "p" );
        assert( e.localName() == "p" );
        assert( e.prefix().isEmpty() );
        assert( e.namespaceURI() == KoXmlNS::text );
    }

    qDebug("testQDom... ok");
}

void testKoDom( const QDomDocument& doc )
{
    QDomElement docElem = KoDom::namedItemNS( doc, KoXmlNS::office, "document-content" );
    assert( !docElem.isNull() );
    assert( docElem.localName() == "document-content" );
    assert( docElem.namespaceURI() == KoXmlNS::office );

    QDomElement body = KoDom::namedItemNS( docElem, KoXmlNS::office, "body" );
    assert( !body.isNull() );
    assert( body.localName() == "body" );
    assert( body.namespaceURI() == KoXmlNS::office );

    QDomElement p = KoDom::namedItemNS( body, KoXmlNS::text, "p" );
    assert( !p.isNull() );
    assert( p.localName() == "p" );
    assert( p.namespaceURI() == KoXmlNS::text );

    // Look for a non-existing element
    QDomElement notexist = KoDom::namedItemNS( body, KoXmlNS::text, "notexist" );
    assert( notexist.isNull() );

    qDebug("testKoDom... ok");
}

int main( int argc, char** argv ) {
    QApplication app( argc, argv, QApplication::Tty );

    const QCString xml = QCString( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                   "<o:document-content xmlns:o=\"" )
                         + KoXmlNS::office
                         + "\" xmlns=\"" + KoXmlNS::text + "\">\n"
		"<o:body><p style-name=\"L1\">foobar</p></o:body>\n"
		"</o:document-content>\n";
    QDomDocument doc;
    //QXmlSimpleReader reader;
    //reader.setFeature( "http://xml.org/sax/features/namespaces", TRUE );
    //reader.setFeature( "http://xml.org/sax/features/namespace-prefixes", FALSE );
    bool ok = doc.setContent( xml, true /* namespace processing */ );
    assert( ok );

    testQDom(doc);
    testKoDom(doc);
}
