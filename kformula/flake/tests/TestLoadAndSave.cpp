/* This file is part of the KDE project
   Copyright 2007-2009 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

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

#include <KoXmlWriter.h>
#include <KoXmlReader.h>

#include "BasicElement.h"
#include "IdentifierElement.h"
#include "NumberElement.h"
#include "OperatorElement.h"
#include "TextElement.h"
#include "SpaceElement.h"
#include "StringElement.h"
#include "GlyphElement.h"
#include "RowElement.h"
#include "FractionElement.h"
#include "RootElement.h"
#include "StyleElement.h"
#include "ErrorElement.h"
#include "PaddedElement.h"
#include "PhantomElement.h"
#include "FencedElement.h"
#include "EncloseElement.h"
#include "MultiscriptElement.h"
#include "UnderOverElement.h"
#include "TableElement.h"
#include "TableRowElement.h"
#include "TableEntryElement.h"
#include "ActionElement.h"

static QString loadAndSave( BasicElement* element, const QString& input )
{
    KoXmlDocument doc;
    doc.setContent( input );
    element->readMathML( doc.documentElement() );
    QBuffer device;
    device.open( QBuffer::ReadWrite );
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

void test( BasicElement* element )
{
    QFETCH( QString, input );
    QFETCH( QString, output );

    QCOMPARE( loadAndSave( element, input ), output );
    delete element;
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
    // Since newline is hardcoded in KoXmlWriter and it's sematically equivalent, add it to expected result
    addRow( "<mi> x <mglyph fontfamily=\"testfont\" index=\"99\" alt=\"c\"/> d </mi>",
            "<mi>x \n <mglyph fontfamily=\"testfont\" index=\"99\" alt=\"c\"/> d</mi>" );
    addRow( "<mi> x  y    z   </mi>",
            "<mi>x y z</mi>" );

    // Entities
    addRow( "<mi>&CapitalDifferentialD;</mi>" );
    addRow( "<mi>&DifferentialD;</mi>" );
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

    // Entities
    addRow( "<mn>&ExponentialE;</mn>" );
    addRow( "<mn>&ImaginaryI;</mn>" );
}

void TestLoadAndSave::operatorElement_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    addRow( "<mo>+</mo>" );

    // Check operator attributes. Section 3.2.5.2
    addRow( "<mo form=\"prefix\">+</mo>" );
    addRow( "<mo form=\"infix\">+</mo>" );
    addRow( "<mo form=\"postfix\">+</mo>" );
    addRow( "<mo fence=\"true\">+</mo>" );
    addRow( "<mo fence=\"false\">+</mo>" );
    addRow( "<mo separator=\"true\">+</mo>" );
    addRow( "<mo separator=\"false\">+</mo>" );
    addRow( "<mo lspace=\"10em\">+</mo>" );
    addRow( "<mo lspace=\"10ex\">+</mo>" );
    addRow( "<mo lspace=\"10px\">+</mo>" );
    addRow( "<mo lspace=\"10in\">+</mo>" );
    addRow( "<mo lspace=\"10cm\">+</mo>" );
    addRow( "<mo lspace=\"10mm\">+</mo>" );
    addRow( "<mo lspace=\"10pt\">+</mo>" );
    addRow( "<mo lspace=\"10pc\">+</mo>" );
    addRow( "<mo lspace=\"90%\">+</mo>" );
    addRow( "<mo lspace=\"1.2\">+</mo>" );
    addRow( "<mo lspace=\"veryverythinmathspace\">+</mo>" );
    addRow( "<mo lspace=\"verythinmathspace\">+</mo>" );
    addRow( "<mo lspace=\"thinmathspace\">+</mo>" );
    addRow( "<mo lspace=\"mediummathspace\">+</mo>" );
    addRow( "<mo lspace=\"thickmathspace\">+</mo>" );
    addRow( "<mo lspace=\"verythickmathspace\">+</mo>" );
    addRow( "<mo lspace=\"veryverythickmathspace\">+</mo>" );
    addRow( "<mo lspace=\"negativeveryverythinmathspace\">+</mo>" );
    addRow( "<mo lspace=\"negativeverythinmathspace\">+</mo>" );
    addRow( "<mo lspace=\"negativethinmathspace\">+</mo>" );
    addRow( "<mo lspace=\"negativemediummathspace\">+</mo>" );
    addRow( "<mo lspace=\"negativethickmathspace\">+</mo>" );
    addRow( "<mo lspace=\"negativeverythickmathspace\">+</mo>" );
    addRow( "<mo lspace=\"negativeveryverythickmathspace\">+</mo>" );
    addRow( "<mo rspace=\"10em\">+</mo>" );
    addRow( "<mo rspace=\"10ex\">+</mo>" );
    addRow( "<mo rspace=\"10px\">+</mo>" );
    addRow( "<mo rspace=\"10in\">+</mo>" );
    addRow( "<mo rspace=\"10cm\">+</mo>" );
    addRow( "<mo rspace=\"10mm\">+</mo>" );
    addRow( "<mo rspace=\"10pt\">+</mo>" );
    addRow( "<mo rspace=\"10pc\">+</mo>" );
    addRow( "<mo rspace=\"90%\">+</mo>" );
    addRow( "<mo rspace=\"1.2\">+</mo>" );
    addRow( "<mo rspace=\"veryverythinmathspace\">+</mo>" );
    addRow( "<mo rspace=\"verythinmathspace\">+</mo>" );
    addRow( "<mo rspace=\"thinmathspace\">+</mo>" );
    addRow( "<mo rspace=\"mediummathspace\">+</mo>" );
    addRow( "<mo rspace=\"thickmathspace\">+</mo>" );
    addRow( "<mo rspace=\"verythickmathspace\">+</mo>" );
    addRow( "<mo rspace=\"veryverythickmathspace\">+</mo>" );
    addRow( "<mo rspace=\"negativeveryverythinmathspace\">+</mo>" );
    addRow( "<mo rspace=\"negativeverythinmathspace\">+</mo>" );
    addRow( "<mo rspace=\"negativethinmathspace\">+</mo>" );
    addRow( "<mo rspace=\"negativemediummathspace\">+</mo>" );
    addRow( "<mo rspace=\"negativethickmathspace\">+</mo>" );
    addRow( "<mo rspace=\"negativeverythickmathspace\">+</mo>" );
    addRow( "<mo rspace=\"negativeveryverythickmathspace\">+</mo>" );
    addRow( "<mo stretchy=\"true\">+</mo>" );
    addRow( "<mo stretchy=\"false\">+</mo>" );
    addRow( "<mo symmetric=\"true\">+</mo>" );
    addRow( "<mo symmetric=\"false\">+</mo>" );
    addRow( "<mo maxsize=\"10em\">+</mo>" );
    addRow( "<mo maxsize=\"10ex\">+</mo>" );
    addRow( "<mo maxsize=\"10px\">+</mo>" );
    addRow( "<mo maxsize=\"10in\">+</mo>" );
    addRow( "<mo maxsize=\"10cm\">+</mo>" );
    addRow( "<mo maxsize=\"10mm\">+</mo>" );
    addRow( "<mo maxsize=\"10pt\">+</mo>" );
    addRow( "<mo maxsize=\"10pc\">+</mo>" );
    addRow( "<mo maxsize=\"90%\">+</mo>" );
    addRow( "<mo maxsize=\"1.2\">+</mo>" );
    addRow( "<mo maxsize=\"veryverythinmathspace\">+</mo>" );
    addRow( "<mo maxsize=\"verythinmathspace\">+</mo>" );
    addRow( "<mo maxsize=\"thinmathspace\">+</mo>" );
    addRow( "<mo maxsize=\"mediummathspace\">+</mo>" );
    addRow( "<mo maxsize=\"thickmathspace\">+</mo>" );
    addRow( "<mo maxsize=\"verythickmathspace\">+</mo>" );
    addRow( "<mo maxsize=\"veryverythickmathspace\">+</mo>" );
    addRow( "<mo maxsize=\"negativeveryverythinmathspace\">+</mo>" );
    addRow( "<mo maxsize=\"negativeverythinmathspace\">+</mo>" );
    addRow( "<mo maxsize=\"negativethinmathspace\">+</mo>" );
    addRow( "<mo maxsize=\"negativemediummathspace\">+</mo>" );
    addRow( "<mo maxsize=\"negativethickmathspace\">+</mo>" );
    addRow( "<mo maxsize=\"negativeverythickmathspace\">+</mo>" );
    addRow( "<mo maxsize=\"negativeveryverythickmathspace\">+</mo>" );
    addRow( "<mo maxsize=\"infinity\">+</mo>" );
    addRow( "<mo minsize=\"10em\">+</mo>" );
    addRow( "<mo minsize=\"10ex\">+</mo>" );
    addRow( "<mo minsize=\"10px\">+</mo>" );
    addRow( "<mo minsize=\"10in\">+</mo>" );
    addRow( "<mo minsize=\"10cm\">+</mo>" );
    addRow( "<mo minsize=\"10mm\">+</mo>" );
    addRow( "<mo minsize=\"10pt\">+</mo>" );
    addRow( "<mo minsize=\"10pc\">+</mo>" );
    addRow( "<mo minsize=\"90%\">+</mo>" );
    addRow( "<mo minsize=\"1.2\">+</mo>" );
    addRow( "<mo minsize=\"veryverythinmathspace\">+</mo>" );
    addRow( "<mo minsize=\"verythinmathspace\">+</mo>" );
    addRow( "<mo minsize=\"thinmathspace\">+</mo>" );
    addRow( "<mo minsize=\"mediummathspace\">+</mo>" );
    addRow( "<mo minsize=\"thickmathspace\">+</mo>" );
    addRow( "<mo minsize=\"verythickmathspace\">+</mo>" );
    addRow( "<mo minsize=\"veryverythickmathspace\">+</mo>" );
    addRow( "<mo minsize=\"negativeveryverythinmathspace\">+</mo>" );
    addRow( "<mo minsize=\"negativeverythinmathspace\">+</mo>" );
    addRow( "<mo minsize=\"negativethinmathspace\">+</mo>" );
    addRow( "<mo minsize=\"negativemediummathspace\">+</mo>" );
    addRow( "<mo minsize=\"negativethickmathspace\">+</mo>" );
    addRow( "<mo minsize=\"negativeverythickmathspace\">+</mo>" );
    addRow( "<mo minsize=\"negativeveryverythickmathspace\">+</mo>" );
    addRow( "<mo largeop=\"true\">+</mo>" );
    addRow( "<mo largeop=\"false\">+</mo>" );
    addRow( "<mo movablelimits=\"true\">+</mo>" );
    addRow( "<mo movablelimits=\"false\">+</mo>" );
    addRow( "<mo accent=\"true\">+</mo>" );
    addRow( "<mo accent=\"false\">+</mo>" );

    // Entities
    addRow( "<mo>&InvisibleTimes;</mo>" );
    addRow( "<mo>&InvisibleComma;</mo>" );
    addRow( "<mo>&ApplyFunction;</mo>" );
}

void TestLoadAndSave::textElement_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    addRow( "<mtext></mtext>" );
    addRow( "<mtext>text</mtext>" );
    addRow( "<mtext> text </mtext>",
            "<mtext>text</mtext>");
    addRow( "<mtext> Theorem 1: </mtext>",
            "<mtext>Theorem 1:</mtext>" );
    addRow( "<mtext> &ThinSpace; </mtext>",
            "<mtext>&ThinSpace;</mtext>" );
    addRow( "<mtext> &ThickSpace;&ThickSpace; </mtext>",
            "<mtext>&ThickSpace;&ThickSpace;</mtext>" );
    addRow( "<mtext> /* a comment */ </mtext>",
            "<mtext>/* a comment */</mtext>" );
}



void TestLoadAndSave::spaceElement_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    addRow( "<mspace/>" );

    // Check operator attributes. Sefction 3.2.7.2

    addRow( "<mspace width=\"10em\"/>" );
    addRow( "<mspace width=\"10ex\"/>" );
    addRow( "<mspace width=\"10px\"/>" );
    addRow( "<mspace width=\"10in\"/>" );
    addRow( "<mspace width=\"10cm\"/>" );
    addRow( "<mspace width=\"10mm\"/>" );
    addRow( "<mspace width=\"10pt\"/>" );
    addRow( "<mspace width=\"10pc\"/>" );
    addRow( "<mspace width=\"90%\"/>" );
    addRow( "<mspace width=\"1.2\"/>" );
    addRow( "<mspace width=\"veryverythinmathspace\"/>" );
    addRow( "<mspace width=\"verythinmathspace\"/>" );
    addRow( "<mspace width=\"thinmathspace\"/>" );
    addRow( "<mspace width=\"mediummathspace\"/>" );
    addRow( "<mspace width=\"thickmathspace\"/>" );
    addRow( "<mspace width=\"verythickmathspace\"/>" );
    addRow( "<mspace width=\"veryverythickmathspace\"/>" );
    addRow( "<mspace width=\"negativeveryverythinmathspace\"/>" );
    addRow( "<mspace width=\"negativeverythinmathspace\"/>" );
    addRow( "<mspace width=\"negativethinmathspace\"/>" );
    addRow( "<mspace width=\"negativemediummathspace\"/>" );
    addRow( "<mspace width=\"negativethickmathspace\"/>" );
    addRow( "<mspace width=\"negativeverythickmathspace\"/>" );
    addRow( "<mspace width=\"negativeveryverythickmathspace\"/>" );

    addRow( "<mspace height=\"10em\"/>" );
    addRow( "<mspace height=\"10ex\"/>" );
    addRow( "<mspace height=\"10px\"/>" );
    addRow( "<mspace height=\"10in\"/>" );
    addRow( "<mspace height=\"10cm\"/>" );
    addRow( "<mspace height=\"10mm\"/>" );
    addRow( "<mspace height=\"10pt\"/>" );
    addRow( "<mspace height=\"10pc\"/>" );
    addRow( "<mspace height=\"90%\"/>" );
    addRow( "<mspace height=\"1.2\"/>" );

    addRow( "<mspace depth=\"10em\"/>" );
    addRow( "<mspace depth=\"10ex\"/>" );
    addRow( "<mspace depth=\"10px\"/>" );
    addRow( "<mspace depth=\"10in\"/>" );
    addRow( "<mspace depth=\"10cm\"/>" );
    addRow( "<mspace depth=\"10mm\"/>" );
    addRow( "<mspace depth=\"10pt\"/>" );
    addRow( "<mspace depth=\"10pc\"/>" );
    addRow( "<mspace depth=\"90%\"/>" );
    addRow( "<mspace depth=\"1.2\"/>" );

    addRow( "<mspace linebreak=\"auto\"/>" );
    addRow( "<mspace linebreak=\"newline\"/>" );
    addRow( "<mspace linebreak=\"indentingnewline\"/>" );
    addRow( "<mspace linebreak=\"nobreak\"/>" );
    addRow( "<mspace linebreak=\"goodbreak\"/>" );
    addRow( "<mspace linebreak=\"badbreak\"/>" );

}

void TestLoadAndSave::stringElement_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    // TODO
    addRow( "" );
}

void TestLoadAndSave::glyphElement_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    // TODO
    addRow( "" );
}

void TestLoadAndSave::mathVariant_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    /*
     * Test all possible values of mathvariant attributes
     */
    addRow( "<mi mathvariant=\"normal\">x</mi>" );
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

void TestLoadAndSave::rowElement_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    addRow( "<mrow></mrow>" );
    addRow( "<mrow>\n <mi>x</mi>\n</mrow>" );
}

void TestLoadAndSave::fractionElement_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    // TODO
    addRow( "<mfrac><mi>x</mi><mi>y</mi></mfrac>",
            "<mfrac>\n"
            " <mi>x</mi>\n"
            " <mi>y</mi>\n"
            "</mfrac>");
}

void TestLoadAndSave::rootElement_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    addRow( "<mroot><mi>x</mi><mn>2</mn></mroot>" );
}

void TestLoadAndSave::styleElement_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    // TODO
    addRow( "<mstyle></mstyle>" );
}

void TestLoadAndSave::errorElement_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    // TODO
    addRow( "<merror></merror>" );
}

void TestLoadAndSave::paddedElement_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    // TODO
    addRow( "<mpadded></mpadded>" );
}

void TestLoadAndSave::phantomElement_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    // TODO
    addRow( "<mphantom></mphantom>" );
}

void TestLoadAndSave::fencedElement_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    // TODO
    addRow( "<mfenced></mfenced>" );
}

void TestLoadAndSave::encloseElement_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    // TODO
    addRow( "<menclose></menclose>" );
}

void TestLoadAndSave::subElement_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    // TODO
    addRow( "<msub></msub>" );
}

void TestLoadAndSave::supElement_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    // TODO
    addRow( "<msup></msup>" );
}

void TestLoadAndSave::subsupElement_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    // TODO
    addRow( "<" );
}

void TestLoadAndSave::underElement_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    // TODO
    addRow( "" );
}

void TestLoadAndSave::overElement_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    // TODO
    addRow( "" );
}

void TestLoadAndSave::underoverElement_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    // TODO
    addRow( "" );
}

void TestLoadAndSave::multiscriptsElement_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    // TODO
    addRow( "" );
}

void TestLoadAndSave::tableElement_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    // TODO
    addRow( "" );
}

void TestLoadAndSave::trElement_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    // TODO
    addRow( "" );
}

void TestLoadAndSave::labeledtrElement_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    // TODO
    addRow( "" );
}

void TestLoadAndSave::tdElement_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    // TODO
    addRow( "" );
}

void TestLoadAndSave::actionElement_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    // TODO
    addRow( "" );
}

void TestLoadAndSave::identifierElement()
{
    test( new IdentifierElement );
}

void TestLoadAndSave::numberElement()
{
    test( new NumberElement );
}

void TestLoadAndSave::operatorElement()
{
    test( new OperatorElement );
}

void TestLoadAndSave::textElement()
{
    test( new TextElement );
}

void TestLoadAndSave::spaceElement()
{
    test( new SpaceElement );
}

void TestLoadAndSave::stringElement()
{
    test( new StringElement );
}

void TestLoadAndSave::glyphElement()
{
    test( new GlyphElement );
}

void TestLoadAndSave::mathVariant()
{
    identifierElement();
}

void TestLoadAndSave::mathSize()
{
    identifierElement();
}

void TestLoadAndSave::mathColor()
{
    identifierElement();
}

void TestLoadAndSave::mathBackground()
{
    identifierElement();
}

void TestLoadAndSave::fontSize()
{
    identifierElement();
}

void TestLoadAndSave::fontWeight()
{
    identifierElement();
}

void TestLoadAndSave::fontStyle()
{
    identifierElement();
}

void TestLoadAndSave::color()
{
    identifierElement();
}

void TestLoadAndSave::rowElement()
{
    test( new RowElement );
}

void TestLoadAndSave::fractionElement()
{
    test( new FractionElement );
}

void TestLoadAndSave::rootElement()
{
    test( new RootElement );
}

void TestLoadAndSave::styleElement()
{
    test( new StyleElement );
}

void TestLoadAndSave::errorElement()
{
    test( new ErrorElement );
}

void TestLoadAndSave::paddedElement()
{
    test( new PaddedElement );
}

void TestLoadAndSave::phantomElement()
{
    test( new PhantomElement );
}

void TestLoadAndSave::fencedElement()
{
    test( new FencedElement );
}

void TestLoadAndSave::encloseElement()
{
    test( new EncloseElement );
}

void TestLoadAndSave::subElement()
{
    test( new MultiscriptElement );
}

void TestLoadAndSave::supElement()
{
    test( new MultiscriptElement );
}

void TestLoadAndSave::subsupElement()
{
    test( new MultiscriptElement );
}

void TestLoadAndSave::underElement()
{
    test( new UnderOverElement );
}

void TestLoadAndSave::overElement()
{
    test( new UnderOverElement );
}

void TestLoadAndSave::underoverElement()
{
    test( new UnderOverElement );
}

void TestLoadAndSave::multiscriptsElement()
{
    test( new MultiscriptElement );
}

void TestLoadAndSave::tableElement()
{
    test( new TableElement );
}

void TestLoadAndSave::trElement()
{
    test( new TableRowElement );
}

void TestLoadAndSave::labeledtrElement()
{
    test( new TableRowElement );
}

void TestLoadAndSave::tdElement()
{
    test( new TableEntryElement );
}

void TestLoadAndSave::actionElement()
{
    test( new ActionElement );
}

QTEST_MAIN(TestLoadAndSave)
#include "TestLoadAndSave.moc"
