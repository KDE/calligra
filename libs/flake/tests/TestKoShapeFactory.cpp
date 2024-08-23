/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Boudewijn Rempt (boud@valdyas.org)
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "TestKoShapeFactory.h"
#include <QBuffer>
#include <QTest>

#include <FlakeDebug.h>
#include <KoOdfLoadingContext.h>
#include <KoOdfStylesReader.h>
#include <KoPathShapeFactory.h>
#include <KoShape.h>
#include <KoShapeFactoryBase.h>
#include <KoShapeLoadingContext.h>
#include <KoXmlNS.h>

void TestKoShapeFactory::testCreateFactory()
{
    KoShapeFactoryBase *factory = new KoPathShapeFactory(QStringList());
    QVERIFY(factory != nullptr);
    delete factory;
}

void TestKoShapeFactory::testSupportsKoXmlElement()
{
}

void TestKoShapeFactory::testPriority()
{
    KoShapeFactoryBase *factory = new KoPathShapeFactory(QStringList());
    QVERIFY(factory->loadingPriority() == 0);
    delete factory;
}

void TestKoShapeFactory::testCreateDefaultShape()
{
    KoShapeFactoryBase *factory = new KoPathShapeFactory(QStringList());
    KoShape *shape = factory->createDefaultShape();
    QVERIFY(shape != nullptr);
    delete shape;
    delete factory;
}

void TestKoShapeFactory::testCreateShape()
{
    KoShapeFactoryBase *factory = new KoPathShapeFactory(QStringList());
    KoShape *shape = factory->createShape(nullptr);
    QVERIFY(shape != nullptr);
    delete shape;
    delete factory;
}

void TestKoShapeFactory::testOdfElement()
{
    KoShapeFactoryBase *factory = new KoPathShapeFactory(QStringList());
    QVERIFY(factory->odfElements().constFirst().second.contains("path"));
    QVERIFY(factory->odfElements().constFirst().second.contains("line"));
    QVERIFY(factory->odfElements().constFirst().second.contains("polyline"));
    QVERIFY(factory->odfElements().constFirst().second.contains("polygon"));
    QVERIFY(factory->odfElements().constFirst().first == KoXmlNS::draw);

    QBuffer xmldevice;
    xmldevice.open(QIODevice::WriteOnly);
    QTextStream xmlstream(&xmldevice);

    xmlstream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    xmlstream
        << "<office:document-content xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\" "
           "xmlns:meta=\"urn:oasis:names:tc:opendocument:xmlns:meta:1.0\" xmlns:config=\"urn:oasis:names:tc:opendocument:xmlns:config:1.0\" "
           "xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\" xmlns:table=\"urn:oasis:names:tc:opendocument:xmlns:table:1.0\" "
           "xmlns:draw=\"urn:oasis:names:tc:opendocument:xmlns:drawing:1.0\" xmlns:presentation=\"urn:oasis:names:tc:opendocument:xmlns:presentation:1.0\" "
           "xmlns:dr3d=\"urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0\" xmlns:chart=\"urn:oasis:names:tc:opendocument:xmlns:chart:1.0\" "
           "xmlns:form=\"urn:oasis:names:tc:opendocument:xmlns:form:1.0\" xmlns:script=\"urn:oasis:names:tc:opendocument:xmlns:script:1.0\" "
           "xmlns:style=\"urn:oasis:names:tc:opendocument:xmlns:style:1.0\" xmlns:number=\"urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0\" "
           "xmlns:math=\"http://www.w3.org/1998/Math/MathML\" xmlns:svg=\"urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0\" "
           "xmlns:fo=\"urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0\" xmlns:calligra=\"http://www.calligra.org/2005/\" "
           "xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">";
    xmlstream << "<office:body>";
    xmlstream << "<office:text>";
    xmlstream << "<text:p text:style-name=\"P1\"><?opendocument cursor-position?></text:p>";
    xmlstream << "<draw:path svg:d=\"M10,10L100,100\"></draw:path>";
    xmlstream << "</office:text>";
    xmlstream << "</office:body>";
    xmlstream << "</office:document-content>";
    xmldevice.close();

    KoXmlDocument doc;
    QString errorMsg;
    int errorLine = 0;
    int errorColumn = 0;

    QCOMPARE(doc.setContent(&xmldevice, true, &errorMsg, &errorLine, &errorColumn), true);
    QCOMPARE(errorMsg.isEmpty(), true);
    QCOMPARE(errorLine, 0);
    QCOMPARE(errorColumn, 0);

    KoXmlElement contentElement = doc.documentElement();
    KoXmlElement bodyElement = contentElement.firstChild().toElement();

    // XXX: When loading is implemented, these no doubt have to be
    // sensibly filled.
    KoOdfStylesReader stylesReader;
    KoOdfLoadingContext odfContext(stylesReader, nullptr);
    KoShapeLoadingContext shapeContext(odfContext, nullptr);

    KoXmlElement textElement = bodyElement.firstChild().firstChild().toElement();
    QVERIFY(textElement.tagName() == "p");
    QCOMPARE(factory->supports(textElement, shapeContext), false);

    KoXmlElement pathElement = bodyElement.firstChild().lastChild().toElement();
    QVERIFY(pathElement.tagName() == "path");
    QCOMPARE(factory->supports(pathElement, shapeContext), true);

    KoShape *shape = factory->createDefaultShape();
    QVERIFY(shape);

    QVERIFY(shape->loadOdf(pathElement, shapeContext));

    delete shape;
    delete factory;
}

QTEST_GUILESS_MAIN(TestKoShapeFactory)
