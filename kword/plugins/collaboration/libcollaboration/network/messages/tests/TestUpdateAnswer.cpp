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
#include "TestUpdateAnswer.h"
#include "../UpdateAnswer.h"
#include "XmlHelper.h"

void kcollaborate::Message::TestUpdateAnswer::testConstructor1_data()
{
    QTest::addColumn<QString>( "id" );
    QTest::addColumn<int>( "status" );
    QTest::addColumn<QString>( "sessionId" );
    QTest::addColumn<bool>( "parentIsNull" );
    QTest::addColumn<QString>( "xml" );

    QTest::newRow( "Accepted" ) << QString( "id" ) << ( int )kcollaborate::Message::UpdateAnswer::Accepted
    << QString( "sessionId" ) << true
    << QString( "<UpdateAnswer status=\"Accepted\" id=\"id\" />\n" );

    QTest::newRow( "Accepted2" ) << QString( "id" ) << ( int )kcollaborate::Message::UpdateAnswer::Accepted
    << QString( "sessionId" ) << false
    << QString( "<UpdateAnswer status=\"Accepted\" id=\"id\" />\n" );

    QTest::newRow( "Rejected" ) << QString( "id2" ) << ( int )kcollaborate::Message::UpdateAnswer::Rejected
    << QString( "" ) << true
    << QString( "<UpdateAnswer status=\"Rejected\" id=\"id2\" />\n" );

    QTest::newRow( "Unsupported" ) << QString( "id3" ) << ( int )kcollaborate::Message::UpdateAnswer::Unsupported
    << QString( "" ) << true
    << QString( "<UpdateAnswer status=\"Unsupported\" id=\"id3\" />\n" );

    QTest::newRow( "Timeout" ) << QString( "id4" ) << ( int )kcollaborate::Message::UpdateAnswer::Timeout
    << QString( "" ) << true
    << QString( "<UpdateAnswer status=\"Timeout\" id=\"id4\" />\n" );
}

void kcollaborate::Message::TestUpdateAnswer::testConstructor1()
{
    QFETCH( QString, id );
    QFETCH( int, status );
    QFETCH( QString, sessionId );
    QFETCH( bool, parentIsNull );
    QFETCH( QString, xml );

    kcollaborate::Message::UpdateAnswer *updateAnswer;
    if ( parentIsNull ) {
        updateAnswer = new kcollaborate::Message::UpdateAnswer(
                          id, ( kcollaborate::Message::UpdateAnswer::UpdateAnswerStatus )status,
                          sessionId
                      );

        QCOMPARE(( QObject* )updateAnswer->parent(), ( QObject* )NULL );
    } else {
        updateAnswer = new kcollaborate::Message::UpdateAnswer(
                          id, ( kcollaborate::Message::UpdateAnswer::UpdateAnswerStatus )status,
                          sessionId, this
                      );

        QCOMPARE(( QObject* )updateAnswer->parent(), ( QObject* )this );
    }

    QCOMPARE( updateAnswer->tagName(), QString::fromAscii( "UpdateAnswer" ) );

    QCOMPARE( updateAnswer->id(), id );
    QCOMPARE(( int )updateAnswer->status(), ( int )status );
    QCOMPARE( updateAnswer->sessionId(), sessionId );

    QCOMPARE( updateAnswer->toString(), xml );

    delete updateAnswer;
}

void kcollaborate::Message::TestUpdateAnswer::testConstructorXml1_data()
{
    QTest::addColumn<QString>( "id" );
    QTest::addColumn<int>( "status" );
    QTest::addColumn<QString>( "sessionId" );
    QTest::addColumn<bool>( "parentIsNull" );
    QTest::addColumn<QString>( "xml" );

    QTest::newRow( "Accepted" ) << QString( "id" ) << ( int )kcollaborate::Message::UpdateAnswer::Accepted
    << QString( "sessionId" ) << true
    << QString( "<UpdateAnswer status=\"Accepted\" id=\"id\" />\n" );

    QTest::newRow( "Accepted2" ) << QString( "id" ) << ( int )kcollaborate::Message::UpdateAnswer::Accepted
    << QString( "sessionId" ) << false
    << QString( "<UpdateAnswer status=\"Accepted\" id=\"id\" />\n" );

    QTest::newRow( "Rejected" ) << QString( "id2" ) << ( int )kcollaborate::Message::UpdateAnswer::Rejected
    << QString( "" ) << true
    << QString( "<UpdateAnswer status=\"Rejected\" id=\"id2\" />\n" );

    QTest::newRow( "Unsupported" ) << QString( "id3" ) << ( int )kcollaborate::Message::UpdateAnswer::Unsupported
    << QString( "" ) << true
    << QString( "<UpdateAnswer status=\"Unsupported\" id=\"id3\" />\n" );

    QTest::newRow( "Timeout" ) << QString( "id4" ) << ( int )kcollaborate::Message::UpdateAnswer::Timeout
    << QString( "" ) << true
    << QString( "<UpdateAnswer status=\"Timeout\" id=\"id4\" />\n" );
}

void kcollaborate::Message::TestUpdateAnswer::testConstructorXml1()
{
    QFETCH( QString, id );
    QFETCH( int, status );
    QFETCH( QString, sessionId );
    QFETCH( bool, parentIsNull );
    QFETCH( QString, xml );

    kcollaborate::Message::UpdateAnswer *updateAnswer;
    if ( parentIsNull ) {
        updateAnswer = new kcollaborate::Message::UpdateAnswer(
                          sessionId, kcollaborate::Message::XmlHelper::parse( xml )
                      );

        QCOMPARE(( QObject* )updateAnswer->parent(), ( QObject* )NULL );
    } else {
        updateAnswer = new kcollaborate::Message::UpdateAnswer(
                          sessionId, kcollaborate::Message::XmlHelper::parse( xml ), this
                      );

        QCOMPARE(( QObject* )updateAnswer->parent(), ( QObject* )this );
    }

    QCOMPARE( updateAnswer->tagName(), QString::fromAscii( "UpdateAnswer" ) );

    QCOMPARE( updateAnswer->id(), id );
    QCOMPARE(( int )updateAnswer->status(), ( int )status );
    QCOMPARE( updateAnswer->sessionId(), sessionId );

    QCOMPARE( updateAnswer->toString(), xml );

    delete updateAnswer;

}

QTEST_MAIN( kcollaborate::Message::TestUpdateAnswer )
#include "TestUpdateAnswer.moc"
