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
#include "TestUser.h"
#include "../User.h"
#include "XmlHelper.h"

void kcollaborate::Message::TestUser::initTestCase()
{
    xml1 = "<User pictureUrl=\"\" name=\"\" />\n";
    xml2 = "<User pictureUrl=\"pictureUrl\" name=\"name\" >\n"
           " <Im account=\"123\" type=\"any\" />\n"
           "</User>\n";
}

void kcollaborate::Message::TestUser::testConstructor1()
{
    kcollaborate::Message::User *user = new kcollaborate::Message::User();

    QCOMPARE(( QObject* )user->parent(), ( QObject* )NULL );
    QCOMPARE( user->name(), QString::fromAscii( "" ) );
    QCOMPARE( user->pictureUrl(), QString::fromAscii( "" ) );
    QCOMPARE( user->im().isEmpty(), true );
    QCOMPARE( user->im( "any" ), QString::fromAscii( "" ) );
    QCOMPARE( user->tagName(), QString::fromAscii( "User" ) );

    QCOMPARE( user->toString(), xml1 );

    delete user;
}

void kcollaborate::Message::TestUser::testConstructor2()
{
    kcollaborate::Message::User *user = new kcollaborate::Message::User(
                                            "name", "pictureUrl", this
                                        );
    user->addIm( "any", "123" );

    QCOMPARE(( QObject* )user->parent(), ( QObject* )this );
    QCOMPARE( user->name(), QString::fromAscii( "name" ) );
    QCOMPARE( user->pictureUrl(), QString::fromAscii( "pictureUrl" ) );
    QCOMPARE( user->im().isEmpty(), false );
    QCOMPARE( user->im().size(), 1 );
    QCOMPARE( user->im( "any" ), QString::fromAscii( "123" ) );
    QCOMPARE( user->tagName(), QString::fromAscii( "User" ) );

    QCOMPARE( user->toString(), xml2 );

    delete user;
}

void kcollaborate::Message::TestUser::testConstructorXml1()
{
    kcollaborate::Message::User *user = new kcollaborate::Message::User(
                                            kcollaborate::Message::XmlHelper::parse( xml1 )
                                        );

    QCOMPARE(( QObject* )user->parent(), ( QObject* )NULL );
    QCOMPARE( user->name(), QString::fromAscii( "" ) );
    QCOMPARE( user->pictureUrl(), QString::fromAscii( "" ) );
    QCOMPARE( user->im().isEmpty(), true );
    QCOMPARE( user->im( "any" ), QString::fromAscii( "" ) );
    QCOMPARE( user->tagName(), QString::fromAscii( "User" ) );

    QCOMPARE( user->toString(), xml1 );

    delete user;
}

void kcollaborate::Message::TestUser::testConstructorXml2()
{
    kcollaborate::Message::User *user = new kcollaborate::Message::User(
                                            kcollaborate::Message::XmlHelper::parse( xml2 ), this
                                        );

    QCOMPARE(( QObject* )user->parent(), ( QObject* )this );
    QCOMPARE( user->name(), QString::fromAscii( "name" ) );
    QCOMPARE( user->pictureUrl(), QString::fromAscii( "pictureUrl" ) );
    QCOMPARE( user->im().isEmpty(), false );
    QCOMPARE( user->im().size(), 1 );
    QCOMPARE( user->im( "any" ), QString::fromAscii( "123" ) );
    QCOMPARE( user->tagName(), QString::fromAscii( "User" ) );

    QCOMPARE( user->toString(), xml2 );

    delete user;
}

QTEST_MAIN( kcollaborate::Message::TestUser )
#include "TestUser.moc"
