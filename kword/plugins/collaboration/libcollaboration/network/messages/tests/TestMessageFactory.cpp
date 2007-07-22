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
#include "TestMessageFactory.h"
#include "../MessageFactory.h"
#include "XmlHelper.h"

void kcollaborate::Message::TestMessageFactory::testProcessRawMessage_data()
{
    QTest::addColumn<bool>( "parentIsNull" );
    QTest::addColumn<QString>( "msgRaw" );
    QTest::addColumn<QString>( "msgParsed" );

    QTest::newRow( "Hello" ) << true << QString(
        "<Messages version=\"0.1\">\n"
        "<Hello protocolVersion=\"protocolVersion2\" invitationNumber=\"invitationNumber2\" id=\"id2\" >\n"
        " <User pictureUrl=\"pictureUrl\" name=\"name\" />\n"
        "</Hello>\n"
        "</Messages>\n"
    ) << QString(
        "<Hello protocolVersion=\"protocolVersion2\" invitationNumber=\"invitationNumber2\" id=\"id2\" >\n"
        " <User pictureUrl=\"pictureUrl\" name=\"name\" />\n"
        "</Hello>\n"
    );

    QTest::newRow( "HelloAnswer" ) << false << QString(
        "<Messages version=\"0.1\">\n"
        "<HelloAnswer id=\"id\" >\n"
        " <Accepted sessionId=\"sessionId\" >\n"
        "  <ReadOnly/>\n"
        " </Accepted>\n"
        "</HelloAnswer>\n"
        "</Messages>\n"
    ) << QString(
        "<HelloAnswer id=\"id\" >\n"
        " <Accepted sessionId=\"sessionId\" >\n"
        "  <ReadOnly/>\n"
        " </Accepted>\n"
        "</HelloAnswer>\n"
    );

    QTest::newRow( "Session_Update" ) << true << QString(
        "<Messages version=\"0.1\">\n"
        "<Session id=\"sessionId\">"
        "<Update objectName=\"objectName\" commandName=\"commandName\" timestamp=\"timestamp\" id=\"id\" ><![CDATA[data]]></Update>\n"
        "</Session>"
        "</Messages>\n"
    ) << QString(
        "<Update objectName=\"objectName\" commandName=\"commandName\" timestamp=\"timestamp\" id=\"id\" ><![CDATA[data]]></Update>\n"
    );

    QTest::newRow( "Session_UpdateAnswer" ) << true << QString(
        "<Messages version=\"0.1\">\n"
        "<Session id=\"sessionId\">"
        "<UpdateAnswer status=\"Accepted\" id=\"id\" />\n"
        "</Session>"
        "</Messages>\n"
    ) << QString(
        "<UpdateAnswer status=\"Accepted\" id=\"id\" />\n"
    );

    QTest::newRow( "Session_Closed" ) << true << QString(
        "<Messages version=\"0.1\">\n"
        "<Session id=\"sessionId\">"
        "<Closed />\n"
        "</Session>"
        "</Messages>\n"
    ) << QString(
        "sessionId"
    );
}

void kcollaborate::Message::TestMessageFactory::testProcessRawMessage()
{
    QFETCH( bool, parentIsNull );
    QFETCH( QString, msgRaw );
    QFETCH( QString, msgParsed );

    kcollaborate::Message::MessageFactory *messageFactory;
    if ( parentIsNull ) {
        messageFactory = new kcollaborate::Message::MessageFactory();

        QCOMPARE(( QObject* )messageFactory->parent(), ( QObject* )NULL );
    } else {
        messageFactory = new kcollaborate::Message::MessageFactory( this );

        QCOMPARE(( QObject* )messageFactory->parent(), ( QObject* )this );
    }

    qRegisterMetaType<kcollaborate::Message::Hello>( "kcollaborate::Message::Hello" );
    qRegisterMetaType<kcollaborate::Message::HelloAnswer>( "kcollaborate::Message::HelloAnswer" );
    qRegisterMetaType<kcollaborate::Message::Update>( "kcollaborate::Message::Update" );
    qRegisterMetaType<kcollaborate::Message::UpdateAnswer>( "kcollaborate::Message::UpdateAnswer" );

    QSignalSpy spy1( messageFactory, SIGNAL( messageReceivedHello( const kcollaborate::Message::Hello & ) ) );
    QSignalSpy spy2( messageFactory, SIGNAL( messageReceivedHelloAnswer( const kcollaborate::Message::HelloAnswer & ) ) );
    QSignalSpy spy3( messageFactory, SIGNAL( messageReceivedUpdate( const kcollaborate::Message::Update & ) ) );
    QSignalSpy spy4( messageFactory, SIGNAL( messageReceivedUpdateAnswer( const kcollaborate::Message::UpdateAnswer & ) ) );
    QSignalSpy spy5( messageFactory, SIGNAL( messageReceivedSessionClosed( const QString & ) ) );
    QVERIFY( spy1.isValid() );
    QVERIFY( spy2.isValid() );
    QVERIFY( spy3.isValid() );
    QVERIFY( spy4.isValid() );
    QVERIFY( spy5.isValid() );
    QCOMPARE( spy1.count(), 0 );
    QCOMPARE( spy2.count(), 0 );
    QCOMPARE( spy3.count(), 0 );
    QCOMPARE( spy4.count(), 0 );
    QCOMPARE( spy5.count(), 0 );

    messageFactory->processRawMessage( msgRaw );

    QCOMPARE( spy1.count() + spy2.count() + spy3.count() + spy4.count() + spy5.count(), 1 );

    QString msgString;
    kcollaborate::Message::Generic *msg = NULL;
    if ( !spy1.isEmpty() ) {
        msg = new kcollaborate::Message::Hello( spy1.at( 0 ).at( 0 ).value<kcollaborate::Message::Hello>() );
    } else if ( !spy2.isEmpty() ) {
        msg = new kcollaborate::Message::HelloAnswer( spy2.at( 0 ).at( 0 ).value<kcollaborate::Message::HelloAnswer>() );
    } else if ( !spy3.isEmpty() ) {
        msg = new kcollaborate::Message::Update( spy3.at( 0 ).at( 0 ).value<kcollaborate::Message::Update>() );
    } else if ( !spy4.isEmpty() ) {
        msg = new kcollaborate::Message::UpdateAnswer( spy4.at( 0 ).at( 0 ).value<kcollaborate::Message::UpdateAnswer>() );
    } else if ( !spy5.isEmpty() ) {
        msgString = spy5.at( 0 ).at( 0 ).value<QString>();
    }

    if ( spy5.isEmpty() ) {
        QVERIFY( msg );
        msgString = msg->toString();
    }
    QCOMPARE( msgString, msgParsed );

    delete messageFactory;
}

QTEST_MAIN( kcollaborate::Message::TestMessageFactory )
#include "TestMessageFactory.moc"
