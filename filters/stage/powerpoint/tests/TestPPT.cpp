/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Jos van den Oever <jos@vandenoever.info>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "TestPPT.h"
#include "PptToOdp.h"
#include "pole.h"
#include <KoOdf.h>
#include <QBuffer>
#include <QDebug>
#include <QDir>
#include <QTest>

namespace
{

class TestRun
{
public:
    QString inputFilePath;
    QString referenceDirPath;
    void convert(QBuffer &buffer, KoStore::Backend backend);
    void compareFiles(KoStore *created, const QString &path);
    QByteArray readFile(const QString &path);
    void test();
};

}

void TestRun::convert(QBuffer &buffer, KoStore::Backend backend)
{
    KoStore *output = KoStore::createStore(&buffer, KoStore::Write, KoOdf::mimeType(KoOdf::Presentation), backend);
    POLE::Storage storage(inputFilePath.toLatin1());
    QVERIFY(storage.open());
    PptToOdp ppttoodp;
    KoFilter::ConversionStatus status = ppttoodp.convert(storage, output);
    QVERIFY(status == KoFilter::OK);
}

QByteArray TestRun::readFile(const QString &path)
{
    QFile f(referenceDirPath + path);
    f.open(QIODevice::ReadOnly);
    QByteArray data = f.readAll();
    f.close();
    return data;
}

void TestRun::compareFiles(KoStore *input, const QString &path)
{
    QVERIFY(input->hasFile(path));
    QVERIFY(input->open(path));
    const QByteArray created = input->read(input->size());
    QVERIFY(created.size() == input->size());
    QVERIFY(input->close());
    const QByteArray reference = readFile(path);

    QTextStream a(reference, QIODevice::ReadOnly);
    QTextStream b(created, QIODevice::ReadOnly);
    while (!a.atEnd()) {
        const QString oldLine = a.readLine();
        const QString newLine = b.readLine();
        if (oldLine != newLine) {
            qDebug() << "old: " << oldLine;
            qDebug() << "new: " << newLine;
        }
        QVERIFY(oldLine == newLine);
    }
    QVERIFY(b.atEnd());
    // Skip this test, as long as all lines are identical it is ok
    // On windows the sizes may differ due to \r\n newlines
    // QVERIFY(reference.size() == created.size());
}

void TestRun::test()
{
    inputFilePath = QFINDTESTDATA("data/diagram.ppt");
    referenceDirPath = QFINDTESTDATA("data/diagram_odp/");
    const KoStore::Backend backend = KoStore::Tar;
    QBuffer buffer;
    convert(buffer, backend);
    qDebug() << buffer.isOpen();
    buffer.close();
    qDebug() << buffer.size();
    KoStore *input = KoStore::createStore(&buffer, KoStore::Read, KoOdf::mimeType(KoOdf::Presentation), backend);
    compareFiles(input, "content.xml");
    compareFiles(input, "styles.xml");
    compareFiles(input, "meta.xml");
    compareFiles(input, "settings.xml");
    compareFiles(input, "META-INF/manifest.xml");
}

void TestPPT::testPPT()
{
    TestRun test;
    test.test();
    QVERIFY(true);
}

QTEST_MAIN(TestPPT)
