/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007-2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "TestListStyle.h"

#include "KoTextBlockBorderData.h"
#include "KoTextDocument.h"
#include "styles/KoListLevelProperties.h"
#include "styles/KoListStyle.h"
#include "styles/KoParagraphStyle.h"
#include "styles/KoStyleManager.h"

#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextList>

#include <QTest>

void TestListStyle::testListStyle()
{
    KoListStyle ls;
    KoListLevelProperties llp = ls.levelProperties(2);
    QCOMPARE(llp.level(), 2);

    llp.setNumberFormat(KoOdfNumberDefinition::AlphabeticLowerCase);
    KoListLevelProperties llp2 = ls.levelProperties(2);
    QVERIFY(llp2.numberFormat() != llp.numberFormat());

    ls.setLevelProperties(llp);
    QCOMPARE(llp.level(), 2);
    QCOMPARE(llp.numberFormat(), KoOdfNumberDefinition::AlphabeticLowerCase);

    llp = ls.levelProperties(2);
    QCOMPARE(llp.level(), 2);
    QCOMPARE(llp.numberFormat(), KoOdfNumberDefinition::AlphabeticLowerCase);

    QTextDocument doc;
    KoTextDocument kodoc(&doc);
    kodoc.setStyleManager(new KoStyleManager(nullptr));
    QTextCursor cursor(&doc);
    cursor.insertText("foo\nbar\nBaz\n");
    QTextBlock block = doc.begin();
    ls.applyStyle(block, 2);
    QVERIFY(block.textList());
    QTextList *textList = block.textList();
    QTextListFormat format = textList->format();
    QCOMPARE(format.intProperty(KoListStyle::NumberFormat), (int)(KoOdfNumberDefinition::AlphabeticLowerCase));

    block = block.next();
    QVERIFY(block.isValid());
    ls.applyStyle(block, 2);
    QVERIFY(block.textList());
    QCOMPARE(block.textList(), textList);

    ls.applyStyle(block, 10); // should set the properties of the only one that is set, level 1
    QVERIFY(block.textList());
    textList = block.textList();
    format = textList->format();
    QCOMPARE(format.intProperty(KoListStyle::NumberFormat), (int)(KoOdfNumberDefinition::AlphabeticLowerCase));

    // getting a properties without setting it doesn't change the list.
    KoListLevelProperties l4 = ls.levelProperties(4);
    QCOMPARE(l4.level(), 4);
    QCOMPARE(l4.displayLevel(), 1); // default
    l4.setDisplayLevel(3);
    QCOMPARE(l4.displayLevel(), 3);
    QCOMPARE(ls.hasLevelProperties(4), false);

    KoListLevelProperties anotherL4 = ls.levelProperties(4);
    QCOMPARE(anotherL4.level(), 4);
    QCOMPARE(anotherL4.displayLevel(), 1); // default
    QCOMPARE(ls.hasLevelProperties(4), false);

    QCOMPARE(ls.hasLevelProperties(5), false);
    // new levels are a copy of the existing level.
    KoListLevelProperties l5 = ls.levelProperties(5);
    QCOMPARE(l5.displayLevel(), 1);
    QCOMPARE(l5.numberFormat(), KoOdfNumberDefinition::AlphabeticLowerCase);
    QCOMPARE(l5.indent(), 0.);
}

QTEST_MAIN(TestListStyle)
