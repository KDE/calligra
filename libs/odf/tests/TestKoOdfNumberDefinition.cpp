// SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include <QLoggingCategory>
#include <QObject>
#include <QTest>

#include "OdfRoundTripTestUtils.h"
#include <KoOdfNumberDefinition.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

class TestKoOdfNumberDefinition : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();

    void testLoad();
    void testSaveAndLoadRoundTrip();
};

void TestKoOdfNumberDefinition::initTestCase()
{
    QLoggingCategory::setFilterRules(
        "*.debug=false\n"
        "calligra.lib.odf=true");
}

void TestKoOdfNumberDefinition::testLoad()
{
    KoXmlDocument doc;
    const KoXmlElement root = OdfTestUtils::parseXmlFragment(
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<style:list-level-properties xmlns:style=\"urn:oasis:names:tc:opendocument:xmlns:style:1.0\""
        " style:num-prefix=\"(\" style:num-suffix=\")\" style:num-format=\"i\" style:num-letter-sync=\"true\"/>",
        doc);
    QVERIFY(!root.isNull());

    KoOdfNumberDefinition def;
    def.loadOdf(root);

    QCOMPARE(def.prefix(), QString("("));
    QCOMPARE(def.suffix(), QString(")"));
    QCOMPARE(def.formatSpecification(), KoOdfNumberDefinition::RomanLowerCase);
    QCOMPARE(def.letterSynchronization(), true);
}

void TestKoOdfNumberDefinition::testSaveAndLoadRoundTrip()
{
    KoOdfNumberDefinition def;
    def.setPrefix("[");
    def.setSuffix("]");
    def.setFormatSpecification(KoOdfNumberDefinition::AlphabeticUpperCase);
    def.setLetterSynchronization(true);

    // KoOdfNumberDefinition::saveOdf() only adds attributes to the current
    // element -- it doesn't create one of its own -- so the test supplies one.
    KoXmlDocument doc;
    const KoXmlElement root = OdfTestUtils::writeAndReparse(
        "style:list-level-properties",
        {{"style", KoXmlNS::style}},
        [&def](KoXmlWriter &writer) {
            def.saveOdf(&writer);
        },
        doc);
    QVERIFY(!root.isNull());

    KoOdfNumberDefinition loaded;
    loaded.loadOdf(root);

    QCOMPARE(loaded.prefix(), def.prefix());
    QCOMPARE(loaded.suffix(), def.suffix());
    QCOMPARE(loaded.formatSpecification(), def.formatSpecification());
    QCOMPARE(loaded.letterSynchronization(), def.letterSynchronization());
}

QTEST_MAIN(TestKoOdfNumberDefinition)
#include "TestKoOdfNumberDefinition.moc"
