/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2026 Calligra contributors
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include <QBuffer>
#include <QTemporaryDir>
#include <QTest>
#include <QXmlStreamReader>

#include <KoDocumentInfo.h>
#include <KoStore.h>

#include "calligra-version.h"

class TestKoDocumentInfo : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testLoadOasis();
    void testLoadOasisFoldsCommentsIntoDescription();
    void testLoadOasisRejectsMissingMeta_data();
    void testLoadOasisRejectsMissingMeta();
    void testLoadOasisDocumentStatistic();
    void testLoadOasisCustomProperties();
    void testSaveOasisRoundTrip();
    void testSaveOasisDocumentStatisticRoundTrip();
    void testSaveOasisCustomPropertyRoundTrip();
};

void TestKoDocumentInfo::testLoadOasis()
{
    // clang-format off
    const QString xml =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<office:document-meta"
        " xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\""
        " xmlns:meta=\"urn:oasis:names:tc:opendocument:xmlns:meta:1.0\""
        " xmlns:dc=\"http://purl.org/dc/elements/1.1/\""
        " office:version=\"1.2\">"
        "<office:meta>"
        "<meta:generator>TestGenerator/1.0</meta:generator>"
        "<dc:title>My Title</dc:title>"
        "<dc:subject>My Subject</dc:subject>"
        "<dc:date>2024-01-02T03:04:05</dc:date>"
        "<dc:language>en-US</dc:language>"
        "<meta:keyword>alpha</meta:keyword>"
        "<meta:keyword>beta</meta:keyword>"
        "<meta:initial-creator>Initial Creator</meta:initial-creator>"
        "<meta:creation-date>2023-12-31T00:00:00</meta:creation-date>"
        "<meta:editing-cycles>3</meta:editing-cycles>"
        // An element the reader doesn't recognize, with nested children -- must be skipped
        // as a whole without confusing later siblings for its children.
        "<meta:some-future-tag><meta:nested>ignored</meta:nested></meta:some-future-tag>"
        "<dc:creator>Jane Doe</dc:creator>"
        "<meta:user-defined meta:name=\"company\">Acme Corp</meta:user-defined>"
        "</office:meta>"
        "</office:document-meta>";
    // clang-format on

    QXmlStreamReader reader(xml);
    KoDocumentInfo info;
    QVERIFY(info.loadOasis(reader));

    QCOMPARE(info.originalGenerator(), QString("TestGenerator/1.0"));
    QCOMPARE(info.aboutInfo("title"), QString("My Title"));
    QCOMPARE(info.aboutInfo("subject"), QString("My Subject"));
    QCOMPARE(info.aboutInfo("date"), QString("2024-01-02T03:04:05"));
    QCOMPARE(info.aboutInfo("language"), QString("en-US"));
    QCOMPARE(info.aboutInfo("keyword"), QString("alpha;beta"));
    QCOMPARE(info.aboutInfo("initial-creator"), QString("Initial Creator"));
    QCOMPARE(info.aboutInfo("creation-date"), QString("2023-12-31T00:00:00"));
    QCOMPARE(info.aboutInfo("editing-cycles"), QString("3"));
    QCOMPARE(info.authorInfo("creator"), QString("Jane Doe"));
    QCOMPARE(info.authorInfo("company"), QString("Acme Corp"));
}

void TestKoDocumentInfo::testLoadOasisFoldsCommentsIntoDescription()
{
    // clang-format off
    const QString xml =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<office:document-meta"
        " xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\""
        " xmlns:meta=\"urn:oasis:names:tc:opendocument:xmlns:meta:1.0\""
        " xmlns:dc=\"http://purl.org/dc/elements/1.1/\">"
        "<office:meta>"
        "<dc:description>Primary description.</dc:description>"
        "<meta:comments>Legacy comment.</meta:comments>"
        "</office:meta>"
        "</office:document-meta>";
    // clang-format on

    QXmlStreamReader reader(xml);
    KoDocumentInfo info;
    QVERIFY(info.loadOasis(reader));

    // dc:description and the legacy meta:comments both feed the same "description" field.
    QCOMPARE(info.aboutInfo("description"), QString("Primary description.Legacy comment."));
}

void TestKoDocumentInfo::testLoadOasisRejectsMissingMeta_data()
{
    QTest::addColumn<QString>("xml");

    QTest::newRow("no office:meta child") << QString(
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<office:document-meta xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\">"
        "<office:other/>"
        "</office:document-meta>");

    QTest::newRow("wrong root element") << QString(
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<office:document-content xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\">"
        "<office:meta/>"
        "</office:document-content>");
}

void TestKoDocumentInfo::testLoadOasisRejectsMissingMeta()
{
    QFETCH(QString, xml);

    QXmlStreamReader reader(xml);
    KoDocumentInfo info;
    QVERIFY(!info.loadOasis(reader));
}

void TestKoDocumentInfo::testLoadOasisDocumentStatistic()
{
    // clang-format off
    const QString xml =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<office:document-meta"
        " xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\""
        " xmlns:meta=\"urn:oasis:names:tc:opendocument:xmlns:meta:1.0\">"
        "<office:meta>"
        "<meta:document-statistic meta:word-count=\"42\" meta:page-count=\"3\" meta:table-count=\"0\"/>"
        "</office:meta>"
        "</office:document-meta>";
    // clang-format on

    QXmlStreamReader reader(xml);
    KoDocumentInfo info;
    QVERIFY(info.loadOasis(reader));

    QCOMPARE(info.aboutInfo("word-count"), QString("42"));
    QCOMPARE(info.aboutInfo("page-count"), QString("3"));
    QCOMPARE(info.aboutInfo("table-count"), QString("0"));
    // Attributes absent from the element must not be synthesized.
    QCOMPARE(info.aboutInfo("character-count"), QString());
}

void TestKoDocumentInfo::testLoadOasisCustomProperties()
{
    // clang-format off
    const QString xml =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<office:document-meta"
        " xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\""
        " xmlns:meta=\"urn:oasis:names:tc:opendocument:xmlns:meta:1.0\">"
        "<office:meta>"
        // Untyped -- but not one of Calligra's own author-profile fields, so it's a
        // genuine custom property rather than author info.
        "<meta:user-defined meta:name=\"Project Code\">ABC-123</meta:user-defined>"
        // Typed custom properties, one per meta:value-type.
        "<meta:user-defined meta:name=\"Budget\" meta:value-type=\"float\">1234.5</meta:user-defined>"
        "<meta:user-defined meta:name=\"Due Date\" meta:value-type=\"date\">2026-01-01</meta:user-defined>"
        "<meta:user-defined meta:name=\"Approved\" meta:value-type=\"boolean\">true</meta:user-defined>"
        // "company" IS one of Calligra's own author-profile fields, and untyped -- so it
        // must still be routed to authorInfo(), not treated as a custom property.
        "<meta:user-defined meta:name=\"company\">Acme Corp</meta:user-defined>"
        // Same, but explicitly string-typed -- what saveOasis() itself now writes.
        "<meta:user-defined meta:name=\"city\" meta:value-type=\"string\">Springfield</meta:user-defined>"
        "</office:meta>"
        "</office:document-meta>";
    // clang-format on

    QXmlStreamReader reader(xml);
    KoDocumentInfo info;
    QVERIFY(info.loadOasis(reader));

    QCOMPARE(info.authorInfo("company"), QString("Acme Corp"));
    QVERIFY(!info.customPropertyNames().contains("company"));
    QCOMPARE(info.authorInfo("city"), QString("Springfield"));
    QVERIFY(!info.customPropertyNames().contains("city"));

    const QStringList names = info.customPropertyNames();
    QCOMPARE(names.size(), 4);

    QCOMPARE(info.customPropertyValue("Project Code"), QString("ABC-123"));
    QCOMPARE(info.customPropertyValueType("Project Code"), QString());

    QCOMPARE(info.customPropertyValue("Budget"), QString("1234.5"));
    QCOMPARE(info.customPropertyValueType("Budget"), QString("float"));

    QCOMPARE(info.customPropertyValue("Due Date"), QString("2026-01-01"));
    QCOMPARE(info.customPropertyValueType("Due Date"), QString("date"));

    QCOMPARE(info.customPropertyValue("Approved"), QString("true"));
    QCOMPARE(info.customPropertyValueType("Approved"), QString("boolean"));
}

void TestKoDocumentInfo::testSaveOasisRoundTrip()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    const QString path = tempDir.path() + QStringLiteral("/meta-roundtrip.zip");

    KoDocumentInfo original;
    original.setAboutInfo("title", "Round Trip Title");
    original.setAboutInfo("subject", "Round Trip Subject");
    original.setAboutInfo("keyword", "one;two;three");
    // setAuthorInfo() overrides win over the machine's real user profile, keeping this
    // deterministic (saveOasis() otherwise pulls the "creator" et al. from KUser/KConfig).
    original.setAuthorInfo("creator", "Round Trip Author");
    original.setAuthorInfo("company", "Round Trip Co");

    KoStore *writeStore = KoStore::createStore(path, KoStore::Write, "", KoStore::Zip);
    QVERIFY(writeStore);
    QVERIFY(!writeStore->bad());
    QVERIFY(writeStore->open("meta.xml"));
    QVERIFY(original.saveOasis(writeStore));
    QVERIFY(writeStore->close());
    delete writeStore;

    KoStore *readStore = KoStore::createStore(path, KoStore::Read, "", KoStore::Zip);
    QVERIFY(readStore);
    QVERIFY(!readStore->bad());
    QVERIFY(readStore->hasFile("meta.xml"));
    QVERIFY(readStore->open("meta.xml"));

    QXmlStreamReader reader(readStore->device());
    KoDocumentInfo reloaded;
    QVERIFY(reloaded.loadOasis(reader));
    readStore->close();
    delete readStore;

    QCOMPARE(reloaded.aboutInfo("title"), QString("Round Trip Title"));
    QCOMPARE(reloaded.aboutInfo("subject"), QString("Round Trip Subject"));
    QCOMPARE(reloaded.aboutInfo("keyword"), QString("one;two;three"));
    QCOMPARE(reloaded.authorInfo("creator"), QString("Round Trip Author"));
    QCOMPARE(reloaded.authorInfo("company"), QString("Round Trip Co"));
    QCOMPARE(reloaded.originalGenerator(), QString("Calligra/%1").arg(CALLIGRA_VERSION_STRING));
}

void TestKoDocumentInfo::testSaveOasisDocumentStatisticRoundTrip()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    const QString path = tempDir.path() + QStringLiteral("/meta-statistic-roundtrip.zip");

    KoDocumentInfo original;
    original.setAboutInfo("word-count", "1234");
    original.setAboutInfo("page-count", "7");

    KoStore *writeStore = KoStore::createStore(path, KoStore::Write, "", KoStore::Zip);
    QVERIFY(writeStore);
    QVERIFY(writeStore->open("meta.xml"));
    QVERIFY(original.saveOasis(writeStore));
    QVERIFY(writeStore->close());
    delete writeStore;

    KoStore *readStore = KoStore::createStore(path, KoStore::Read, "", KoStore::Zip);
    QVERIFY(readStore);
    QVERIFY(readStore->open("meta.xml"));

    QXmlStreamReader reader(readStore->device());
    KoDocumentInfo reloaded;
    QVERIFY(reloaded.loadOasis(reader));
    readStore->close();
    delete readStore;

    QCOMPARE(reloaded.aboutInfo("word-count"), QString("1234"));
    QCOMPARE(reloaded.aboutInfo("page-count"), QString("7"));
    // Statistics that were never set must not appear as e.g. an empty string.
    QCOMPARE(reloaded.aboutInfo("table-count"), QString());
}

void TestKoDocumentInfo::testSaveOasisCustomPropertyRoundTrip()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    const QString path = tempDir.path() + QStringLiteral("/meta-customprop-roundtrip.zip");

    KoDocumentInfo original;
    original.setCustomProperty("Project Code", "ABC-123");
    original.setCustomProperty("Budget", "1234.5", "float");

    KoStore *writeStore = KoStore::createStore(path, KoStore::Write, "", KoStore::Zip);
    QVERIFY(writeStore);
    QVERIFY(writeStore->open("meta.xml"));
    QVERIFY(original.saveOasis(writeStore));
    QVERIFY(writeStore->close());
    delete writeStore;

    KoStore *readStore = KoStore::createStore(path, KoStore::Read, "", KoStore::Zip);
    QVERIFY(readStore);
    QVERIFY(readStore->open("meta.xml"));

    QXmlStreamReader reader(readStore->device());
    KoDocumentInfo reloaded;
    QVERIFY(reloaded.loadOasis(reader));
    readStore->close();
    delete readStore;

    QCOMPARE(reloaded.customPropertyValue("Project Code"), QString("ABC-123"));
    QCOMPARE(reloaded.customPropertyValueType("Project Code"), QString());
    QCOMPARE(reloaded.customPropertyValue("Budget"), QString("1234.5"));
    QCOMPARE(reloaded.customPropertyValueType("Budget"), QString("float"));
}

QTEST_MAIN(TestKoDocumentInfo)
#include "TestKoDocumentInfo.moc"
