/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2010 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "testfindmatch.h"

#include <QTest>
#include <QTextCursor>
#include <QTextDocument>

#include "KoFindMatch.h"

Q_DECLARE_METATYPE(QTextDocument *)
Q_DECLARE_METATYPE(QTextCursor)

void TestFindMatch::testBasic()
{
    KoFindMatch match;

    QTextDocument *doc = new QTextDocument("Test Document", this);
    QVariant docVariant = QVariant::fromValue(doc);
    QVERIFY(docVariant.isValid());

    match.setContainer(docVariant);
    QCOMPARE(match.container(), docVariant);
    QCOMPARE(match.container().value<QTextDocument *>(), doc);

    QTextCursor cursor = doc->find("Document");
    QVariant curVariant = QVariant::fromValue(cursor);
    QVERIFY(curVariant.isValid());

    match.setLocation(curVariant);
    QCOMPARE(match.location(), curVariant);
    QCOMPARE(match.location().value<QTextCursor>(), cursor);

    QVERIFY(match.isValid());

    KoFindMatch other(docVariant, curVariant);
    QVERIFY(other.isValid());
    QCOMPARE(other.container(), docVariant);
    QCOMPARE(other.location(), curVariant);
}

void TestFindMatch::testCopyAssign()
{
    KoFindMatch match;

    QTextDocument *doc = new QTextDocument("Test Document", this);
    QVariant docVariant = QVariant::fromValue(doc);
    QTextCursor cursor = doc->find("Document");
    QVariant curVariant = QVariant::fromValue(cursor);

    QVERIFY(docVariant.isValid());
    QVERIFY(curVariant.isValid());

    match.setContainer(docVariant);
    match.setLocation(curVariant);

    KoFindMatch other(match);
    QVERIFY(other.isValid());
    QCOMPARE(other.container(), match.container());
    QCOMPARE(other.location(), match.location());

    KoFindMatch third = match;
    QVERIFY(third.isValid());
    QCOMPARE(third.container(), match.container());
    QCOMPARE(third.location(), match.location());

    QVERIFY(other == match);
}

QTEST_MAIN(TestFindMatch)
