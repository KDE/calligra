/* This file is part of the KDE project
 * Copyright (C) 2013 Jos van den Oever <jos@vandenoever.info>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "TestPPT.h"
#include "PptToOdp.h"
#include "pole.h"
#include <KoOdf.h>
#include <QDebug>
#include <QDir>
#include <QBuffer>
#include <QTest>


namespace {

class TestRun {
public:
    QString inputFilePath;
    QString referenceDirPath;
    void convert(QBuffer& buffer, KoStore::Backend backend);
    void compareFiles(KoStore* created, const QString& path);
    QByteArray readFile(const QString& path);
    void test();
};

}

void
TestRun::convert(QBuffer& buffer, KoStore::Backend backend) {
    KoStore* output = KoStore::createStore(&buffer, KoStore::Write,
                                           KoOdf::mimeType(KoOdf::Presentation),
                                           backend);
    POLE::Storage storage(inputFilePath.toLatin1());
    QVERIFY(storage.open());
    PptToOdp ppttoodp(0, 0);
    KoFilter::ConversionStatus status = ppttoodp.convert(storage, output);
    QVERIFY(status == KoFilter::OK);
}

QByteArray
TestRun::readFile(const QString& path) {
    QFile f(referenceDirPath + path);
    f.open(QIODevice::ReadOnly);
    QByteArray data = f.readAll();
    f.close();
    return data;
}

void
TestRun::compareFiles(KoStore* input, const QString& path) {
    QVERIFY(input->hasFile(path));
    QVERIFY(input->open(path));
    const QByteArray created = input->read(input->size());
    QVERIFY(created.size() == input->size());
    QVERIFY(input->close());
    const QByteArray reference = readFile(path);

    QTextStream a(reference, QIODevice::ReadOnly);
    a.setCodec("UTF-8");
    QTextStream b(created, QIODevice::ReadOnly);
    b.setCodec("UTF-8");
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
    QVERIFY(reference.size() == created.size());
}

void
TestRun::test() {
    inputFilePath = QFINDTESTDATA("data/diagram.ppt");
    referenceDirPath = QFINDTESTDATA("data/diagram_odp/");
    const KoStore::Backend backend = KoStore::Tar;
    QBuffer buffer;
    convert(buffer, backend);
    qDebug() << buffer.isOpen();
    buffer.close();
    qDebug() << buffer.size();
    KoStore* input = KoStore::createStore(&buffer, KoStore::Read,
                                          KoOdf::mimeType(KoOdf::Presentation),
                                          backend);
    compareFiles(input, "content.xml");
    compareFiles(input, "styles.xml");
    compareFiles(input, "meta.xml");
    compareFiles(input, "settings.xml");
    compareFiles(input, "META-INF/manifest.xml");
}

void
TestPPT::testPPT() {
    TestRun test;
    test.test();
    QVERIFY(true);
}

QTEST_MAIN(TestPPT)
