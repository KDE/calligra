/*
  SPDX-FileCopyrightText: 2014-2022 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "richtexteditwithautocorrectiontest.h"
#include "../autocorrection.h"
#include "../widgets/richtexteditwithautocorrection.h"
#include <QTest>
#include <qtestkeyboard.h>

RichTextEditWithAutoCorrectionTest::RichTextEditWithAutoCorrectionTest() = default;

void RichTextEditWithAutoCorrectionTest::shouldNotAutocorrectWhenDisabled()
{
    PimCommon::RichTextEditWithAutoCorrection richtext(nullptr);
    QHash<QString, QString> entries;
    const QString originalWord = QStringLiteral("FOOFOO");
    const QString replaceWord = QStringLiteral("BLABLA");
    entries.insert(originalWord, replaceWord);
    richtext.autocorrection()->setAutocorrectEntries(entries);
    richtext.show();
    QVERIFY(QTest::qWaitForWindowExposed(&richtext));
    QTest::keyClicks(&richtext, originalWord);
    QTest::keyClick(&richtext, ' ');
    QCOMPARE(richtext.toPlainText(), QString(originalWord + QLatin1Char(' ')));
}

void RichTextEditWithAutoCorrectionTest::shouldReplaceWordWhenExactText()
{
    PimCommon::RichTextEditWithAutoCorrection richtext(nullptr);
    const QString originalWord = QStringLiteral("FOOFOO");
    const QString replaceWord = QStringLiteral("BLABLA");
    QHash<QString, QString> entries;
    entries.insert(originalWord, replaceWord);
    richtext.autocorrection()->setAutocorrectEntries(entries);
    richtext.autocorrection()->setEnabledAutoCorrection(true);
    richtext.autocorrection()->setAdvancedAutocorrect(true);
    richtext.show();
    QVERIFY(QTest::qWaitForWindowExposed(&richtext));
    QTest::keyClicks(&richtext, originalWord);
    QTest::keyClick(&richtext, ' ');
    QCOMPARE(richtext.toPlainText(), QString(replaceWord + QLatin1Char(' ')));
}

void RichTextEditWithAutoCorrectionTest::shouldNotReplaceWordWhenInexactText()
{
    PimCommon::RichTextEditWithAutoCorrection richtext(nullptr);
    const QString originalWord = QStringLiteral("FOOFOO");
    const QString replaceWord = QStringLiteral("BLABLA");
    QHash<QString, QString> entries;
    entries.insert(originalWord, replaceWord);
    richtext.autocorrection()->setAutocorrectEntries(entries);
    richtext.autocorrection()->setEnabledAutoCorrection(true);
    richtext.autocorrection()->setAdvancedAutocorrect(true);
    richtext.show();
    const QString nonExactText = QStringLiteral("BLIBLI");
    QVERIFY(QTest::qWaitForWindowExposed(&richtext));
    QTest::keyClicks(&richtext, nonExactText);
    QTest::keyClick(&richtext, ' ');
    QCOMPARE(richtext.toPlainText(), QString(nonExactText + QLatin1Char(' ')));
}

void RichTextEditWithAutoCorrectionTest::shouldReplaceWhenPressEnter()
{
    PimCommon::RichTextEditWithAutoCorrection richtext(nullptr);
    const QString originalWord = QStringLiteral("FOOFOO");
    const QString replaceWord = QStringLiteral("BLABLA");
    QHash<QString, QString> entries;
    entries.insert(originalWord, replaceWord);
    richtext.autocorrection()->setAutocorrectEntries(entries);
    richtext.autocorrection()->setEnabledAutoCorrection(true);
    richtext.autocorrection()->setAdvancedAutocorrect(true);
    richtext.show();
    QVERIFY(QTest::qWaitForWindowExposed(&richtext));
    QTest::keyClicks(&richtext, originalWord);
    QTest::keyPress(&richtext, Qt::Key_Enter);
    QCOMPARE(richtext.toPlainText(), QString(replaceWord + QLatin1Char('\n')));
}

void RichTextEditWithAutoCorrectionTest::shouldReplaceWhenPressReturn()
{
    PimCommon::RichTextEditWithAutoCorrection richtext(nullptr);
    const QString originalWord = QStringLiteral("FOOFOO");
    const QString replaceWord = QStringLiteral("BLABLA");
    QHash<QString, QString> entries;
    entries.insert(originalWord, replaceWord);
    richtext.autocorrection()->setAutocorrectEntries(entries);
    richtext.autocorrection()->setEnabledAutoCorrection(true);
    richtext.autocorrection()->setAdvancedAutocorrect(true);
    richtext.show();
    QVERIFY(QTest::qWaitForWindowExposed(&richtext));
    QTest::keyClicks(&richtext, originalWord);
    QTest::keyPress(&richtext, Qt::Key_Return);
    QCOMPARE(richtext.toPlainText(), QString(replaceWord + QLatin1Char('\n')));
}

QTEST_MAIN(RichTextEditWithAutoCorrectionTest)
