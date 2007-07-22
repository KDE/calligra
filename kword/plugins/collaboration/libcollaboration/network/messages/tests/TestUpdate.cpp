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
#include "TestUpdate.h"
#include "../Update.h"
#include "XmlHelper.h"

void kcollaborate::Message::TestUpdate::initTestCase()
{
    xml1 = "<Update objectName=\"objectName\" commandName=\"commandName\" timestamp=\"timestamp\" id=\"id\" ><![CDATA[data]]></Update>\n";
    xml2 = "<Update objectName=\"objectName2\" commandName=\"commandName2\" timestamp=\"timestamp2\" id=\"id2\" ><![CDATA[data2]]></Update>\n";
    xml3 = "<Update objectName=\"objectName2\" commandName=\"commandName2\" timestamp=\"timestamp2\" id=\"id2\" />\n";
}

void kcollaborate::Message::TestUpdate::testConstructor1()
{
    kcollaborate::Message::Update *update;
    update = new kcollaborate::Message::Update(
                 "sessionId", "id", "timestamp", "objectName", "commandName", "data"
             );

    QCOMPARE( update->tagName(), QString::fromAscii( "Update" ) );
    QCOMPARE(( QObject* )update->parent(), ( QObject* )NULL );
    QCOMPARE( update->sessionId(), QString::fromAscii( "sessionId" ) );
    QCOMPARE( update->id(), QString::fromAscii( "id" ) );
    QCOMPARE( update->timestamp(), QString::fromAscii( "timestamp" ) );
    QCOMPARE( update->objectName(), QString::fromAscii( "objectName" ) );
    QCOMPARE( update->commandName(), QString::fromAscii( "commandName" ) );
    QCOMPARE( update->data(), QString::fromAscii( "data" ) );

    QCOMPARE( update->toString(), xml1 );

    delete update;
}

void kcollaborate::Message::TestUpdate::testConstructor2()
{
    kcollaborate::Message::Update *update;
    update = new kcollaborate::Message::Update(
                 "sessionId2", "id2", "timestamp2", "objectName2", "commandName2", "data2", this
             );

    QCOMPARE(( QObject* )update->parent(), ( QObject* )this );
    QCOMPARE( update->sessionId(), QString::fromAscii( "sessionId2" ) );
    QCOMPARE( update->id(), QString::fromAscii( "id2" ) );
    QCOMPARE( update->timestamp(), QString::fromAscii( "timestamp2" ) );
    QCOMPARE( update->objectName(), QString::fromAscii( "objectName2" ) );
    QCOMPARE( update->commandName(), QString::fromAscii( "commandName2" ) );
    QCOMPARE( update->data(), QString::fromAscii( "data2" ) );

    QCOMPARE( update->toString(), xml2 );

    delete update;
}

void kcollaborate::Message::TestUpdate::testConstructorXml1()
{
    kcollaborate::Message::Update *update;
    update = new kcollaborate::Message::Update(
                 "sessionId", kcollaborate::Message::XmlHelper::parse( xml1 )
             );

    QCOMPARE(( QObject* )update->parent(), ( QObject* )NULL );
    QCOMPARE( update->sessionId(), QString::fromAscii( "sessionId" ) );
    QCOMPARE( update->id(), QString::fromAscii( "id" ) );
    QCOMPARE( update->timestamp(), QString::fromAscii( "timestamp" ) );
    QCOMPARE( update->objectName(), QString::fromAscii( "objectName" ) );
    QCOMPARE( update->commandName(), QString::fromAscii( "commandName" ) );
    QCOMPARE( update->data(), QString::fromAscii( "data" ) );

    QCOMPARE( update->toString(), xml1 );

    delete update;
}

void kcollaborate::Message::TestUpdate::testConstructorXml2()
{
    kcollaborate::Message::Update *update;
    update = new kcollaborate::Message::Update(
                 "sessionId2", kcollaborate::Message::XmlHelper::parse( xml2 ), this
             );

    QCOMPARE(( QObject* )update->parent(), ( QObject* )this );
    QCOMPARE( update->sessionId(), QString::fromAscii( "sessionId2" ) );
    QCOMPARE( update->id(), QString::fromAscii( "id2" ) );
    QCOMPARE( update->timestamp(), QString::fromAscii( "timestamp2" ) );
    QCOMPARE( update->objectName(), QString::fromAscii( "objectName2" ) );
    QCOMPARE( update->commandName(), QString::fromAscii( "commandName2" ) );
    QCOMPARE( update->data(), QString::fromAscii( "data2" ) );

    QCOMPARE( update->toString(), xml2 );

    delete update;
}

void kcollaborate::Message::TestUpdate::testConstructorXml3()
{
    kcollaborate::Message::Update *update;
    update = new kcollaborate::Message::Update(
                 "sessionId2", kcollaborate::Message::XmlHelper::parse( xml3 ), this
             );

    QCOMPARE(( QObject* )update->parent(), ( QObject* )this );
    QCOMPARE( update->sessionId(), QString::fromAscii( "sessionId2" ) );
    QCOMPARE( update->id(), QString::fromAscii( "id2" ) );
    QCOMPARE( update->timestamp(), QString::fromAscii( "timestamp2" ) );
    QCOMPARE( update->objectName(), QString::fromAscii( "objectName2" ) );
    QCOMPARE( update->commandName(), QString::fromAscii( "commandName2" ) );
    QCOMPARE( update->data(), QString::fromAscii( "" ) );

    QCOMPARE( update->toString(), xml3 );

    delete update;
}

QTEST_MAIN( kcollaborate::Message::TestUpdate )
#include "TestUpdate.moc"
