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

#include "TestLoad.h"

#include <QtTest/QtTest>
#include <QtCore/QBuffer>
#include <QtXml/QDomDocument>

#include <KoXmlWriter.h>

#include "BasicElement.h"
#include "IdentifierElement.h"
#include "NumberElement.h"
#include "OperatorElement.h"
#include "RowElement.h"
#include "RootElement.h"
#include "BracketElement.h"

static void load(BasicElement* element, const QString& input)
{
    QDomDocument doc;
    doc.setContent( input );
    element->readMathML(doc.documentElement());
}

static int count( const QList<BasicElement*>& list )
{
    BasicElement* element;
    int counter = 0;
    foreach ( element, list ) {
        counter += count( element->childElements() );

    }
    counter += list.count();
    return counter;
}

static void addRow( const QString& input, int output )
{
    QTest::newRow("Load") << input << output << output;
}

static void addRow( const QString& input, int output, int outputRecursive )
{
    QTest::newRow("Load") << input << output << outputRecursive;
}

void test( BasicElement* element )
{
    QFETCH(QString, input);
    QFETCH(int, output);
    QFETCH(int, outputRecursive);

    load( element, input );
    QCOMPARE( element->childElements().count() , output );
    QCOMPARE( count( element->childElements() ), outputRecursive );
    
    delete element;
}


void TestLoad::identifierElement_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<int>("output");
    QTest::addColumn<int>("outputRecursive");

    addRow( "<mi>x</mi>", 0);
    addRow( "<mi>x<mglyph fontfamily=\"serif\" alt=\"a\" index=\"97\"></mi>", 1);
}


void TestLoad::rowElement_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<int>("output");
    QTest::addColumn<int>("outputRecursive");

    addRow( "<mrow></mrow>", 0 );
    addRow( "<mrow><mi>x</mi></mrow>", 1 );
    addRow( "<mrow><mi>x</mi><mo>=</mo><mn>3</mn></mrow>", 3 );
}

void TestLoad::rootElement_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<int>("output");
    QTest::addColumn<int>("outputRecursive");

    addRow( "<msqrt><mi>x</mi></msqrt>", 1, 2 );
    addRow( "<mroot><mi>x</mi><mn>2</mn></mroot>", 2, 4 );
}

void TestLoad::fencedElement_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<int>("output");
    QTest::addColumn<int>("outputRecursive");

    addRow( "<mfenced></mfenced>", 0 );
    addRow( "<mfenced><mi>x</mi></mfenced>", 1, 2 );           // Inferred mrow
    addRow( "<mfenced><mi>x</mi><mn>2</mn></mfenced>", 1, 4 ); // Inferred mrow and separator
}

void TestLoad::identifierElement()
{
    test( new IdentifierElement );
}

void TestLoad::rowElement()
{
    test( new RowElement );
}

void TestLoad::rootElement()
{
    test( new RootElement );
}

void TestLoad::fencedElement()
{
    test( new BracketElement );
}

QTEST_MAIN(TestLoad)
#include "TestLoad.moc"
