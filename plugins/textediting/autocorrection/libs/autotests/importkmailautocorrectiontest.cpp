/*
  SPDX-FileCopyrightText: 2020-2022 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "importkmailautocorrectiontest.h"
#include "../import/importkmailautocorrection.h"
#include <QTest>
QTEST_MAIN(ImportKMailAutocorrectionTest)

ImportKMailAutocorrectionTest::ImportKMailAutocorrectionTest(QObject *parent)
    : QObject(parent)
{
}

void ImportKMailAutocorrectionTest::shouldHaveDefaultValues()
{
    ImportKMailAutocorrection import;
    QVERIFY(import.upperCaseExceptions().isEmpty());
    QVERIFY(import.twoUpperLetterExceptions().isEmpty());
    QVERIFY(import.autocorrectEntries().isEmpty());
    QVERIFY(import.superScriptEntries().isEmpty());

    QCOMPARE(import.typographicSingleQuotes().begin, QChar());
    QCOMPARE(import.typographicSingleQuotes().end, QChar());
    QCOMPARE(import.typographicDoubleQuotes().begin, QChar());
    QCOMPARE(import.typographicDoubleQuotes().end, QChar());

    QCOMPARE(import.maxFindStringLenght(), 0);
    QCOMPARE(import.minFindStringLenght(), 0);
}

void ImportKMailAutocorrectionTest::shouldLoadFile()
{
    const QString originalFile = QLatin1String(AUTOCORRECTION_DATA_DIR) + QStringLiteral("/custom-fr.xml");
    ImportKMailAutocorrection import;
    QString messageError;
    QVERIFY(import.import(originalFile, messageError));
    QCOMPARE(import.typographicSingleQuotes().begin, QStringLiteral("‘"));
    QCOMPARE(import.typographicSingleQuotes().end, QStringLiteral("’"));
    QCOMPARE(import.typographicDoubleQuotes().begin, QStringLiteral("“"));
    QCOMPARE(import.typographicDoubleQuotes().end, QStringLiteral("”"));

    auto resultTwoUpperLetterExceptions = import.twoUpperLetterExceptions();
    QCOMPARE(resultTwoUpperLetterExceptions.count(), 21);

    auto resultUpperCaseExceptions = import.upperCaseExceptions();
    QCOMPARE(resultUpperCaseExceptions.count(), 45);

    auto resultAutocorrectEntries = import.autocorrectEntries();
    QCOMPARE(resultAutocorrectEntries.count(), 1221);
}
