/* This file is part of the KDE project
 * Copyright (C) 2007 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>
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
#include "MusicXmlWriterTest.h"
#include "Sheet.h"
#include "Part.h"
#include "PartGroup.h"
#include "MusicXmlWriter.h"

#include <KoXmlWriter.h>
#include <KoXmlReader.h>

#include <QtTest/QtTest>
#include <QBuffer>


using namespace MusicCore;

void MusicXmlWriterTest::init()
{
    writer = new MusicXmlWriter();
    dev = new QBuffer();
    dev->open(QIODevice::ReadWrite);
    xmlWriter = new KoXmlWriter(dev);
    xmlWriter->startDocument("score-partwise", "-//Recordare//DTD MusicXML 1.1 Partwise//EN",
                     "http://www.musicxml.org/dtds/partwise.dtd");
}

void MusicXmlWriterTest::cleanup()
{
    delete xmlWriter;
    delete writer;
    delete dev;
}

void MusicXmlWriterTest::testEmptySheet()
{
    Sheet* sheet = new Sheet();
    writer->writeSheet(*xmlWriter, sheet);
    delete sheet;
    xmlWriter->endDocument();

    KoXmlDocument doc;
    KoXml::setDocument(doc, dev, true);
    KoXmlDocumentType docType = doc.doctype();

    QCOMPARE(docType.name(), QString("score-partwise"));
}

void MusicXmlWriterTest::testParts()
{
    Sheet* sheet = new Sheet();
    Part* p = sheet->addPart("first part");
    p->setShortName("part1");
    sheet->addPart("second part");

    sheet->addBar();

    writer->writeSheet(*xmlWriter, sheet);
    delete sheet;
    xmlWriter->endDocument();
    
    validateOutput("parts.xml"); 
}

void MusicXmlWriterTest::testPartGroups()
{
    Sheet* sheet = new Sheet();
    sheet->addBar();
    for (int i = 0; i < 8; i++) {
        sheet->addPart(QString("part %1").arg(i));
    }

    PartGroup* pg = sheet->addPartGroup(0, 1);
    pg->setName("group 1");
    
    pg = sheet->addPartGroup(2, 3);
    pg->setSymbol(PartGroup::Brace);

    pg = sheet->addPartGroup(4, 5);
    pg->setSymbol(PartGroup::Line);

    pg = sheet->addPartGroup(6, 7);
    pg->setSymbol(PartGroup::Bracket);
    pg->setCommonBarLines(false);

    writer->writeSheet(*xmlWriter, sheet);
    delete sheet;
    xmlWriter->endDocument();
    
    validateOutput("partgroups.xml");
}

void MusicXmlWriterTest::testNestedPartGroups()
{
    Sheet* sheet = new Sheet();
    sheet->addBar();
    for (int i = 0; i < 7; i++) {
        sheet->addPart(QString("part %1").arg(i));
    }

    sheet->addPartGroup(0, 1)->setName("group 1");
    sheet->addPartGroup(1, 2)->setName("group 2");
    sheet->addPartGroup(2, 3)->setName("group 3");

    sheet->addPartGroup(0, 6)->setName("group 4");
    sheet->addPartGroup(4, 5)->setName("group 5");
    sheet->addPartGroup(4, 5)->setName("group 6");

    writer->writeSheet(*xmlWriter, sheet);
    delete sheet;
    xmlWriter->endDocument();
    
    validateOutput("nestedpartgroups.xml");
}

#define FAIL(message) do { QTest::qFail(message, __FILE__, __LINE__); return false; } while (0)
bool MusicXmlWriterTest::compareNodes(KoXmlNode& valid, KoXmlNode& result, QString path)
{
    path += '/' + valid.nodeName();
    
    if (result.localName() != valid.localName()) {
        FAIL(QString("nodeName does not match at %1; expected %2, received %3").arg(path, valid.nodeName(), result.nodeName()).toLocal8Bit().constData());
    }

    if (result.namespaceURI() != valid.namespaceURI()) {
        FAIL(QString("namespace does not match at %1; expected %2, received %3").arg(path, valid.namespaceURI(), result.namespaceURI()).toLocal8Bit().constData());
    }
    
    if (result.nodeValue() != valid.nodeValue()) {
        FAIL(QString("nodeValue does not match at %1; expected %2, received %3").arg(path, valid.nodeValue(), result.nodeValue()).toLocal8Bit().constData());
    }

    // if comparing identification nodes, simply return as the contents is not really relevant
    if (result.nodeName() == "identification") return true;

    // compare attributes
    KoXmlElement r = result.toElement();
    KoXmlElement v = valid.toElement();
    if (!r.isNull() && !v.isNull()) {
        foreach (QString attr, KoXml::attributeNames(v)) {
            if (r.attribute(attr) != v.attribute(attr)) {
                FAIL(QString("incorrect attribute %1 for %2; expected %3, received %4").arg(attr, path, v.attribute(attr), r.attribute(attr)).toLocal8Bit().constData());
            }
        }
        
        foreach (QString attr, KoXml::attributeNames(r)) {
            if (!v.hasAttribute(attr)) {
                FAIL(QString("incorrect attribute %1 for %2; expected %3, received %4").arg(attr, path, v.attribute(attr), r.attribute(attr)).toLocal8Bit().constData());
            }
        }
    }

    // compare child nodes
    if (KoXml::childNodesCount(result) != KoXml::childNodesCount(valid)) {
        FAIL(QString("childNodesCount does not match at %1; expected %2, received %3")
            .arg(path).arg(KoXml::childNodesCount(valid)).arg(KoXml::childNodesCount(result)).toLocal8Bit().constData());
    }

    int idx = 0;
    for (KoXmlNode rChild = result.firstChild(), vChild = valid.firstChild(); !rChild.isNull() || !vChild.isNull(); rChild = rChild.nextSibling(), vChild = vChild.nextSibling()) {
        if (!compareNodes(vChild, rChild, (path + "[%1]").arg(idx++))) return false;
    }

    return true;
}

bool MusicXmlWriterTest::validateOutput(const char* fname)
{
    QFile validFile(QString(KDESRCDIR "/files/%1").arg(fname));
    validFile.open(QIODevice::ReadOnly);
    KoXmlDocument valid;
    KoXml::setDocument(valid, &validFile, true);

    KoXmlDocument result;
    KoXml::setDocument(result, dev, true);

    bool res = compareNodes(valid, result);
    if (!res) {
        QFile f(QString(KDESRCDIR "/files/out_%1").arg(fname));
        f.open(QIODevice::WriteOnly);
        f.write(((QBuffer*)dev)->data());
        f.close();
    }
    return res;
}

QTEST_MAIN(MusicXmlWriterTest)

#include "MusicXmlWriterTest.moc"
