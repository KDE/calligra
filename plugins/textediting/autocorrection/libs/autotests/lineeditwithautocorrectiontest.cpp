/*
  SPDX-FileCopyrightText: 2014-2022 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "lineeditwithautocorrectiontest.h"
#include "../autocorrection.h"
#include "../widgets/lineeditwithautocorrection.h"
#include <QTest>
#include <qtestkeyboard.h>

LineEditWithAutocorrectionTest::LineEditWithAutocorrectionTest() = default;

void LineEditWithAutocorrectionTest::shouldNotAutocorrectWhenDisabled()
{
    LineEditWithAutoCorrection lineedit(nullptr, QStringLiteral("lineeditwithautocorrecttestrc"));
    QHash<QString, QString> entries;
    const QString originalWord = QStringLiteral("FOOFOO");
    const QString replaceWord = QStringLiteral("BLABLA");
    entries.insert(originalWord, replaceWord);
    lineedit.autocorrection()->setAutocorrectEntries(entries);
    lineedit.show();
    QVERIFY(QTest::qWaitForWindowExposed(&lineedit));
    QTest::keyClicks(&lineedit, originalWord);
    QTest::keyClick(&lineedit, ' ');
    QCOMPARE(lineedit.toPlainText(), QString(originalWord + QLatin1Char(' ')));
}

void LineEditWithAutocorrectionTest::shouldReplaceWordWhenExactText()
{
    LineEditWithAutoCorrection lineedit(nullptr, QStringLiteral("lineeditwithautocorrecttestrc"));
    const QString originalWord = QStringLiteral("FOOFOO");
    const QString replaceWord = QStringLiteral("BLABLA");
    QHash<QString, QString> entries;
    entries.insert(originalWord, replaceWord);
    lineedit.autocorrection()->setAutocorrectEntries(entries);
    lineedit.autocorrection()->setEnabledAutoCorrection(true);
    lineedit.autocorrection()->setAdvancedAutocorrect(true);
    lineedit.show();
    QVERIFY(QTest::qWaitForWindowExposed(&lineedit));
    QTest::keyClicks(&lineedit, originalWord);
    QTest::keyClick(&lineedit, ' ');
    QCOMPARE(lineedit.toPlainText(), QString(replaceWord + QLatin1Char(' ')));
}

void LineEditWithAutocorrectionTest::shouldNotReplaceWordWhenInexactText()
{
    LineEditWithAutoCorrection lineedit(nullptr, QStringLiteral("lineeditwithautocorrecttestrc"));
    const QString originalWord = QStringLiteral("FOOFOO");
    const QString replaceWord = QStringLiteral("BLABLA");
    QHash<QString, QString> entries;
    entries.insert(originalWord, replaceWord);
    lineedit.autocorrection()->setAutocorrectEntries(entries);
    lineedit.autocorrection()->setEnabledAutoCorrection(true);
    lineedit.autocorrection()->setAdvancedAutocorrect(true);
    lineedit.show();
    const QString nonExactText = QStringLiteral("BLIBLI");
    QVERIFY(QTest::qWaitForWindowExposed(&lineedit));
    QTest::keyClicks(&lineedit, nonExactText);
    QTest::keyClick(&lineedit, ' ');
    QCOMPARE(lineedit.toPlainText(), QString(nonExactText + QLatin1Char(' ')));
}

void LineEditWithAutocorrectionTest::shouldNotAddTwoSpace()
{
    LineEditWithAutoCorrection lineedit(nullptr, QStringLiteral("lineeditwithautocorrecttestrc"));
    const QString originalWord = QStringLiteral("FOOFOO ");
    lineedit.autocorrection()->setSingleSpaces(true);
    lineedit.autocorrection()->setEnabledAutoCorrection(true);
    lineedit.show();
    QVERIFY(QTest::qWaitForWindowExposed(&lineedit));
    QTest::keyClicks(&lineedit, originalWord);
    QTest::keyClick(&lineedit, ' ');
    QCOMPARE(lineedit.toPlainText(), originalWord);
}

QTEST_MAIN(LineEditWithAutocorrectionTest)
