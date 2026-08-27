// SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include <QLoggingCategory>
#include <QObject>
#include <QTest>

#include "OdfRoundTripTestUtils.h"
#include <KoOdfBibliographyConfiguration.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

class TestKoOdfBibliographyConfiguration : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();

    void testLoad();
    void testSaveAndLoadRoundTrip();
};

void TestKoOdfBibliographyConfiguration::initTestCase()
{
    QLoggingCategory::setFilterRules(
        "*.debug=false\n"
        "calligra.lib.odf=true");
}

void TestKoOdfBibliographyConfiguration::testLoad()
{
    KoXmlDocument doc;
    const KoXmlElement root = OdfTestUtils::parseXmlFragment(
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<text:bibliography-configuration xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\""
        " text:prefix=\"{\" text:suffix=\"}\" text:numbered-entries=\"true\" text:sort-by-position=\"false\""
        " text:sort-algorithm=\"alphanumeric\">"
        "<text:sort-key text:key=\"author\" text:sort-ascending=\"true\"/>"
        "<text:sort-key text:key=\"year\" text:sort-ascending=\"false\"/>"
        "</text:bibliography-configuration>",
        doc);
    QVERIFY(!root.isNull());

    KoOdfBibliographyConfiguration config;
    config.loadOdf(root);

    QCOMPARE(config.prefix(), QString("{"));
    QCOMPARE(config.suffix(), QString("}"));
    QCOMPARE(config.numberedEntries(), true);
    QCOMPARE(config.sortByPosition(), false);
    QCOMPARE(config.sortAlgorithm(), QString("alphanumeric"));

    QCOMPARE(config.sortKeys().size(), 2);
    QCOMPARE(config.sortKeys().at(0).first, QString("author"));
    QCOMPARE(config.sortKeys().at(0).second, Qt::AscendingOrder);
    QCOMPARE(config.sortKeys().at(1).first, QString("year"));
    QCOMPARE(config.sortKeys().at(1).second, Qt::DescendingOrder);
}

void TestKoOdfBibliographyConfiguration::testSaveAndLoadRoundTrip()
{
    KoOdfBibliographyConfiguration config;
    config.setPrefix("<");
    config.setSuffix(">");
    config.setNumberedEntries(true);
    config.setSortByPosition(false);
    config.setSortAlgorithm("alphanumeric");
    config.setSortKeys({
        {QStringLiteral("title"), Qt::AscendingOrder},
        {QStringLiteral("publisher"), Qt::DescendingOrder},
    });

    // KoOdfBibliographyConfiguration::saveOdf() writes its own complete
    // text:bibliography-configuration element, so it's written as the child
    // of a throwaway root that only exists to declare the text namespace.
    KoXmlDocument doc;
    const KoXmlElement wrapper = OdfTestUtils::writeAndReparse(
        "office:styles",
        {{"office", KoXmlNS::office}, {"text", KoXmlNS::text}},
        [&config](KoXmlWriter &writer) {
            config.saveOdf(&writer);
        },
        doc);
    QVERIFY(!wrapper.isNull());
    const KoXmlElement root = wrapper.firstChildElement();
    QVERIFY(!root.isNull());

    KoOdfBibliographyConfiguration loaded;
    loaded.loadOdf(root);

    QCOMPARE(loaded.prefix(), config.prefix());
    QCOMPARE(loaded.suffix(), config.suffix());
    QCOMPARE(loaded.numberedEntries(), config.numberedEntries());
    QCOMPARE(loaded.sortByPosition(), config.sortByPosition());
    QCOMPARE(loaded.sortAlgorithm(), config.sortAlgorithm());
    QCOMPARE(loaded.sortKeys(), config.sortKeys());
}

QTEST_MAIN(TestKoOdfBibliographyConfiguration)
#include "TestKoOdfBibliographyConfiguration.moc"
