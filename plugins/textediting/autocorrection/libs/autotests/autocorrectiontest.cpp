/*
  SPDX-FileCopyrightText: 2014-2022 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "autocorrectiontest.h"
#include "../autocorrection.h"
#include <QStandardPaths>
#include <QTest>
#include <QTextDocument>

#ifndef Q_OS_WIN
void initLocale()
{
    setenv("LC_ALL", "en_US.utf-8", 1);
}

Q_CONSTRUCTOR_FUNCTION(initLocale)
#endif

AutoCorrectionTest::AutoCorrectionTest()
{
    QStandardPaths::setTestModeEnabled(true);
    mConfig = KSharedConfig::openConfig(QStringLiteral("autocorrectiontestrc"));
}

AutoCorrectionTest::~AutoCorrectionTest() = default;

void AutoCorrectionTest::shouldHaveDefaultValue()
{
    QEXPECT_FAIL("", "Adapt to calligra config", Continue);
    QVERIFY(false);
#if 0
    AutoCorrection autocorrection;
    QVERIFY(!autocorrection.isEnabledAutoCorrection());
    QVERIFY(!autocorrection.isUppercaseFirstCharOfSentence());
    QVERIFY(!autocorrection.isFixTwoUppercaseChars());
    QVERIFY(!autocorrection.isSingleSpaces());
    QVERIFY(!autocorrection.isAutoFractions());
    QVERIFY(!autocorrection.isCapitalizeWeekDays());
    QVERIFY(!autocorrection.isReplaceDoubleQuotes());
    QVERIFY(!autocorrection.isReplaceSingleQuotes());
    QVERIFY(!autocorrection.isAdvancedAutocorrect());
    QVERIFY(!autocorrection.isAutoFormatUrl());
    QVERIFY(!autocorrection.isAutoBoldUnderline());
    QVERIFY(!autocorrection.isSuperScript());
    QVERIFY(!autocorrection.isAddNonBreakingSpace());
#endif
}

void AutoCorrectionTest::shouldRestoreValue()
{
    AutoCorrection autocorrection;
    // TODO
}

void AutoCorrectionTest::shouldUpperCaseFirstCharOfSentence()
{
    AutoCorrection autocorrection;
    autocorrection.setEnabledAutoCorrection(true);
    autocorrection.setUppercaseFirstCharOfSentence(true);

    // Uppercase here.
    QTextDocument doc;
    QString text = QStringLiteral("foo");
    doc.setPlainText(text);
    int position = text.length();
    autocorrection.autocorrect(false, doc, position);
    QCOMPARE(doc.toPlainText(), QStringLiteral("Foo"));

    // IT's not first char -> not uppercase
    text = QStringLiteral(" foo");
    doc.setPlainText(text);
    position = text.length();
    autocorrection.autocorrect(false, doc, position);
    QCOMPARE(doc.toPlainText(), text);

    // It's already uppercase
    text = QStringLiteral("Foo");
    doc.setPlainText(text);
    position = text.length();
    autocorrection.autocorrect(false, doc, position);
    QCOMPARE(doc.toPlainText(), text);

    // Word is after a ". "
    text = QStringLiteral("Foo. foo");
    doc.setPlainText(text);
    position = text.length();
    autocorrection.autocorrect(false, doc, position);
    QCOMPARE(doc.toPlainText(), QStringLiteral("Foo. Foo"));
    QCOMPARE(position, text.length());
}

void AutoCorrectionTest::shouldFixTwoUpperCaseChars()
{
    AutoCorrection autocorrection;
    autocorrection.setEnabledAutoCorrection(true);
    autocorrection.setFixTwoUppercaseChars(true);

    // Remove two uppercases
    QTextDocument doc;
    QString text = QStringLiteral("FOo");
    doc.setPlainText(text);
    int position = text.length();
    autocorrection.autocorrect(false, doc, position);
    QCOMPARE(doc.toPlainText(), QStringLiteral("Foo"));

    // There is not two uppercase
    text = QStringLiteral("foo");
    doc.setPlainText(text);
    position = text.length();
    autocorrection.autocorrect(false, doc, position);
    QCOMPARE(doc.toPlainText(), text);

    text = QStringLiteral("Foo");
    doc.setPlainText(text);
    position = text.length();
    autocorrection.autocorrect(false, doc, position);
    QCOMPARE(doc.toPlainText(), text);

    // There is a uppercase word
    text = QStringLiteral("FOO");
    doc.setPlainText(text);
    position = text.length();
    autocorrection.autocorrect(false, doc, position);
    QCOMPARE(doc.toPlainText(), text);

    // Exclude 2 upper letter
    text = QStringLiteral("ABc");
    doc.setPlainText(text);
    position = text.length();
    autocorrection.autocorrect(false, doc, position);
    QCOMPARE(doc.toPlainText(), QStringLiteral("Abc"));

    QSet<QString> exception;
    exception.insert(QStringLiteral("ABc"));
    autocorrection.setTwoUpperLetterExceptions(exception);
    text = QStringLiteral("ABc");
    doc.setPlainText(text);
    position = text.length();
    autocorrection.autocorrect(false, doc, position);
    QCOMPARE(doc.toPlainText(), text);
}

void AutoCorrectionTest::shouldReplaceSingleQuote()
{
    AutoCorrection autocorrection;
    autocorrection.setEnabledAutoCorrection(true);
    autocorrection.setReplaceSingleQuotes(true);
    AutoCorrection::TypographicQuotes simpleQuote;
    simpleQuote.begin = QLatin1Char('A');
    simpleQuote.end = QLatin1Char('B');

    autocorrection.setTypographicSingleQuotes(simpleQuote);

    QTextDocument doc;
    QString text = QStringLiteral("sss");
    doc.setPlainText(QStringLiteral("'") + text);
    int position = text.length();
    autocorrection.autocorrect(false, doc, position);
    QCOMPARE(doc.toPlainText(), QString(simpleQuote.begin + text));

    text = QStringLiteral("sss");
    doc.setPlainText(text + QStringLiteral("'"));
    position = text.length();
    autocorrection.autocorrect(false, doc, position);
    QCOMPARE(doc.toPlainText(), QString(text + simpleQuote.end));

    text = QStringLiteral("sss");
    doc.setPlainText(QStringLiteral("'") + text + QStringLiteral("'"));
    position = text.length();
    autocorrection.autocorrect(false, doc, position);
    QCOMPARE(doc.toPlainText(), QString(simpleQuote.begin + text + simpleQuote.end));
}

void AutoCorrectionTest::shouldReplaceDoubleQuote()
{
    AutoCorrection autocorrection;
    autocorrection.setEnabledAutoCorrection(true);
    autocorrection.setReplaceDoubleQuotes(true);
    AutoCorrection::TypographicQuotes doubleQuote;
    doubleQuote.begin = QLatin1Char('A');
    doubleQuote.end = QLatin1Char('B');

    autocorrection.setTypographicDoubleQuotes(doubleQuote);

    QTextDocument doc;
    QString text = QStringLiteral("sss");

    doc.setPlainText(QLatin1Char('"') + text);
    int position = text.length();
    autocorrection.autocorrect(false, doc, position);
    QCOMPARE(doc.toPlainText(), QString(doubleQuote.begin + text));

    text = QStringLiteral("sss");
    doc.setPlainText(text + QStringLiteral("\""));
    position = text.length();
    autocorrection.autocorrect(false, doc, position);
    QCOMPARE(doc.toPlainText(), QString(text + doubleQuote.end));

    text = QStringLiteral("sss");
    doc.setPlainText(QStringLiteral("\"") + text + QStringLiteral("\""));
    position = text.length();
    autocorrection.autocorrect(false, doc, position);
    QCOMPARE(doc.toPlainText(), QString(doubleQuote.begin + text + doubleQuote.end));
}

void AutoCorrectionTest::shouldNotReplaceUppercaseLetter()
{
    AutoCorrection autocorrection;
    autocorrection.setEnabledAutoCorrection(true);
    autocorrection.setFixTwoUppercaseChars(true);
    QSet<QString> exceptions;
    exceptions.insert(QStringLiteral("ABc"));
    autocorrection.setTwoUpperLetterExceptions(exceptions);

    QTextDocument doc;
    QString text = QStringLiteral("foo ABc");
    doc.setPlainText(text);
    int position = text.length();
    autocorrection.autocorrect(false, doc, position);
    QCOMPARE(doc.toPlainText(), text);
}

void AutoCorrectionTest::shouldReplaceToTextFormat()
{
    AutoCorrection autocorrection;
    autocorrection.setEnabledAutoCorrection(true);
    autocorrection.setAutoBoldUnderline(true);

    QTextDocument doc;
    // We don't use html => don't change it.
    QString text = QStringLiteral("*foo*");
    doc.setHtml(text);
    int position = text.length();
    autocorrection.autocorrect(false, doc, position);
    QCOMPARE(doc.toPlainText(), text);

    QString result = QStringLiteral("foo");
    doc.setHtml(text);
    position = text.length();
    autocorrection.autocorrect(true, doc, position);
    QCOMPARE(doc.toPlainText(), result);

    text = QStringLiteral("*foo*");
    doc.setHtml(text);
    position = text.length();
    autocorrection.autocorrect(true, doc, position);
    QCOMPARE(doc.toPlainText(), result);
    QTextCursor cursor(&doc);
    cursor.setPosition(2);
    QTextCharFormat charFormat = cursor.charFormat();
    QCOMPARE(charFormat.font().underline(), false);
    QCOMPARE(charFormat.font().bold(), true);
    QCOMPARE(charFormat.font().strikeOut(), false);

    text = QStringLiteral("_foo_");
    doc.setHtml(text);
    position = text.length();
    autocorrection.autocorrect(true, doc, position);
    QCOMPARE(doc.toPlainText(), result);
    cursor = QTextCursor(&doc);
    cursor.setPosition(2);
    charFormat = cursor.charFormat();
    QCOMPARE(charFormat.font().underline(), true);
    QCOMPARE(charFormat.font().bold(), false);
    QCOMPARE(charFormat.font().strikeOut(), false);

    text = QStringLiteral("-foo-");
    doc.setHtml(text);
    position = text.length();
    autocorrection.autocorrect(true, doc, position);
    QCOMPARE(doc.toPlainText(), result);
    cursor = QTextCursor(&doc);
    cursor.setPosition(2);
    charFormat = cursor.charFormat();
    QCOMPARE(charFormat.font().underline(), false);
    QCOMPARE(charFormat.font().bold(), false);
    QCOMPARE(charFormat.font().strikeOut(), true);

    // Don't convert it.
    text = QStringLiteral("-foo1");
    doc.setHtml(text);
    position = text.length();
    autocorrection.autocorrect(true, doc, position);
    QCOMPARE(doc.toPlainText(), text);

    text = QStringLiteral("*foo* blabla");
    position = 5;
    doc.setHtml(text);
    autocorrection.autocorrect(true, doc, position);
    result = QStringLiteral("foo blabla");
    QCOMPARE(doc.toPlainText(), result);
    QCOMPARE(position, 3);

    // TODO QCOMPARE(doc.toHtml(), text);
}

void AutoCorrectionTest::shouldReplaceAutoFraction()
{
    AutoCorrection autocorrection;
    autocorrection.setEnabledAutoCorrection(true);
    autocorrection.setAutoFractions(true);

    QTextDocument doc;
    QString text = QStringLiteral("1/2");
    doc.setPlainText(text);
    int position = text.length();
    autocorrection.autocorrect(false, doc, position);

    QCOMPARE(doc.toPlainText(), QStringLiteral("½"));

    QString suffix = QStringLiteral(" after");
    text = QStringLiteral("1/2");
    position = 3;
    text += suffix;
    doc.setPlainText(text);
    autocorrection.autocorrect(false, doc, position);
    QCOMPARE(doc.toPlainText(), QString(QStringLiteral("½") + suffix));
    QCOMPARE(position, 1);
}

void AutoCorrectionTest::shouldNotAddSpaceWhenWeAlreadyHaveASpace()
{
    AutoCorrection autocorrection;
    autocorrection.setEnabledAutoCorrection(true);
    autocorrection.setSingleSpaces(true);
    QTextDocument doc;
    // We already a space => don't allow to add more
    QString text = QStringLiteral("FOO ");
    doc.setPlainText(text);
    int position = text.length();
    bool result = autocorrection.autocorrect(false, doc, position);
    QCOMPARE(result, false);

    // We can add a space
    text = QStringLiteral("FOO");
    doc.setPlainText(text);
    position = text.length();
    result = autocorrection.autocorrect(false, doc, position);
    QCOMPARE(result, true);

    // We have a space => don't add it.
    text = QStringLiteral("FOO ");
    position = text.length();
    QString fullText = text + QStringLiteral("FOO");
    doc.setPlainText(fullText);
    result = autocorrection.autocorrect(false, doc, position);
    QCOMPARE(result, false);
}

void AutoCorrectionTest::shouldAutocorrectWord()
{
    AutoCorrection autocorrection;
    autocorrection.setEnabledAutoCorrection(true);
    autocorrection.setAdvancedAutocorrect(true);

    QTextDocument doc;
    // No changes
    QString text = QStringLiteral("FOO");
    doc.setPlainText(text);
    int position = text.length();
    int oldPosition = position;
    autocorrection.autocorrect(false, doc, position);
    QCOMPARE(doc.toPlainText(), text);
    QCOMPARE(position, oldPosition);

    // Convert word
    QHash<QString, QString> entries;
    const QString convertWord = QStringLiteral("BLABLA");
    entries.insert(text, convertWord);
    autocorrection.setAutocorrectEntries(entries);
    text = QStringLiteral("FOO");
    doc.setPlainText(text);
    position = text.length();
    autocorrection.autocorrect(false, doc, position);
    QCOMPARE(doc.toPlainText(), convertWord);
    QCOMPARE(position, convertWord.length());

    QString suffix = QStringLiteral(" TOTO");
    text = QStringLiteral("FOO");
    position = text.length();
    text += suffix;
    doc.setPlainText(text);
    autocorrection.autocorrect(false, doc, position);
    QCOMPARE(doc.toPlainText(), QString(convertWord + suffix));
    // FIXME ? QCOMPARE(position, convertWord.length());
}

void AutoCorrectionTest::shouldNotUpperCaseFirstCharOfSentence()
{
    AutoCorrection autocorrection;
    autocorrection.setEnabledAutoCorrection(true);
    autocorrection.setUppercaseFirstCharOfSentence(true);
    QSet<QString> lst;
    lst.insert(QStringLiteral("Foo."));
    autocorrection.setUpperCaseExceptions(lst);

    // Uppercase here.
    QTextDocument doc;
    QString text = QStringLiteral("foo. blabla Foo. tt");
    doc.setPlainText(text);
    int position = text.length();
    autocorrection.autocorrect(false, doc, position);
    QCOMPARE(doc.toPlainText(), text);

    autocorrection.setUpperCaseExceptions(QSet<QString>());
    doc.setPlainText(text);
    position = text.length();
    autocorrection.autocorrect(false, doc, position);
    QString result = QStringLiteral("foo. blabla Foo. Tt");
    QCOMPARE(doc.toPlainText(), result);
}

using mapAutoCorrect = QHash<QString, QString>;
Q_DECLARE_METATYPE(mapAutoCorrect)

void AutoCorrectionTest::shouldAutocorrectMultiWord_data()
{
    QTest::addColumn<QString>("originalString");
    QTest::addColumn<QString>("convertedString");
    QTest::addColumn<mapAutoCorrect>("convertStringHash");

    mapAutoCorrect map;
    map.insert(QStringLiteral("boo"), QStringLiteral("bla"));
    QTest::newRow("simpleReplace") << QStringLiteral("boo") << QStringLiteral("bla") << map;

    map.clear();
    map.insert(QStringLiteral("a boo"), QStringLiteral("b bla"));
    QTest::newRow("multiword") << QStringLiteral("a boo") << QStringLiteral("b bla") << map;

    map.clear();
    map.insert(QStringLiteral("a boo"), QStringLiteral("b bla"));
    map.insert(QStringLiteral("a booeeeeeeeeeeeeeeeeeee"), QStringLiteral("b blaeee"));
    QTest::newRow("multiword-2") << QStringLiteral("toto. a boo") << QStringLiteral("toto. b bla") << map;

    map.clear();
    QTest::newRow("multiword-2 without replace") << QStringLiteral("toto. a boo") << QStringLiteral("toto. a boo") << map;

    map.clear();
    map.insert(QStringLiteral("a boo"), QStringLiteral("b bla"));
    QTest::newRow("multiword-2 with uppercase") << QStringLiteral("toto. A boo") << QStringLiteral("toto. B bla") << map;

    map.clear();
    map.insert(QStringLiteral("a boo1111111111"), QStringLiteral("b bla"));
    QTest::newRow("multiword-3") << QStringLiteral("a boo") << QStringLiteral("a boo") << map;

    map.clear();
    map.insert(QStringLiteral("boo"), QStringLiteral("Bla"));
    QTest::newRow("withuppercase") << QStringLiteral("Boo") << QStringLiteral("Bla") << map;

    map.clear();
    map.insert(QStringLiteral("boo"), QStringLiteral("bla"));
    QTest::newRow("withuppercase-2") << QStringLiteral("Boo") << QStringLiteral("Bla") << map;

    map.clear();
    map.insert(QStringLiteral("boo"), QStringLiteral("Bla"));
    QTest::newRow("withuppercase-3") << QStringLiteral("Boo") << QStringLiteral("Bla") << map;

    map.clear();
    map.insert(QStringLiteral("boo"), QStringLiteral("Bla"));
    QTest::newRow("withuppercase-4") << QStringLiteral("boo") << QStringLiteral("bla") << map;

    map.clear();
    map.insert(QStringLiteral("booooo"), QStringLiteral("bla"));
    QTest::newRow("nofindtext") << QStringLiteral("boo") << QStringLiteral("boo") << map;

    map.clear();
    map.insert(QStringLiteral("boosss"), QStringLiteral("Blasss"));
    map.insert(QStringLiteral("boo"), QStringLiteral("bla"));
    QTest::newRow("withponct") << QStringLiteral("boo!") << QStringLiteral("bla!") << map;

    map.clear();
    map.insert(QStringLiteral("boosss"), QStringLiteral("Blasss"));
    map.insert(QStringLiteral("boo"), QStringLiteral("bla"));
    QTest::newRow("withponct-2") << QStringLiteral("lolo. boo!") << QStringLiteral("lolo. bla!") << map;

    map.clear();
    map.insert(QStringLiteral("boo"), QStringLiteral("bla"));
    QTest::newRow("dontreplaceinpieceofword") << QStringLiteral("voitureboo") << QStringLiteral("voitureboo") << map;

    map.clear();
    map.insert(QStringLiteral("boo"), QStringLiteral("bla"));
    map.insert(QStringLiteral("booOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOo"), QStringLiteral("bla"));
    QTest::newRow("dontreplaceall") << QStringLiteral("Boo boo boo") << QStringLiteral("Boo boo bla") << map;
}

void AutoCorrectionTest::shouldAutocorrectMultiWord()
{
    QFETCH(QString, originalString);
    QFETCH(QString, convertedString);
    QFETCH(mapAutoCorrect, convertStringHash);

    AutoCorrection autocorrection;
    autocorrection.setEnabledAutoCorrection(true);
    autocorrection.setAdvancedAutocorrect(true);
    autocorrection.setAutocorrectEntries(convertStringHash);

    QTextDocument doc;
    doc.setPlainText(originalString);
    int position = originalString.length();
    autocorrection.autocorrect(false, doc, position);
    QCOMPARE(doc.toPlainText(), convertedString);
}

void AutoCorrectionTest::shouldAddNonBreakingSpace_data()
{
    QTest::addColumn<QString>("originalString");
    QTest::addColumn<QString>("convertedString");
    QTest::addColumn<QString>("language");
    QTest::addColumn<bool>("enableAddNonBreakingSpace");
    QTest::newRow("convert1") << QStringLiteral("boo !") << QStringLiteral("boob!") << QStringLiteral("fr") << true;
    QTest::newRow("disable") << QStringLiteral("boo !") << QStringLiteral("boo !") << QStringLiteral("fr") << false;
    QTest::newRow("nonchanges") << QStringLiteral("boo") << QStringLiteral("boo") << QStringLiteral("fr") << true;
    QTest::newRow("convert2") << QStringLiteral("boo ;") << QStringLiteral("boob;") << QStringLiteral("fr") << true;
    QTest::newRow("convert3") << QStringLiteral("boo ?") << QStringLiteral("boob?") << QStringLiteral("fr") << true;
    QTest::newRow("convert4") << QStringLiteral("boo :") << QStringLiteral("boob:") << QStringLiteral("fr") << true;
    QTest::newRow("nonfrenchlanguage") << QStringLiteral("boo :") << QStringLiteral("boo :") << QStringLiteral("ge") << true;
    QTest::newRow("onecharacter") << QStringLiteral(":") << QStringLiteral(":") << QStringLiteral("fr") << true;
    QTest::newRow("onecharacter2") << QStringLiteral(" ") << QStringLiteral(" ") << QStringLiteral("fr") << true;
    QTest::newRow("percentage") << QStringLiteral("50 %") << QStringLiteral("50b%") << QStringLiteral("fr") << true;
    QTest::newRow("degrees") << QStringLiteral("50 °C") << QStringLiteral("50b°C") << QStringLiteral("fr") << true;
    QTest::newRow("simplespace") << QStringLiteral(" ") << QStringLiteral(" ") << QStringLiteral("fr") << true;
}

void AutoCorrectionTest::shouldAddNonBreakingSpace()
{
    QFETCH(QString, originalString);
    QFETCH(QString, convertedString);
    QFETCH(QString, language);
    QFETCH(bool, enableAddNonBreakingSpace);

    AutoCorrection autocorrection;
    autocorrection.setEnabledAutoCorrection(true);
    autocorrection.setAddNonBreakingSpace(enableAddNonBreakingSpace);
    autocorrection.setLanguage(language);
    autocorrection.setNonBreakingSpace(QChar(QLatin1Char('b')));

    QTextDocument doc;
    doc.setPlainText(originalString);
    int position = originalString.length();
    autocorrection.autocorrect(false, doc, position);
    QCOMPARE(doc.toPlainText(), convertedString);
}

void AutoCorrectionTest::shouldReplaceWithMultiOption_data()
{
    QTest::addColumn<QString>("originalString");
    QTest::addColumn<QString>("convertedString");
    QTest::addColumn<mapAutoCorrect>("convertStringHash");

    QTest::addColumn<bool>("enable");
    QTest::addColumn<bool>("uppercaseFirstCharOfSentence");
    QTest::addColumn<bool>("advancedAutocorrect");
    QTest::addColumn<bool>("fixtoouppercase");
    mapAutoCorrect map;
    map.insert(QStringLiteral("boo"), QStringLiteral("bla"));

    QTest::newRow("disable") << QStringLiteral("Boo boo boo") << QStringLiteral("Boo boo boo") << map << false << false << false << false;
    QTest::newRow("enablebutdisablealloptions") << QStringLiteral("Boo boo boo") << QStringLiteral("Boo boo boo") << map << true << false << false << false;
    QTest::newRow("enableandenableuppercase") << QStringLiteral("Boo boo boo") << QStringLiteral("Boo boo boo") << map << true << true << false << false;
    QTest::newRow("enableandenableuppercaseandadvanced")
        << QStringLiteral("Boo boo boo") << QStringLiteral("Boo boo bla") << map << true << true << true << false;

    QTest::newRow("enableandenableuppercaseandadvanced-2")
        << QStringLiteral("Boo boo. boo") << QStringLiteral("Boo boo. Bla") << map << true << true << true << false;
    QTest::newRow("enableandenableuppercaseandadvanced-3")
        << QStringLiteral("blablobli") << QStringLiteral("Blablobli") << map << true << true << true << false;
    QTest::newRow("enableandenableuppercaseandadvanced-4")
        << QStringLiteral("blablobli. foo") << QStringLiteral("blablobli. Foo") << map << true << true << true << false;

    QTest::newRow("enableandenablefixtouppercase") << QStringLiteral("Boo boo. BOo") << QStringLiteral("Boo boo. Boo") << map << true << true << false << true;
    QTest::newRow("enableandenablefixtouppercase-2") << QStringLiteral("Boo BOo") << QStringLiteral("Boo Boo") << map << true << true << false << true;
    // TODO add more
}

void AutoCorrectionTest::shouldReplaceWithMultiOption()
{
    QFETCH(QString, originalString);
    QFETCH(QString, convertedString);
    QFETCH(mapAutoCorrect, convertStringHash);
    QFETCH(bool, enable);
    QFETCH(bool, uppercaseFirstCharOfSentence);
    QFETCH(bool, advancedAutocorrect);
    QFETCH(bool, fixtoouppercase);

    AutoCorrection autocorrection;
    autocorrection.setEnabledAutoCorrection(enable);
    autocorrection.setAdvancedAutocorrect(advancedAutocorrect);
    autocorrection.setAutocorrectEntries(convertStringHash);
    autocorrection.setUppercaseFirstCharOfSentence(uppercaseFirstCharOfSentence);
    autocorrection.setFixTwoUppercaseChars(fixtoouppercase);

    QTextDocument doc;
    doc.setPlainText(originalString);
    int position = originalString.length();
    autocorrection.autocorrect(false, doc, position);
    QCOMPARE(doc.toPlainText(), convertedString);
}

void AutoCorrectionTest::shouldAddNonBreakingSpaceBeforeAfterQuote()
{
    AutoCorrection autocorrection;
    autocorrection.setEnabledAutoCorrection(true);
    autocorrection.setReplaceDoubleQuotes(true);
    autocorrection.setReplaceSingleQuotes(true);
    autocorrection.setLanguage(QStringLiteral("fr"));
    autocorrection.setAddNonBreakingSpace(true);
    // TODO fix me verify why it doesn't use no breaking space
    const QChar nbsp = QChar(/*QChar::Nbsp*/ QLatin1Char('b'));
    autocorrection.setNonBreakingSpace(nbsp);

    AutoCorrection::TypographicQuotes doubleQuote;
    doubleQuote.begin = QLatin1Char('A');
    doubleQuote.end = QLatin1Char('B');
    autocorrection.setTypographicDoubleQuotes(doubleQuote);

    AutoCorrection::TypographicQuotes simpleQuote;
    simpleQuote.begin = QLatin1Char('A');
    simpleQuote.end = QLatin1Char('B');

    autocorrection.setTypographicSingleQuotes(simpleQuote);

    QTextDocument doc;
    QString text = QStringLiteral("sss");

    doc.setPlainText(QLatin1Char('"') + text);
    int position = text.length();
    autocorrection.autocorrect(false, doc, position);

    QCOMPARE(doc.toPlainText(), QString(doubleQuote.begin + nbsp + text));

    text = QStringLiteral("sss");
    doc.setPlainText(text + QStringLiteral("\""));
    position = text.length();
    autocorrection.autocorrect(false, doc, position);
    QCOMPARE(doc.toPlainText(), QString(text + nbsp + doubleQuote.end));

    // Simple quote
    text = QStringLiteral("sss");
    doc.setPlainText(text + QStringLiteral("\'"));
    position = text.length();
    autocorrection.autocorrect(false, doc, position);
    QCOMPARE(doc.toPlainText(), QString(text + nbsp + simpleQuote.end));

    text = QStringLiteral("sss");
    doc.setPlainText(QStringLiteral("\"") + text + QStringLiteral("\""));
    position = text.length();
    autocorrection.autocorrect(false, doc, position);
    QCOMPARE(doc.toPlainText(), QString(doubleQuote.begin + nbsp + text + nbsp + doubleQuote.end));

    // Simple quote
    text = QStringLiteral("sss");
    doc.setPlainText(QStringLiteral("\'") + text + QStringLiteral("\'"));
    position = text.length();
    autocorrection.autocorrect(false, doc, position);
    QCOMPARE(doc.toPlainText(), QString(simpleQuote.begin + nbsp + text + nbsp + simpleQuote.end));

    text = QLatin1Char('(');
    doc.setPlainText(QStringLiteral("\"") + text + QStringLiteral("\""));
    position = text.length();
    autocorrection.autocorrect(false, doc, position);
    QCOMPARE(doc.toPlainText(), QString(doubleQuote.begin + nbsp + text + nbsp + doubleQuote.end));
}

void AutoCorrectionTest::shouldLoadSaveAutocorrection_data()
{
    QTest::addColumn<QString>("filename");
    QTest::newRow("empty") << QStringLiteral("empty");
    QTest::newRow("custom-fr") << QStringLiteral("custom-fr");
}

void AutoCorrectionTest::shouldLoadSaveAutocorrection()
{
    QFETCH(QString, filename);
    AutoCorrection autocorrection;
    AutoCorrection autocorrectionReference;
    const QString originalFile = QLatin1String(AUTOCORRECTION_DATA_DIR) + QLatin1Char('/') + filename + QStringLiteral(".xml");
    const QString refFile = QLatin1String(AUTOCORRECTION_DATA_DIR) + QLatin1Char('/') + filename + QStringLiteral("-ref.xml");
    const QString generatedFile = QLatin1String(AUTOCORRECTION_DATA_DIR) + QLatin1Char('/') + filename + QStringLiteral("-generated.xml");

    autocorrection.loadGlobalFileName(originalFile, true);
    autocorrection.writeAutoCorrectionXmlFile(generatedFile);

    // make sure the xml is valid and we can feed ourself with our own generated file
    autocorrection.loadGlobalFileName(generatedFile, true);

    autocorrectionReference.loadGlobalFileName(refFile, true);

    QCOMPARE(autocorrection.autocorrectEntries(), autocorrectionReference.autocorrectEntries());
    QCOMPARE(autocorrection.upperCaseExceptions(), autocorrectionReference.upperCaseExceptions());
    QCOMPARE(autocorrection.twoUpperLetterExceptions(), autocorrectionReference.twoUpperLetterExceptions());
}

QTEST_MAIN(AutoCorrectionTest)
