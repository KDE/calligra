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
#include "TestHello.h"
#include "../Hello.h"
#include "../User.h"
#include "XmlHelper.h"

void kcollaborate::Message::TestHello::initTestCase()
{
    xml1 = "<Hello protocolVersion=\"protocolVersion\" invitationNumber=\"invitationNumber\" id=\"id\" />\n";
    xml2 = "<Hello protocolVersion=\"protocolVersion2\" invitationNumber=\"invitationNumber2\" id=\"id2\" >\n"
           " <User pictureUrl=\"pictureUrl\" name=\"name\" />\n"
           "</Hello>\n";
}

void kcollaborate::Message::TestHello::testConstructor1()
{
    kcollaborate::Message::Hello *hello;
    hello = new kcollaborate::Message::Hello(
                "id", "protocolVersion", "invitationNumber", NULL, NULL
            );

    QCOMPARE( hello->tagName(), QString::fromAscii("Hello") );
    QCOMPARE(( QObject* )hello->parent(), ( QObject* )NULL );
    QCOMPARE( hello->id(), QString::fromAscii( "id" ) );
    QCOMPARE( hello->protocolVersion(), QString::fromAscii( "protocolVersion" ) );
    QCOMPARE( hello->invitationNumber(), QString::fromAscii( "invitationNumber" ) );
    QCOMPARE(( QObject* )hello->user(), ( QObject* )NULL );

    QCOMPARE( hello->toString(), xml1 );

    delete hello;
}

void kcollaborate::Message::TestHello::testConstructor2()
{
    kcollaborate::Message::Hello *hello;
    kcollaborate::Message::User *user;
    user = new kcollaborate::Message::User(
               "name", "pictureUrl", NULL
           );
    hello = new kcollaborate::Message::Hello(
                "id2", "protocolVersion2", "invitationNumber2", user, this
            );

    QCOMPARE(( QObject* )hello->parent(), ( QObject* )this );
    QCOMPARE( hello->id(), QString::fromAscii( "id2" ) );
    QCOMPARE( hello->protocolVersion(), QString::fromAscii( "protocolVersion2" ) );
    QCOMPARE( hello->invitationNumber(), QString::fromAscii( "invitationNumber2" ) );
    QCOMPARE( hello->user(), user );
    QCOMPARE( hello->user()->name(), QString::fromAscii( "name" ) );
    QCOMPARE( hello->user()->pictureUrl(), QString::fromAscii( "pictureUrl" ) );
    QCOMPARE( hello->user()->im().isEmpty(), true );
    QCOMPARE( hello->user()->im().size(), 0 );
    QCOMPARE( hello->user()->im( "any" ), QString::fromAscii( "" ) );

    QCOMPARE( hello->toString(), xml2 );

    delete hello;
}

void kcollaborate::Message::TestHello::testConstructorXml1()
{
    kcollaborate::Message::Hello *hello = new kcollaborate::Message::Hello(
                                              kcollaborate::Message::XmlHelper::parse( xml1 )
                                          );

    QCOMPARE(( QObject* )hello->parent(), ( QObject* )NULL );
    QCOMPARE( hello->id(), QString::fromAscii( "id" ) );
    QCOMPARE( hello->protocolVersion(), QString::fromAscii( "protocolVersion" ) );
    QCOMPARE( hello->invitationNumber(), QString::fromAscii( "invitationNumber" ) );
    QCOMPARE(( QObject* )hello->user(), ( QObject* )NULL );

    QCOMPARE( hello->toString(), xml1 );

    delete hello;
}

void kcollaborate::Message::TestHello::testConstructorXml2()
{
    kcollaborate::Message::Hello *hello = new kcollaborate::Message::Hello(
                                              kcollaborate::Message::XmlHelper::parse( xml2 ), this
                                          );

    QCOMPARE(( QObject* )hello->parent(), ( QObject* )this );
    QCOMPARE( hello->id(), QString::fromAscii( "id2" ) );
    QCOMPARE( hello->protocolVersion(), QString::fromAscii( "protocolVersion2" ) );
    QCOMPARE( hello->invitationNumber(), QString::fromAscii( "invitationNumber2" ) );
    QCOMPARE( hello->user()->name(), QString::fromAscii( "name" ) );
    QCOMPARE( hello->user()->pictureUrl(), QString::fromAscii( "pictureUrl" ) );
    QCOMPARE( hello->user()->im().isEmpty(), true );
    QCOMPARE( hello->user()->im().size(), 0 );
    QCOMPARE( hello->user()->im( "any" ), QString::fromAscii( "" ) );

    QCOMPARE( hello->toString(), xml2 );

    delete hello;
}

QTEST_MAIN( kcollaborate::Message::TestHello )
//QTEST _ KDEMAIN(, NoGUI)
#include "TestHello.moc"
