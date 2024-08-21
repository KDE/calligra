/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Boudewijn Rempt (boud@valdyas.org)
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "TestKoShapeRegistry.h"
#include <QBuffer>
#include <QDateTime>
#include <QFile>
#include <QProcess>
#include <QString>
#include <QTest>
#include <QTextStream>

#include <KoOdfLoadingContext.h>
#include <KoOdfStylesReader.h>

#include "KoPathShape.h"
#include "KoShape.h"
#include "KoShapeLoadingContext.h"
#include "KoShapeRegistry.h"

#include <KoXmlReader.h>

#include <FlakeDebug.h>

void TestKoShapeRegistry::testGetKoShapeRegistryInstance()
{
    KoShapeRegistry *registry = KoShapeRegistry::instance();
    QVERIFY(registry != nullptr);
}

void TestKoShapeRegistry::testCreateShapes()
{
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
    xmlstream << "<draw:path svg:d=\"M0,0L100,100\"></draw:path>";
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

    KoShapeRegistry *registry = KoShapeRegistry::instance();

    // XXX: When loading is implemented, these no doubt have to be
    // sensibly filled.
    KoOdfStylesReader stylesReader;
    KoOdfLoadingContext odfContext(stylesReader, nullptr);
    KoShapeLoadingContext shapeContext(odfContext, nullptr);

    KoShape *shape = registry->createShapeFromOdf(bodyElement, shapeContext);
    QVERIFY(shape == nullptr);

    KoXmlElement pathElement = bodyElement.firstChild().firstChild().toElement();
    shape = registry->createShapeFromOdf(pathElement, shapeContext);
    QVERIFY(shape != nullptr);
    QVERIFY(shape->shapeId() == KoPathShapeId);
}

void TestKoShapeRegistry::testCreateFramedShapes()
{
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
    xmlstream << "<draw:path svg:d=\"M0,0L100,100\"></draw:path>";
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

    KoShapeRegistry *registry = KoShapeRegistry::instance();

    // XXX: When loading is implemented, these no doubt have to be
    // sensibly filled.
    KoOdfStylesReader stylesReader;
    KoOdfLoadingContext odfContext(stylesReader, nullptr);
    KoShapeLoadingContext shapeContext(odfContext, nullptr);

    KoShape *shape = registry->createShapeFromOdf(bodyElement, shapeContext);
    QVERIFY(shape == nullptr);

    KoXmlElement pathElement = bodyElement.firstChild().firstChild().toElement();
    shape = registry->createShapeFromOdf(pathElement, shapeContext);
    QVERIFY(shape != nullptr);
    QVERIFY(shape->shapeId() == KoPathShapeId);
}

QTEST_MAIN(TestKoShapeRegistry)
