/* This file is part of the KDE project
   Copyright 2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; only
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "TestLoadAndSave.h"

#include <QtTest/QtTest>
#include <QtCore/QBuffer>
#include <QtXml/QDomDocument>

#include <KoXmlWriter.h>

#include "BasicElement.h"
#include "IdentifierElement.h"
#include "NumberElement.h"

static QString loadAndSave(BasicElement* element, const QString& input)
{
    QDomDocument doc;
    doc.setContent( input );
    element->readMathML(doc.documentElement());
    QBuffer device;
    device.open(QBuffer::ReadWrite);
    KoXmlWriter writer( &device );
    element->writeMathML( &writer );
    device.seek( 0 );
    return device.readAll();
}

static void addRow( const QString& input )
{
    QTest::newRow("Load and Save") << input << input;
}

static void addRow( const QString& input, const QString& output )
{
    QTest::newRow("Load and Save") << input << output;
}

void TestLoadAndSave::identifierElement_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    addRow( "<mi>x</mi>" );
    addRow( "<mi>abc</mi>" );
    addRow( "<mi> a b c </mi>",
            "<mi>a b c</mi>" );
}

void TestLoadAndSave::identifierElement()
{
    QFETCH(QString, input);
    QFETCH(QString, output);

    IdentifierElement* element = new IdentifierElement;
    QCOMPARE(loadAndSave(element, input), output);
    delete element;
}

void TestLoadAndSave::numberElement_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    addRow( "<mn>1</mn>" );
    addRow( "<mn>1.2</mn>" );
    addRow( "<mn>1,2</mn>" );
    addRow( "<mn>1 , 2</mn>" );
    addRow( "<mn> 12 </mn>",
            "<mn>12</mn>");
}

void TestLoadAndSave::numberElement()
{
    QFETCH(QString, input);
    QFETCH(QString, output);

    NumberElement* element = new NumberElement;
    QCOMPARE(loadAndSave(element, input), output);
    delete element;
}

QTEST_MAIN(TestLoadAndSave)
#include "TestLoadAndSave.moc"
