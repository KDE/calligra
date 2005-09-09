#ifndef XMLWRITERTEST_H
#define XMLWRITERTEST_H

#define QT_NO_CAST_ASCII

// Those macros are in a separate header file in order to share them
// with kofficecore/tests/kogenstylestest.cpp

#include <qbuffer.h>
#include <qregexp.h>

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
            s1.replace( QRegExp( QString::fromLatin1( "[x]{1000}" ) ), "[x]*1000" ); \
            s2.replace( QRegExp( QString::fromLatin1( "[x]{1000}" ) ), "[x]*1000" ); \
            qDebug( "%s", s1.data() ); \
            qDebug( "Expected:\n%s", s2.data() ); \
            return 1; /*exit*/ \
        } \
    }


#endif
