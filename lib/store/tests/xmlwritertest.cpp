#include "koxmlwriter.h"
#include "xmlwritertest.h"

#include <qapplication.h>
#include <qfile.h>
#include <qdatetime.h>

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

    TEST_BEGIN( 0, 0 );
    TEST_END( "framework test", "<!DOCTYPE r>\n<r/>\n" );

    TEST_BEGIN( "-//KDE//DTD kword 1.3//EN", "http://www.koffice.org/DTD/kword-1.3.dtd" );
    TEST_END( "doctype test", "<!DOCTYPE r PUBLIC \"-//KDE//DTD kword 1.3//EN\" \"http://www.koffice.org/DTD/kword-1.3.dtd\">\n<r/>\n" );

    TEST_BEGIN( 0, 0 );
    writer.addAttribute( "a", "val" );
    writer.addAttribute( "b", "<\">" );
    writer.addAttribute( "c", -42 );
    writer.addAttribute( "d", 1234.56789012345 );
    writer.addAttributePt( "e", 1234.56789012345 );
    TEST_END( "attributes test", "<!DOCTYPE r>\n<r a=\"val\" b=\"&lt;&quot;&gt;\" c=\"-42\" d=\"1234.56789012345\" e=\"1234.56789012345pt\"/>\n" );

    TEST_BEGIN( 0, 0 );
    writer.startElement( "m" );
    writer.endElement();
    TEST_END( "empty element test", "<!DOCTYPE r>\n<r>\n <m/>\n</r>\n" );

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
