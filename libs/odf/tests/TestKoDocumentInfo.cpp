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
    void testSaveOasisRoundTrip();
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

QTEST_MAIN(TestKoDocumentInfo)
#include "TestKoDocumentInfo.moc"
