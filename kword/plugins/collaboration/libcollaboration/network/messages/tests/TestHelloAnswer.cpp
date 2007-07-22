/*
 * Copyright (C) 2007 Igor Stepin <igor_for_os@stepin.name>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "TestHelloAnswer.h"
#include "../HelloAnswer.h"
#include "XmlHelper.h"

void kcollaborate::Message::TestHelloAnswer::testConstructor1_data()
{
    QTest::addColumn<QString>( "id" );
    QTest::addColumn<int>( "status" );
    QTest::addColumn<QString>( "sessionId" );
    QTest::addColumn<bool>( "readOnly" );
    QTest::addColumn<QString>( "text" );
    QTest::addColumn<bool>( "parentIsNull" );
    QTest::addColumn<QString>( "xml" );

    QTest::newRow( "Accepted" ) << QString( "id" ) << ( int )kcollaborate::Message::HelloAnswer::Accepted
    << QString( "sessionId" ) << true << QString( "" ) << true
    << QString( "<HelloAnswer id=\"id\" >\n"
                " <Accepted sessionId=\"sessionId\" >\n"
                "  <ReadOnly/>\n"
                " </Accepted>\n"
                "</HelloAnswer>\n"
              );

    QTest::newRow( "Accepted2" ) << QString( "id" ) << ( int )kcollaborate::Message::HelloAnswer::Accepted
    << QString( "sessionId" ) << false << QString( "" ) << false
    << QString( "<HelloAnswer id=\"id\" >\n"
                " <Accepted sessionId=\"sessionId\" />\n"
                "</HelloAnswer>\n"
              );

    QTest::newRow( "Rejected" ) << QString( "id2" ) << ( int )kcollaborate::Message::HelloAnswer::Rejected
    << QString( "" ) << true << QString( "text2" ) << true
    << QString( "<HelloAnswer id=\"id2\" >\n"
                " <Rejected><![CDATA[text2]]></Rejected>\n"
                "</HelloAnswer>\n"
              );

    QTest::newRow( "Unsupported" ) << QString( "id3" ) << ( int )kcollaborate::Message::HelloAnswer::Unsupported
    << QString( "" ) << true << QString( "text3" ) << true
    << QString( "<HelloAnswer id=\"id3\" >\n"
                " <Unsupported><![CDATA[text3]]></Unsupported>\n"
                "</HelloAnswer>\n"
              );

    QTest::newRow( "Timeout" ) << QString( "id4" ) << ( int )kcollaborate::Message::HelloAnswer::Timeout
    << QString( "" ) << true << QString( "text4" ) << true
    << QString( "<HelloAnswer id=\"id4\" >\n"
                " <Timeout><![CDATA[text4]]></Timeout>\n"
                "</HelloAnswer>\n"
              );
}

void kcollaborate::Message::TestHelloAnswer::testConstructor1()
{
    QFETCH( QString, id );
    QFETCH( int, status );
    QFETCH( QString, sessionId );
    QFETCH( bool, readOnly );
    QFETCH( QString, text );
    QFETCH( bool, parentIsNull );
    QFETCH( QString, xml );

    kcollaborate::Message::HelloAnswer *helloAnswer;
    if ( parentIsNull ) {
        helloAnswer = new kcollaborate::Message::HelloAnswer(
                          id, ( kcollaborate::Message::HelloAnswer::HelloAnswerStatus )status,
                          sessionId, readOnly, text
                      );

        QCOMPARE(( QObject* )helloAnswer->parent(), ( QObject* )NULL );
    } else {
        helloAnswer = new kcollaborate::Message::HelloAnswer(
                          id, ( kcollaborate::Message::HelloAnswer::HelloAnswerStatus )status,
                          sessionId, readOnly, text, this
                      );

        QCOMPARE(( QObject* )helloAnswer->parent(), ( QObject* )this );
    }

    QCOMPARE( helloAnswer->tagName(), QString::fromAscii( "HelloAnswer" ) );

    QCOMPARE( helloAnswer->id(), id );
    QCOMPARE(( int )helloAnswer->status(), ( int )status );
    QCOMPARE( helloAnswer->sessionId(), sessionId );
    if ( helloAnswer->status() == kcollaborate::Message::HelloAnswer::Accepted ) {
        QCOMPARE( helloAnswer->isReadOnly(), readOnly );
    }
    QCOMPARE( helloAnswer->text(), text );

    QCOMPARE( helloAnswer->toString(), xml );

    delete helloAnswer;
}

void kcollaborate::Message::TestHelloAnswer::testConstructorXml1_data()
{
    QTest::addColumn<QString>( "id" );
    QTest::addColumn<int>( "status" );
    QTest::addColumn<QString>( "sessionId" );
    QTest::addColumn<bool>( "readOnly" );
    QTest::addColumn<QString>( "text" );
    QTest::addColumn<bool>( "parentIsNull" );
    QTest::addColumn<QString>( "xml" );

    QTest::newRow( "Accepted" ) << QString( "id" ) << ( int )kcollaborate::Message::HelloAnswer::Accepted
    << QString( "sessionId" ) << true << QString( "" ) << true
    << QString( "<HelloAnswer id=\"id\" >\n"
                " <Accepted sessionId=\"sessionId\" >\n"
                "  <ReadOnly/>\n"
                " </Accepted>\n"
                "</HelloAnswer>\n"
              );

    QTest::newRow( "Accepted2" ) << QString( "id" ) << ( int )kcollaborate::Message::HelloAnswer::Accepted
    << QString( "sessionId" ) << false << QString( "" ) << false
    << QString( "<HelloAnswer id=\"id\" >\n"
                " <Accepted sessionId=\"sessionId\" />\n"
                "</HelloAnswer>\n"
              );

    QTest::newRow( "Rejected" ) << QString( "id2" ) << ( int )kcollaborate::Message::HelloAnswer::Rejected
    << QString( "" ) << true << QString( "text2" ) << true
    << QString( "<HelloAnswer id=\"id2\" >\n"
                " <Rejected><![CDATA[text2]]></Rejected>\n"
                "</HelloAnswer>\n"
              );

    QTest::newRow( "Unsupported" ) << QString( "id3" ) << ( int )kcollaborate::Message::HelloAnswer::Unsupported
    << QString( "" ) << true << QString( "text3" ) << true
    << QString( "<HelloAnswer id=\"id3\" >\n"
                " <Unsupported><![CDATA[text3]]></Unsupported>\n"
                "</HelloAnswer>\n"
              );

    QTest::newRow( "Timeout" ) << QString( "id4" ) << ( int )kcollaborate::Message::HelloAnswer::Timeout
    << QString( "" ) << true << QString( "text4" ) << true
    << QString( "<HelloAnswer id=\"id4\" >\n"
                " <Timeout><![CDATA[text4]]></Timeout>\n"
                "</HelloAnswer>\n"
              );
}

void kcollaborate::Message::TestHelloAnswer::testConstructorXml1()
{
    QFETCH( QString, id );
    QFETCH( int, status );
    QFETCH( QString, sessionId );
    QFETCH( bool, readOnly );
    QFETCH( QString, text );
    QFETCH( bool, parentIsNull );
    QFETCH( QString, xml );

    kcollaborate::Message::HelloAnswer *helloAnswer;
    if ( parentIsNull ) {
        helloAnswer = new kcollaborate::Message::HelloAnswer(
                          kcollaborate::Message::XmlHelper::parse( xml )
                      );

        QCOMPARE(( QObject* )helloAnswer->parent(), ( QObject* )NULL );
    } else {
        helloAnswer = new kcollaborate::Message::HelloAnswer(
                          kcollaborate::Message::XmlHelper::parse( xml ), this
                      );

        QCOMPARE(( QObject* )helloAnswer->parent(), ( QObject* )this );
    }

    QCOMPARE( helloAnswer->tagName(), QString::fromAscii( "HelloAnswer" ) );

    QCOMPARE( helloAnswer->id(), id );
    QCOMPARE(( int )helloAnswer->status(), ( int )status );
    QCOMPARE( helloAnswer->sessionId(), sessionId );
    if ( helloAnswer->status() == kcollaborate::Message::HelloAnswer::Accepted ) {
        QCOMPARE( helloAnswer->isReadOnly(), readOnly );
    }
    QCOMPARE( helloAnswer->text(), text );

    QCOMPARE( helloAnswer->toString(), xml );

    delete helloAnswer;

}

QTEST_MAIN( kcollaborate::Message::TestHelloAnswer )
#include "TestHelloAnswer.moc"
