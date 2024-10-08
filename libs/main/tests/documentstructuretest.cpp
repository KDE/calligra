/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Girish Ramakrishnan <girish@forwardbias.in>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "documentstructuretest.h"

#include <QTest>

#include <KoDocument.h>
#include <KoDocumentInfo.h>
#include <KoOdfReadStore.h>
#include <KoStore.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>

// May the testing begin
DocumentStructureTest::DocumentStructureTest() = default;

DocumentStructureTest::~DocumentStructureTest() = default;

// initTestCase/cleanupTestCase are called beginning and end of test suite
void DocumentStructureTest::initTestCase()
{
}

void DocumentStructureTest::cleanupTestCase()
{
}

// init/cleanup are called beginning and end of every test case
void DocumentStructureTest::init()
{
}

void DocumentStructureTest::cleanup()
{
}

static KoDocumentInfo *loadDocumentInfo(const QString &odt)
{
    KoStore *store = KoStore::createStore(odt, KoStore::Read, "", KoStore::Zip);
    KoOdfReadStore odfReadStore(store);
    KoXmlDocument metaDoc;
    KoDocumentInfo *documentInfo = new KoDocumentInfo;
    QString error;
    if (!odfReadStore.loadAndParse("meta.xml", metaDoc, error)) {
        delete store;
        delete documentInfo;
        return nullptr;
    }
    if (!documentInfo->loadOasis(metaDoc)) {
        delete store;
        delete documentInfo;
        return nullptr;
    }
    delete store;
    return documentInfo;
}

void DocumentStructureTest::rootAttributes()
{
    QString odt = QString(FILES_DATA_DIR) + "/DocumentStructure/rootAttributes.odt";
    QVERIFY(QFile(odt).exists());
    KoDocumentInfo *documentInfo = loadDocumentInfo(odt);
    QVERIFY(documentInfo);

    QCOMPARE(documentInfo->aboutInfo("creation-date"), QString("2005-12-13T11:59:58"));
    QCOMPARE(documentInfo->aboutInfo("initial-creator"), QString("Majid Ali Khan"));
    QCOMPARE(documentInfo->aboutInfo("date"), QString("2005-12-13T11:59:58"));
    QCOMPARE(documentInfo->aboutInfo("language"), QString("en-US"));
    QCOMPARE(documentInfo->aboutInfo("editing-cycles"), QString("1"));
    QEXPECT_FAIL("", "Not supported", Continue);
    QCOMPARE(documentInfo->aboutInfo("editing-duration"), QString("PT19S"));

    QCOMPARE(documentInfo->authorInfo("creator"), QString("Majid Ali Khan"));
    delete documentInfo;
}

void DocumentStructureTest::predefinedMetaData()
{
    QString odt = QString(FILES_DATA_DIR) + "/DocumentStructure/predefined.odt";
    QVERIFY(QFile(odt).exists());
    KoDocumentInfo *documentInfo = loadDocumentInfo(odt);
    QVERIFY(documentInfo);
    QString keyword = documentInfo->aboutInfo("keyword");
    QStringList keywords = keyword.split(";");
    QVERIFY(keywords.count() == 3);
    QCOMPARE(keywords[0], QString("First keyword"));
    QCOMPARE(keywords[1], QString("Second keyword"));
    QCOMPARE(keywords[2], QString("Third keyword"));

    // this is not really the point of this test, but I wrote this already. So...
    QCOMPARE(documentInfo->aboutInfo("title"), QString("Meta Test Document"));
    QCOMPARE(documentInfo->aboutInfo("description"), QString("A test document to test meta elements"));
    QCOMPARE(documentInfo->aboutInfo("subject"), QString("Subject of the document"));
    QCOMPARE(documentInfo->aboutInfo("creation-date"), QString("2005-12-19T12:50:56"));
    QCOMPARE(documentInfo->aboutInfo("initial-creator"), QString("Majid Khan"));
    QCOMPARE(documentInfo->aboutInfo("date"), QString("2005-12-19T12:50:56"));

    QCOMPARE(documentInfo->authorInfo("creator"), QString("Majid Khan"));
    delete documentInfo;
}

void DocumentStructureTest::multipleDocumentContents()
{
    QString odt = QString(FILES_DATA_DIR) + "/DocumentStructure/multipleDocumentContents.odt";
    QVERIFY(QFile(odt).exists());
    KoDocumentInfo *documentInfo = loadDocumentInfo(odt);
    QVERIFY(documentInfo);
    QCOMPARE(documentInfo->authorInfo("creator"), QString("Majid Ali Khan"));

    // Should probably test if the document loads fine, but then the test would then depend
    // on kotext. See libs/kotext/opendocument/tests/TestLoading.cpp::documentFromOdt.
    delete documentInfo;
}

void DocumentStructureTest::singleDocumentContents()
{
    QString odt = QString(FILES_DATA_DIR) + "/DocumentStructure/singleDocumentContents.odt";
    QVERIFY(QFile(odt).exists());
    KoDocumentInfo *documentInfo = loadDocumentInfo(odt);
    QEXPECT_FAIL("", "Not supported", Continue);
    QVERIFY(documentInfo);

    // Should probably test if the document loads fine, but then the test would then depend
    // on kotext. See libs/kotext/opendocument/tests/TestLoading.cpp::documentFromOdt.
    delete documentInfo;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    return QTest::qExec(new DocumentStructureTest, argc, argv);
}
