#include "koxmlwriter.h"

#include <qapplication.h>
#include <qfile.h>
#include <qdatetime.h>
#include <qbuffer.h>
#include <qregexp.h>

static const int numParagraphs = 30000;
void speedTest()
{
    QTime time;
    time.start();
    QString paragText = QString::fromUtf8( "This is the text of the paragraph. I'm including a euro sign to test encoding issues: €" );
    QCString styleName = "Heading 1";

    QFile out( "out5.xml" );
    out.open(IO_WriteOnly);
    {
        KoXmlWriter writer( &out );
        writer.startDocument( "rootelem" );
        writer.startElement( "rootelem" );
        for ( int i = 0 ; i < numParagraphs ; ++i )
        {
            writer.startElement( "paragraph" );
            writer.addAttribute( "text:style-name", styleName );
            writer.addTextNode( paragText );
            writer.endElement();
        }
        writer.endElement();
        writer.endDocument();
    }
    out.close();
    qDebug( "writing %i XML elements using TagWriter: %i", numParagraphs, time.elapsed() );
}

int main( int argc, char** argv ) {
    QApplication app( argc, argv, QApplication::Tty );

#define TEST_BEGIN(publicId,systemId) \
    { \
        QCString cstr; \
        QBuffer buffer( cstr ); \
        buffer.open( IO_WriteOnly ); \
        { \
            KoXmlWriter writer( &buffer ); \
            writer.startDocument( "r", publicId, systemId ); \
            writer.startElement( "r" )

#define TEST_END(testname, expected) \
            writer.endElement(); \
            writer.endDocument(); \
        } \
        buffer.putch( '\0' ); /*null-terminate*/ \
        QCString expectedFull( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" ); \
        expectedFull += expected; \
        if ( cstr == expectedFull ) \
            qDebug( "%s OK", testname ); \
        else { \
            qDebug( "%s FAILED!", testname ); \
            QCString s1 = cstr; \
            QCString s2 = expectedFull; \
            if ( s1.length() != s2.length() ) \
                qDebug( "got length %d, expected %d", s1.length(), s2.length() ); \
            s1.replace( QRegExp( "[x]{1000}" ), "[x]*1000" ); \
            s2.replace( QRegExp( "[x]{1000}" ), "[x]*1000" ); \
            qDebug( "%s", s1.data() ); \
            qDebug( "Expected:\n%s", s2.data() ); \
            return 1; /*exit*/ \
        } \
    }

    TEST_BEGIN( 0, 0 );
    TEST_END( "framework test", "<!DOCTYPE r>\n<r/>\n" );

    TEST_BEGIN( "-//KDE//DTD kword 1.3//EN", "http://www.koffice.org/DTD/kword-1.3.dtd" );
    TEST_END( "doctype test", "<!DOCTYPE r PUBLIC \"-//KDE//DTD kword 1.3//EN\" \"http://www.koffice.org/DTD/kword-1.3.dtd\">\n<r/>\n" );

    TEST_BEGIN( 0, 0 );
    writer.addAttribute( "a", "val" );
    writer.addAttribute( "b", "<\">" );
    TEST_END( "attributes test", "<!DOCTYPE r>\n<r a=\"val\" b=\"&lt;&quot;&gt;\"/>\n" );

    TEST_BEGIN( 0, 0 );
    writer.startElement( "a" );
    writer.startElement( "b" );
    writer.startElement( "c" );
    writer.endElement();
    writer.endElement();
    writer.endElement();
    TEST_END( "indent test", "<!DOCTYPE r>\n<r>\n <a>\n  <b>\n   <c/>\n  </b>\n </a>\n</r>\n" );

    TEST_BEGIN( 0, 0 );
    writer.startElement( "a" );
    writer.startElement( "b" );
    writer.startElement( "c" );
    writer.endElement();
    writer.addTextNode( "te" );
    writer.addTextNode( "xt" );
    writer.endElement();
    writer.endElement();
    TEST_END( "textnode test", "<!DOCTYPE r>\n<r>\n <a>\n  <b>\n   <c/>text</b>\n </a>\n</r>\n" );

    int sz = 15000;  // must be more than KoXmlWriter::s_escapeBufferLen
    QCString x( sz );
    x.fill( 'x', sz );
    x += '&';
    QCString expected = "<!DOCTYPE r>\n<r a=\"";
    expected += x + "amp;\"/>\n";
    TEST_BEGIN( 0, 0 );
    writer.addAttribute( "a", x );
    TEST_END( "escaping long string", expected.data() );

    speedTest();
}
