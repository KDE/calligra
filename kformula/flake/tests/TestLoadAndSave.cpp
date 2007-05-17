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
#include "RowElement.h"

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
    addRow( "<MI>x</MI>",
            "<mi>x</mi>" );

    // See section 2.4.6 Collapsing Whitespace in Input
    addRow( "<mi> a b c </mi>",
            "<mi>a b c</mi>" );
    addRow( "<mi> x <mglyph index=\"99\" alt=\"c\"> d </mi>",
            "<mi>x <mglyph index=\"99\" alt=\"c\"> d</mi>" );
    addRow( "<mi> x  y    z   </mi>",
            "<mi>x y z</mi>" );
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

void TestLoadAndSave::operatorElement_data()
{
    // TODO
}

void TestLoadAndSave::operatorElement()
{
    // TODO
}

void TestLoadAndSave::textElement_data()
{
    // TODO
}

void TestLoadAndSave::textElement()
{
    // TODO
}

void TestLoadAndSave::spaceElement_data()
{
    // TODO
}

void TestLoadAndSave::spaceElement()
{
    // TODO
}

void TestLoadAndSave::stringElement_data()
{
    // TODO
}

void TestLoadAndSave::stringElement()
{
    // TODO
}

void TestLoadAndSave::glyphElement_data()
{
    // TODO
}

void TestLoadAndSave::glyphElement()
{
    // TODO
}

void TestLoadAndSave::mathVariant_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    /*
     * Test all possible values of mathvariant attributes
     */
    addRow( "<mi mathvariant=\"bold\">x</mi>" );
    addRow( "<mi mathvariant=\"italic\">x</mi>" );
    addRow( "<mi mathvariant=\"bold-italic\">x</mi>" );
    addRow( "<mi mathvariant=\"double-struck\">x</mi>" );
    addRow( "<mi mathvariant=\"bold-fraktur\">x</mi>" );
    addRow( "<mi mathvariant=\"fraktur\">x</mi>" );
    addRow( "<mi mathvariant=\"sans-serif\">x</mi>" );
    addRow( "<mi mathvariant=\"bold-sans-serif\">x</mi>" );
    addRow( "<mi mathvariant=\"sans-serif-italic\">x</mi>" );
    addRow( "<mi mathvariant=\"sans-serif-bold-italic\">x</mi>" );
    addRow( "<mi mathvariant=\"monospace\">x</mi>" );

    /*
     * Unallowed mathvariant values should be removed
     */
    addRow( "<mi mathvariant=\"invalid\">x</mi>",
            "<mi>x</mi>");
    
    /*
     * It's better to store attribute names and values lowercase and avoid
     * having to check whether it's upper or lower case on a per-use case,
     * which is more error prone performance consuming.
     */
    addRow( "<mi mathvariant=\"Bold\">x</mi>",
            "<mi mathvariant=\"bold\">x</mi>" );
    addRow( "<mi mathvariant=\"BOLD\">x</mi>",
            "<mi mathvariant=\"bold\">x</mi>");
    addRow( "<mi MATHVARIANT=\"bold\">x</mi>",
            "<mi mathvariant=\"bold\">x</mi>" );
    addRow( "<mi MathVariant=\"bold\">x</mi>",
            "<mi mathvariant=\"bold\">x</mi>" );
}

void TestLoadAndSave::mathVariant()
{
    identifierElement();
}

void TestLoadAndSave::mathSize_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    /*
     * Test all possible values of mathsize attributes
     */
    addRow( "<mi mathsize=\"small\">x</mi>" );
    addRow( "<mi mathsize=\"normal\">x</mi>" );
    addRow( "<mi mathsize=\"big\">x</mi>" );
    addRow( "<mi mathsize=\"10em\">x</mi>" );
    addRow( "<mi mathsize=\"10ex\">x</mi>" );
    addRow( "<mi mathsize=\"10px\">x</mi>" );
    addRow( "<mi mathsize=\"10in\">x</mi>" );
    addRow( "<mi mathsize=\"10cm\">x</mi>" );
    addRow( "<mi mathsize=\"10mm\">x</mi>" );
    addRow( "<mi mathsize=\"10pt\">x</mi>" );
    addRow( "<mi mathsize=\"10pc\">x</mi>" );
    addRow( "<mi mathsize=\"90%\">x</mi>" );
    addRow( "<mi mathsize=\"1.2\">x</mi>" );

    /*
     * Unallowed mathsize values should be removed
     */
    addRow( "<mi mathsize=\"invalid\">x</mi>",
            "<mi>x</mi>");
    
    /*
     * It's better to store attribute names and values lowercase and avoid
     * having to check whether it's upper or lower case on a per-use case,
     * which is more error prone performance consuming.
     */
    addRow( "<mi mathsize=\"Normal\">x</mi>",
            "<mi mathsize=\"normal\">x</mi>" );
    addRow( "<mi mathsize=\"NORMAL\">x</mi>",
            "<mi mathsize=\"normal\">x</mi>");
    addRow( "<mi MATHSIZE=\"normal\">x</mi>",
            "<mi mathsize=\"normal\">x</mi>" );
    addRow( "<mi MathSize=\"normal\">x</mi>",
            "<mi mathsize=\"normal\">x</mi>" );
}

void TestLoadAndSave::mathSize()
{
    identifierElement();
}

void TestLoadAndSave::mathColor_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    /*
     * Test all possible values of mathcolor attributes
     */
    addRow( "<mi mathcolor=\"white\">x</mi>" );
    addRow( "<mi mathcolor=\"black\">x</mi>" );
    addRow( "<mi mathcolor=\"green\">x</mi>" );
    addRow( "<mi mathcolor=\"#abc\">x</mi>" );
    addRow( "<mi mathcolor=\"#abcdef\">x</mi>" );

    /*
     * Unallowed mathcolor values should be removed
     */
    addRow( "<mi mathcolor=\"invalid\">x</mi>",
            "<mi>x</mi>");
    addRow( "<mi mathcolor=\"#abcdefg\">x</mi>",
            "<mi>x</mi>");
    
    /*
     * It's better to store attribute names and values lowercase and avoid
     * having to check whether it's upper or lower case on a per-use case,
     * which is more error prone performance consuming.
     */
    addRow( "<mi mathcolor=\"Black\">x</mi>",
            "<mi mathcolor=\"black\">x</mi>" );
    addRow( "<mi mathcolor=\"BLACK\">x</mi>",
            "<mi mathcolor=\"black\">x</mi>");
    addRow( "<mi MATHCOLOR=\"black\">x</mi>",
            "<mi mathcolor=\"black\">x</mi>" );
    addRow( "<mi MathColor=\"black\">x</mi>",
            "<mi mathcolor=\"black\">x</mi>" );
    addRow( "<mi MathColor=\"#ABC\">x</mi>",
            "<mi mathcolor=\"#abc\">x</mi>" );
}

void TestLoadAndSave::mathColor()
{
    identifierElement();
}

void TestLoadAndSave::mathBackground_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    /*
     * Test all possible values of mathbackground attributes
     */
    addRow( "<mi mathbackground=\"white\">x</mi>" );
    addRow( "<mi mathbackground=\"black\">x</mi>" );
    addRow( "<mi mathbackground=\"green\">x</mi>" );
    addRow( "<mi mathbackground=\"#abc\">x</mi>" );
    addRow( "<mi mathbackground=\"#abcdef\">x</mi>" );

    /*
     * Unallowed mathbackground values should be removed
     */
    addRow( "<mi mathbackground=\"invalid\">x</mi>",
            "<mi>x</mi>");
    addRow( "<mi mathbackground=\"#abcdefg\">x</mi>",
            "<mi>x</mi>");
    
    /*
     * It's better to store attribute names and values lowercase and avoid
     * having to check whether it's upper or lower case on a per-use case,
     * which is more error prone performance consuming.
     */
    addRow( "<mi mathbackground=\"Black\">x</mi>",
            "<mi mathbackground=\"black\">x</mi>" );
    addRow( "<mi mathbackground=\"BLACK\">x</mi>",
            "<mi mathbackground=\"black\">x</mi>");
    addRow( "<mi MATHBACKGROUND=\"black\">x</mi>",
            "<mi mathbackground=\"black\">x</mi>" );
    addRow( "<mi MathBackground=\"black\">x</mi>",
            "<mi mathbackground=\"black\">x</mi>" );
    addRow( "<mi MathBackground=\"#ABC\">x</mi>",
            "<mi mathbackground=\"#abc\">x</mi>" );
}

void TestLoadAndSave::mathBackground()
{
    identifierElement();
}

void TestLoadAndSave::fontSize_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    /*
     * Test all possible values of fontsize attributes
     */
    addRow( "<mi fontsize=\"10em\">x</mi>" );
    addRow( "<mi fontsize=\"10ex\">x</mi>" );
    addRow( "<mi fontsize=\"10px\">x</mi>" );
    addRow( "<mi fontsize=\"10in\">x</mi>" );
    addRow( "<mi fontsize=\"10cm\">x</mi>" );
    addRow( "<mi fontsize=\"10mm\">x</mi>" );
    addRow( "<mi fontsize=\"10pt\">x</mi>" );
    addRow( "<mi fontsize=\"10pc\">x</mi>" );
    addRow( "<mi fontsize=\"90%\">x</mi>" );
    addRow( "<mi fontsize=\"1.2\">x</mi>" );

    /*
     * Unallowed fontsize values should be removed
     */
    addRow( "<mi fontsize=\"invalid\">x</mi>",
            "<mi>x</mi>");
    
    /*
     * It's better to store attribute names and values lowercase and avoid
     * having to check whether it's upper or lower case on a per-use case,
     * which is more error prone performance consuming.
     */
    addRow( "<mi fontsize=\"10Em\">x</mi>",
            "<mi fontsize=\"10em\">x</mi>" );
    addRow( "<mi fontsize=\"10EM\">x</mi>",
            "<mi fontsize=\"10em\">x</mi>");
    addRow( "<mi FONTSIZE=\"10em\">x</mi>",
            "<mi fontsize=\"10em\">x</mi>" );
    addRow( "<mi FontSize=\"10em\">x</mi>",
            "<mi fontsize=\"10em\">x</mi>" );
}

void TestLoadAndSave::fontSize()
{
    identifierElement();
}

void TestLoadAndSave::fontWeight_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    /*
     * Test all possible values of fontweight attributes
     */
    addRow( "<mi fontweight=\"bold\">x</mi>" );
    addRow( "<mi fontweight=\"normal\">x</mi>" );

    /*
     * Unallowed fontweight values should be removed
     */
    addRow( "<mi fontweight=\"invalid\">x</mi>",
            "<mi>x</mi>");
    
    /*
     * It's better to store attribute names and values lowercase and avoid
     * having to check whether it's upper or lower case on a per-use case,
     * which is more error prone performance consuming.
     */
    addRow( "<mi fontweight=\"Bold\">x</mi>",
            "<mi fontweight=\"bold\">x</mi>" );
    addRow( "<mi fontweight=\"BOLD\">x</mi>",
            "<mi fontweight=\"bold\">x</mi>");
    addRow( "<mi FONTWEIGHT=\"bold\">x</mi>",
            "<mi fontweight=\"bold\">x</mi>" );
    addRow( "<mi FontWeight=\"bold\">x</mi>",
            "<mi fontweight=\"bold\">x</mi>" );
}

void TestLoadAndSave::fontWeight()
{
    identifierElement();
}

void TestLoadAndSave::fontStyle_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    /*
     * Test all possible values of fontstyle attributes
     */
    addRow( "<mi fontstyle=\"italic\">x</mi>" );
    addRow( "<mi fontstyle=\"normal\">x</mi>" );

    /*
     * Unallowed fontstyle values should be removed
     */
    addRow( "<mi fontstyle=\"invalid\">x</mi>",
            "<mi>x</mi>");
    
    /*
     * It's better to store attribute names and values lowercase and avoid
     * having to check whether it's upper or lower case on a per-use case,
     * which is more error prone performance consuming.
     */
    addRow( "<mi fontstyle=\"Italic\">x</mi>",
            "<mi fontstyle=\"italic\">x</mi>" );
    addRow( "<mi fontstyle=\"ITALIC\">x</mi>",
            "<mi fontstyle=\"italic\">x</mi>");
    addRow( "<mi FONTSTYLE=\"italic\">x</mi>",
            "<mi fontstyle=\"italic\">x</mi>" );
    addRow( "<mi FontStyle=\"italic\">x</mi>",
            "<mi fontstyle=\"italic\">x</mi>" );
}

void TestLoadAndSave::fontStyle()
{
    identifierElement();
}

void TestLoadAndSave::color_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    /*
     * Test all possible values of color attributes
     */
    addRow( "<mi color=\"white\">x</mi>" );
    addRow( "<mi color=\"black\">x</mi>" );
    addRow( "<mi color=\"green\">x</mi>" );
    addRow( "<mi color=\"#abc\">x</mi>" );
    addRow( "<mi color=\"#abcdef\">x</mi>" );

    /*
     * Unallowed color values should be removed
     */
    addRow( "<mi color=\"invalid\">x</mi>",
            "<mi>x</mi>");
    addRow( "<mi color=\"#abcdefg\">x</mi>",
            "<mi>x</mi>");
    
    /*
     * It's better to store attribute names and values lowercase and avoid
     * having to check whether it's upper or lower case on a per-use case,
     * which is more error prone performance consuming.
     */
    addRow( "<mi color=\"Black\">x</mi>",
            "<mi color=\"black\">x</mi>" );
    addRow( "<mi color=\"BLACK\">x</mi>",
            "<mi color=\"black\">x</mi>");
    addRow( "<mi COLOR=\"black\">x</mi>",
            "<mi color=\"black\">x</mi>" );
    addRow( "<mi Color=\"black\">x</mi>",
            "<mi color=\"black\">x</mi>" );
    addRow( "<mi Color=\"#ABC\">x</mi>",
            "<mi color=\"#abc\">x</mi>" );
}

void TestLoadAndSave::color()
{
    identifierElement();
}

void TestLoadAndSave::rowElement_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    addRow( "<mrow></mrow>" );
    addRow( "<mrow>\n <mi>x</mi>\n</mrow>" );
}

void TestLoadAndSave::rowElement()
{
    QFETCH(QString, input);
    QFETCH(QString, output);

    RowElement* element = new RowElement;
    QCOMPARE(loadAndSave(element, input), output);
    delete element;
}

void TestLoadAndSave::fractionElement_data()
{
    // TODO
}

void TestLoadAndSave::fractionElement()
{
    // TODO
}

void TestLoadAndSave::rootElement_data()
{
    // TODO
}

void TestLoadAndSave::rootElement()
{
    // TODO
}

void TestLoadAndSave::styleElement_data()
{
    // TODO
}

void TestLoadAndSave::styleElement()
{
    // TODO
}

void TestLoadAndSave::errorElement_data()
{
    // TODO
}

void TestLoadAndSave::errorElement()
{
    // TODO
}

void TestLoadAndSave::paddedElement_data()
{
    // TODO
}

void TestLoadAndSave::paddedElement()
{
    // TODO
}

void TestLoadAndSave::phantomElement_data()
{
    // TODO
}

void TestLoadAndSave::phantomElement()
{
    // TODO
}

void TestLoadAndSave::fencedElement_data()
{
    // TODO
}

void TestLoadAndSave::fencedElement()
{
    // TODO
}

void TestLoadAndSave::encloseElement_data()
{
    // TODO
}

void TestLoadAndSave::encloseElement()
{
    // TODO
}

void TestLoadAndSave::subElement_data()
{
    // TODO
}

void TestLoadAndSave::subElement()
{
    // TODO
}

void TestLoadAndSave::supElement_data()
{
    // TODO
}

void TestLoadAndSave::supElement()
{
    // TODO
}

void TestLoadAndSave::subsupElement_data()
{
    // TODO
}

void TestLoadAndSave::subsupElement()
{
    // TODO
}

void TestLoadAndSave::underElement_data()
{
    // TODO
}

void TestLoadAndSave::underElement()
{
    // TODO
}

void TestLoadAndSave::overElement_data()
{
    // TODO
}

void TestLoadAndSave::overElement()
{
    // TODO
}

void TestLoadAndSave::underoverElement_data()
{
    // TODO
}

void TestLoadAndSave::underoverElement()
{
    // TODO
}

void TestLoadAndSave::multiscriptsElement_data()
{
    // TODO
}

void TestLoadAndSave::multiscriptsElement()
{
    // TODO
}

void TestLoadAndSave::tableElement_data()
{
    // TODO
}

void TestLoadAndSave::tableElement()
{
    // TODO
}

void TestLoadAndSave::trElement_data()
{
    // TODO
}

void TestLoadAndSave::trElement()
{
    // TODO
}

void TestLoadAndSave::labeledtrElement_data()
{
    // TODO
}

void TestLoadAndSave::labeledtrElement()
{
    // TODO
}

void TestLoadAndSave::tdElement_data()
{
    // TODO
}

void TestLoadAndSave::tdElement()
{
    // TODO
}

void TestLoadAndSave::actionElement_data()
{
    // TODO
}

void TestLoadAndSave::actionElement()
{
    // TODO
}

QTEST_MAIN(TestLoadAndSave)
#include "TestLoadAndSave.moc"
