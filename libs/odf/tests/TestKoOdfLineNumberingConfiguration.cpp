// SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include <QLoggingCategory>
#include <QObject>
#include <QTest>

#include "OdfRoundTripTestUtils.h"
#include <KoOdfLineNumberingConfiguration.h>
#include <KoOdfNumberDefinition.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

class TestKoOdfLineNumberingConfiguration : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();

    void testLoad();
    void testSaveAndLoadRoundTrip();
    void testSaveAndLoadRoundTrip_disabled();
};

void TestKoOdfLineNumberingConfiguration::initTestCase()
{
    QLoggingCategory::setFilterRules(
        "*.debug=false\n"
        "calligra.lib.odf=true");
}

void TestKoOdfLineNumberingConfiguration::testLoad()
{
    KoXmlDocument doc;
    const KoXmlElement root = OdfTestUtils::parseXmlFragment(
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<text:linenumbering-configuration xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\""
        " xmlns:style=\"urn:oasis:names:tc:opendocument:xmlns:style:1.0\""
        " text:number-lines=\"true\" text:style-name=\"LineNumber\" text:increment=\"5\""
        " text:position=\"outer\" text:offset=\"20\" text:count-empty-lines=\"true\""
        " text:count-in-text-boxes=\"true\" text:restart-on-page=\"true\">"
        "<text:linenumbering-separator text:increment=\"3\">--</text:linenumbering-separator>"
        "</text:linenumbering-configuration>",
        doc);
    QVERIFY(!root.isNull());

    KoOdfLineNumberingConfiguration config;
    config.loadOdf(root);

    QCOMPARE(config.enabled(), true);
    QCOMPARE(config.textStyle(), QString("LineNumber"));
    QCOMPARE(config.increment(), 5);
    QCOMPARE(config.position(), KoOdfLineNumberingConfiguration::Outer);
    QCOMPARE(config.offset(), 20);
    QCOMPARE(config.countEmptyLines(), true);
    QCOMPARE(config.countLinesInTextBoxes(), true);
    QCOMPARE(config.restartNumberingOnEveryPage(), true);
    QCOMPARE(config.separator(), QString("--"));
    QCOMPARE(config.separatorIncrement(), 3);
}

void TestKoOdfLineNumberingConfiguration::testSaveAndLoadRoundTrip()
{
    KoOdfLineNumberingConfiguration config;
    config.setEnabled(true);
    config.setTextStyle("LineNo");
    config.setIncrement(2);
    config.setPosition(KoOdfLineNumberingConfiguration::Inner);
    config.setOffset(15);
    config.setCountEmptyLines(true);
    config.setCountLinesInTextBoxes(true);
    config.setRestartNumberingOnEveryPage(true);
    config.setSeparator("*");
    config.setSeparatorIncrement(7);
    KoOdfNumberDefinition numberFormat;
    numberFormat.setFormatSpecification(KoOdfNumberDefinition::AlphabeticLowerCase);
    config.setNumberFormat(numberFormat);

    // saveOdf() only adds attributes/children to the current element -- it
    // doesn't create one of its own -- so the test supplies one directly.
    KoXmlDocument doc;
    const KoXmlElement root = OdfTestUtils::writeAndReparse(
        "text:linenumbering-configuration",
        {{"text", KoXmlNS::text}, {"style", KoXmlNS::style}},
        [&config](KoXmlWriter &writer) {
            config.saveOdf(&writer);
        },
        doc);
    QVERIFY(!root.isNull());

    KoOdfLineNumberingConfiguration loaded;
    loaded.loadOdf(root);

    QCOMPARE(loaded.enabled(), config.enabled());
    QCOMPARE(loaded.textStyle(), config.textStyle());
    QCOMPARE(loaded.increment(), config.increment());
    QCOMPARE(loaded.position(), config.position());
    QCOMPARE(loaded.offset(), config.offset());
    QCOMPARE(loaded.countEmptyLines(), config.countEmptyLines());
    QCOMPARE(loaded.countLinesInTextBoxes(), config.countLinesInTextBoxes());
    QCOMPARE(loaded.restartNumberingOnEveryPage(), config.restartNumberingOnEveryPage());
    QCOMPARE(loaded.separator(), config.separator());
    QCOMPARE(loaded.separatorIncrement(), config.separatorIncrement());
    QCOMPARE(loaded.numberFormat().formatSpecification(), config.numberFormat().formatSpecification());
}

void TestKoOdfLineNumberingConfiguration::testSaveAndLoadRoundTrip_disabled()
{
    // Regression test: saveOdf() used to hardcode text:number-lines="true"
    // regardless of enabled(), so a disabled configuration would come back
    // enabled after a save/load round trip.
    KoOdfLineNumberingConfiguration config;
    config.setEnabled(false);

    KoXmlDocument doc;
    const KoXmlElement root = OdfTestUtils::writeAndReparse(
        "text:linenumbering-configuration",
        {{"text", KoXmlNS::text}, {"style", KoXmlNS::style}},
        [&config](KoXmlWriter &writer) {
            config.saveOdf(&writer);
        },
        doc);
    QVERIFY(!root.isNull());

    KoOdfLineNumberingConfiguration loaded;
    loaded.loadOdf(root);

    QCOMPARE(loaded.enabled(), false);
}

QTEST_MAIN(TestKoOdfLineNumberingConfiguration)
#include "TestKoOdfLineNumberingConfiguration.moc"
