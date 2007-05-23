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

    // Empty content
    addRow( "<mi></mi>", 0 );

    // Basic content
    addRow( "<mi>x</mi>", 0 );
    addRow( "<mi>sin</mi>", 0 );

    // Glyph element contents
    addRow( "<mi>x<mglyph fontfamily=\"serif\" alt=\"a\" index=\"97\"></mi>", 1);
    addRow( "<mi> <mglyph fontfamily=\"serif\" alt=\"sin\" index=\"97\"> </mi>", 1);
    addRow( "<mi> <mglyph fontfamily=\"serif\" alt=\"x\" index=\"97\"> "
            "     <mglyph fontfamily=\"serif\" alt=\"y\" index=\"97\"> </mi>", 2);

    // Be sure attributes don't break anything
    addRow( "<mi mathvariant=\"bold\">x</mi>", 0 );
    addRow( "<mi fontsize=\"18pt\">x</mi>", 0 );

    // Be sure content with entity references don't break anything
    addRow( "<mi> &pi; </mi>", 0 );
    addRow( "<mi> &ImaginaryI; </mi>", 0 );
}

void TestLoad::numberElement_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<int>("output");
    QTest::addColumn<int>("outputRecursive");

    // Basic content
    addRow( "<mn> 3 </mn>", 0 );
    addRow( "<mn> 1,000,000.11 </mn>", 0 );
    addRow( "<mn> 1.000.000,11 </mn>", 0 );

    // Glyph element contents
    addRow( "<mn>12<mglyph fontfamnly=\"serif\" alt=\"8\" index=\"56\"></mn>", 1);
    addRow( "<mn> <mglyph fontfamnly=\"serif\" alt=\"8\" index=\"56\"> </mn>", 1);
    addRow( "<mn> <mglyph fontfamnly=\"serif\" alt=\"8\" index=\"56\"> "
            "     <mglyph fontfamnly=\"serif\" alt=\"7\" index=\"55\"> </mn>", 2);

    // Be sure attributes don't break anything
    addRow( "<mn mathvariant=\"bold\">1</mn>", 0 );
    addRow( "<mn fontsize=\"18pt\">1</mn>", 0 );
}

void TestLoad::operatorElement_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<int>("output");
    QTest::addColumn<int>("outputRecursive");

    // Basic content
    addRow( "<mo>+</mo>", 0 );
    addRow( "<mo> ++ </mo>", 0 );

    // Glyph element contents
    addRow( "<mo>+<mglyph fontfamoly=\"serif\" alt=\"+\" index=\"43\"></mo>", 1);
    addRow( "<mo> <mglyph fontfamoly=\"serif\" alt=\"+\" index=\"43\"> </mo>", 1);
    addRow( "<mo> <mglyph fontfamoly=\"serif\" alt=\"+\" index=\"43\"> "
            "     <mglyph fontfamoly=\"serif\" alt=\"=\" index=\"61\"> </mo>", 2);

    // Be sure attributes don't break anything
    addRow( "<mo mathvariant=\"bold\">+</mo>", 0 );
    addRow( "<mo fontsize=\"18pt\">+</mo>", 0 );

    // Be sure content with entity references don't break anything
    addRow( "<mo> &sum; </mo>", 0 );
    addRow( "<mo> &InvisibleTimes; </mo>", 0 );
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

void TestLoad::numberElement()
{
    test( new NumberElement );
}

void TestLoad::operatorElement()
{
    test( new OperatorElement );
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
