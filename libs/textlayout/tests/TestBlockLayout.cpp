/*
 *  This file is part of Calligra tests
 *
 *  Copyright (C) 2006-2010 Thomas Zander <zander@kde.org>
 *  Copyright (C) 2011 C. Boemann <cbo@boemann.dk>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "TestBlockLayout.h"
#include "MockRootAreaProvider.h"

#include <KoParagraphStyle.h>
#include <KoCharacterStyle.h>
#include <KoListStyle.h>
#include <KoListLevelProperties.h>
#include <KoStyleManager.h>
#include <KoTextBlockData.h>
#include <KoTextBlockBorderData.h>
#include <KoTextDocument.h>
#include <KoInlineTextObjectManager.h>

#include <QSharedPointer>
#include <QVariant>
#include <QTest>

#include <TextLayoutDebug.h>

#define FRAME_SPACING 10.0
#define LINESPACING_FACTOR 1.16

void TestBlockLayout::initTestCase()
{
    m_doc = 0;
    m_layout = 0;

    m_loremIpsum = QString("Lorem ipsum dolor sit amet, XgXgectetuer adiXiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat. Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat. Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi.");
}

void TestBlockLayout::setupTest(const QString &initText)
{
    m_doc = new QTextDocument;
    Q_ASSERT(m_doc);

    MockRootAreaProvider *provider = new MockRootAreaProvider();
    Q_ASSERT(provider);
    KoTextDocument(m_doc).setInlineTextObjectManager(new KoInlineTextObjectManager);

    m_doc->setDefaultFont(QFont("Sans Serif", 12.0, QFont::Normal, false)); //do it manually since we do not load the appDefaultStyle

    m_styleManager = new KoStyleManager(0);
    KoTextDocument(m_doc).setStyleManager(m_styleManager);

    m_layout = new KoTextDocumentLayout(m_doc, provider);
    Q_ASSERT(m_layout);
    m_doc->setDocumentLayout(m_layout);

    //m_area = provider->provide(m_layout);

    m_block = m_doc->begin();
    if (initText.length() > 0) {
        QTextCursor cursor(m_doc);
        cursor.insertText(initText);
        KoParagraphStyle style;
        style.setFontPointSize(12.0);
        style.setStyleId(101); // needed to do manually since we don't use the stylemanager
        QTextBlock b2 = m_doc->begin();
        while (b2.isValid()) {
            style.applyStyle(b2);
            b2 = b2.next();
        }
    }
}


void TestBlockLayout::testLineBreaking()
{
    setupTest(m_loremIpsum);
    m_layout->layout();
    QTextLayout *blockLayout = m_block.layout();

    //QCOMPARE(blockLayout->lineCount(), 16);
    QCOMPARE(blockLayout->lineForTextPosition(1).width(), 200.0);
}


void TestBlockLayout::testBasicLineSpacing()
{
    /// Tests incrementing Y pos based on the font size
    setupTest(m_loremIpsum);
    QTextCursor cursor(m_doc);
    cursor.setPosition(0);
    cursor.setPosition(m_loremIpsum.length() - 1, QTextCursor::KeepAnchor);
    QTextCharFormat charFormat = cursor.charFormat();
    charFormat.setFontPointSize(12.0);
    cursor.mergeCharFormat(charFormat);
    m_layout->layout();
    QTextLayout *blockLayout = m_block.layout();

    const qreal fontHeight12 = 12.0;
    qreal lineSpacing12 = fontHeight12 * LINESPACING_FACTOR;
    const qreal fontHeight18 = 18.0;
    qreal lineSpacing18 = fontHeight18 * LINESPACING_FACTOR;

    // QCOMPARE(blockLayout->lineCount(), 15);
    QTextLine line;
    for (int i = 0; i < 15; i++) {
        line = blockLayout->lineAt(i);
        QVERIFY(line.isValid());
        // The reason for this weird check is that the values are stored internally
        // as 26.6 fixed point integers. The entire internal text layout is
        // actually done using fixed point arithmetic. This is due to embedded
        // considerations, and offers general performance benefits across all
        // platforms.
        //qDebug() << i << qAbs(line.y() - i * lineSpacing12);
        QVERIFY(qAbs(line.y() - (i * lineSpacing12  + 100.0)) < ROUNDING);
    }

    // make first word smaller, should have zero effect on lineSpacing.
    cursor.setPosition(0);
    cursor.setPosition(11, QTextCursor::KeepAnchor);
    charFormat.setFontPointSize(10);
    cursor.mergeCharFormat(charFormat);
    m_layout->layout();
    for (int i = 0; i < 15; i++) {
        line = blockLayout->lineAt(i);
        QVERIFY(line.isValid());
        //qDebug() << i << qAbs(line.y() - i * lineSpacing12);
        QVERIFY(qAbs(line.y() - (i * lineSpacing12 + 100.0)) < ROUNDING);
    }

    // make first word on second line word bigger, should move that line down a little.
    int pos = blockLayout->lineAt(1).textStart();
    cursor.setPosition(pos);
    cursor.setPosition(pos + 12, QTextCursor::KeepAnchor);
    charFormat.setFontPointSize(18);
    cursor.mergeCharFormat(charFormat);
    m_layout->layout();
    line = blockLayout->lineAt(0);
    QCOMPARE(line.y(), 0.0 + 100.0);
    line = blockLayout->lineAt(1);
    QVERIFY(qAbs(line.y() - (lineSpacing12 + 100.0)) < ROUNDING);

    for (int i = 2; i < 15; i++) {
        line = blockLayout->lineAt(i);
//qDebug() << "i: " << i << " gives: " << line.y() << (lineSpacing12 + lineSpacing18 + (i - 2) * lineSpacing12);
        QVERIFY(qAbs(line.y() - (lineSpacing12 + lineSpacing18 + (i - 2) * lineSpacing12 + 100.0)) < ROUNDING);
    }
}

void TestBlockLayout::testBasicLineSpacing2()
{
    setupTest(m_loremIpsum);
    QTextCursor cursor(m_doc);
    cursor.insertText("foo\n\n"); // insert empty parag;

    m_layout->layout();
    QTextBlock block = m_doc->begin().next();
    QTextLayout *blockLayout = block.layout();
    QVERIFY(block.isValid());
    blockLayout = block.layout();
    QCOMPARE(blockLayout->lineCount(), 1);

    block = block.next();
    QVERIFY(block.isValid());
    blockLayout = block.layout();
    //qDebug() << blockLayout->lineAt(0).y();
    QVERIFY(qAbs(blockLayout->lineAt(0).y() - (2*12.0*LINESPACING_FACTOR + 100.0)) < ROUNDING);
}

void TestBlockLayout::testFixedLineSpacing()
{
    setupTest(QString("Line1")+QChar(0x2028)+"Line2"+QChar(0x2028)+"Line3");

    KoParagraphStyle style;
    style.setFontPointSize(12.0);
    style.setLineHeightAbsolute(28.0);
    QTextBlock block = m_doc->begin();
    style.applyStyle(block);

    QCOMPARE(block.blockFormat().doubleProperty(KoParagraphStyle::FixedLineHeight), 28.0);

    m_layout->layout();
    QTextLayout *blockLayout = block.layout();

    // lines with fontsize less than the fixed height are bottom aligned, resulting in
    // positive y for first line
    QCOMPARE(blockLayout->lineAt(0).y(), 28.0-12.0 + 100.0);
    QCOMPARE(blockLayout->lineAt(1).y(), 28.0 + 28.0-12.0 + 100.0);
    QCOMPARE(blockLayout->lineAt(2).y(), 56.0 + 28.0-12.0 + 100.0);

    style.setLineHeightAbsolute(8.0);
    style.applyStyle(block);

    QCOMPARE(block.blockFormat().doubleProperty(KoParagraphStyle::FixedLineHeight), 8.0);

    m_layout->layout();
    blockLayout = block.layout();

    // lines with fontsize more than the fixed height are bottom aligned, resulting in
    //negative y for first line
    QCOMPARE(blockLayout->lineAt(0).y(), 8.0-12.0 + 100.0);
    QCOMPARE(blockLayout->lineAt(1).y(), 8.0-12.0 + 8.0 + 100.0);
    QCOMPARE(blockLayout->lineAt(2).y(), 8.0-12.0 + 8.0 + 8.0 + 100.0);
}

void TestBlockLayout::testPercentageLineSpacing()
{
    setupTest(QString("Line1")+QChar(0x2028)+"Line2"+QChar(0x2028)+"Line3");

    KoParagraphStyle style;
    style.setFontPointSize(12.0);
    style.setLineHeightPercent(150); // NOTE: This is *PercentLineHeight*, so operates on font size * linespacing
    QTextBlock block = m_doc->begin();
    style.applyStyle(block);
    
    QCOMPARE(block.blockFormat().doubleProperty(KoParagraphStyle::PercentLineHeight), 150.0);

    m_layout->layout();
    QTextLayout *blockLayout = block.layout();

    const qreal origlineheight = 12.0 * LINESPACING_FACTOR;
    qreal newlineheight = origlineheight * 1.5;

    QCOMPARE(blockLayout->lineAt(0).y(), 0.0 + 100.0);
    QVERIFY(qAbs(blockLayout->lineAt(1).y() - (0.0 + newlineheight + 100.0)) < ROUNDING);
    QVERIFY(qAbs(blockLayout->lineAt(2).y() - (0.0 + newlineheight + newlineheight + 100.0)) < ROUNDING);

    style.setLineHeightPercent(50);
    style.applyStyle(block);

    QCOMPARE(block.blockFormat().doubleProperty(KoParagraphStyle::PercentLineHeight), 50.0);

    m_layout->layout();
    blockLayout = block.layout();

    newlineheight = origlineheight * 0.5;

    QCOMPARE(blockLayout->lineAt(0).y(), 0.0 + 100.0);
    QVERIFY(qAbs(blockLayout->lineAt(1).y() - (0.0 + newlineheight + 100.0)) < ROUNDING);
    QVERIFY(qAbs(blockLayout->lineAt(2).y() - (0.0 + newlineheight + newlineheight + 100.0)) < ROUNDING);
}

void TestBlockLayout::testAdvancedLineSpacing()
{
    setupTest("Line1\nLine2\nLine3\nLine4\nLine5\nLine6\nLine7");

    KoParagraphStyle style;
    style.setFontPointSize(12.0);
    style.setLineHeightPercent(80);
    QTextBlock block = m_doc->begin();
    style.applyStyle(block); //line1

    // check if styles do their work ;)
    QCOMPARE(block.blockFormat().doubleProperty(KoParagraphStyle::PercentLineHeight), 80.0);

    block = block.next();
    QVERIFY(block.isValid()); //line2
    style.setLineHeightAbsolute(28.0); // removes the percentage
    style.applyStyle(block);
    QCOMPARE(block.blockFormat().doubleProperty(KoParagraphStyle::PercentLineHeight), 0.0);
    QCOMPARE(block.blockFormat().doubleProperty(KoParagraphStyle::FixedLineHeight), 28.0);

    block = block.next();
    QVERIFY(block.isValid()); // line3
    style.setMinimumLineHeight(QTextLength(QTextLength::FixedLength, 40.0));
    style.setLineHeightPercent(120);
    style.applyStyle(block);

    block = block.next();
    QVERIFY(block.isValid()); // line4
    style.remove(KoParagraphStyle::FixedLineHeight);
    style.setMinimumLineHeight(QTextLength(QTextLength::FixedLength, 5.0));
    style.applyStyle(block);

    block = block.next();
    QVERIFY(block.isValid()); // line5
    style.setMinimumLineHeight(QTextLength(QTextLength::FixedLength, 0.0));
    style.setLineSpacing(8.0);
    style.remove(KoParagraphStyle::PercentLineHeight);
    style.applyStyle(block);
    block = block.next();
    QVERIFY(block.isValid()); // line6
    style.setLineSpacingFromFont(true);
    style.setLineHeightPercent(100);
    style.remove(KoParagraphStyle::LineSpacing);
    style.applyStyle(block);

    const qreal lineheight = 12.0 * LINESPACING_FACTOR;

    m_layout->layout();

    block = m_doc->begin(); // line1
    for (int i = 1; i < 7; ++i) {
        qInfo()<<i<<block.layout()->lineAt(0).y();
        block = block.next();
    }

    const qreal line1_y = 0.0  + 100.0;
    const qreal line1_height = 0.8 * lineheight;
    const qreal line2_y = line1_y + line1_height + 28.0 - lineheight; // bottom aligned
    const qreal line2_height = 28.0;
    const qreal line3_y = line1_y + line1_height +  line2_height;
    const qreal line3_height = 1.2 * lineheight; // percentage overrides minimum so percentage value is the right to test against
    const qreal line4_y = line1_y + line1_height + line2_height + line3_height;
    const qreal line4_height = lineheight;
    const qreal line5_y = line1_y + line1_height + line2_height + line3_height + line4_height;
    const qreal line5_height = lineheight + 8.0 /*spacing*/; // minimum of 5 is irrelevant (Note: percentage of 1.2 has been removed)
    const qreal line6_y = line1_y + line1_height + line2_height + line3_height + line4_height + line5_height;
    const qreal line6_height = lineheight;
    const qreal line7_y = line1_y + line1_height + line2_height + line3_height + line4_height + line5_height + line6_height;
    
    block = m_block; // line1
    QVERIFY(block.isValid());
    QTextLayout *blockLayout = block.layout();
    QVERIFY2(qAbs(blockLayout->lineAt(0).y() - line1_y) < ROUNDING, QString("Actual: %1 Expected: %2").arg(blockLayout->lineAt(0).y()).arg(line1_y).toLatin1());

    block = block.next(); // line2 with fixed we are bottom aligned so offset by 28.0-lineheight
    QVERIFY(block.isValid());
    blockLayout = block.layout();
    //qDebug() << blockLayout->lineAt(0).y();
    QEXPECT_FAIL("", "FIXME: Should not this calculate with lineheight and not fontsize?", Continue);
    QVERIFY2(qAbs(blockLayout->lineAt(0).y() - line2_y) < ROUNDING, QString("Actual: %1 Expected: %2").arg(blockLayout->lineAt(0).y()).arg(line2_y).toLatin1());

    block = block.next(); // line3
    QVERIFY(block.isValid());
    blockLayout = block.layout();
    //qDebug() << blockLayout->lineAt(0).y();
    QVERIFY2(qAbs(blockLayout->lineAt(0).y() - line3_y) < ROUNDING, QString("Actual: %1 Expected: %2").arg(blockLayout->lineAt(0).y()).arg(line3_y).toLatin1());

    block = block.next(); // line4
    QVERIFY(block.isValid());
    blockLayout = block.layout();
    //qDebug() << blockLayout->lineAt(0).y();
    // percentage overrides minimum so percentage value is the right to test against
    //QVERIFY(qAbs(blockLayout->lineAt(0).y() - (0.8*12 + 28.0 + 40.0 + 100.0)) < ROUNDING);
    QVERIFY2(qAbs(blockLayout->lineAt(0).y() - line4_y) < ROUNDING, QString("Actual: %1 Expected: %2").arg(blockLayout->lineAt(0).y()).arg(line4_y).toLatin1());

    block = block.next(); // line5
    QVERIFY(block.isValid());
    blockLayout = block.layout();
    //qDebug() << blockLayout->lineAt(0).y();
    // minimum of 5 is irrelevant and percentage of 1.2 was still there
    QVERIFY2(qAbs(blockLayout->lineAt(0).y() - line5_y) < ROUNDING, QString("Actual: %1 Expected: %2").arg(blockLayout->lineAt(0).y()).arg(line5_y).toLatin1());

    block = block.next(); // line6
    QVERIFY(block.isValid());
    blockLayout = block.layout();
    //qDebug() << blockLayout->lineAt(0).y();
    QEXPECT_FAIL("", "FIXME: Should not this calculate with lineheight and not fontsize?", Continue);
    QVERIFY2(qAbs(blockLayout->lineAt(0).y() - line6_y) < ROUNDING, QString("Actual: %1 Expected: %2").arg(blockLayout->lineAt(0).y()).arg(line6_y).toLatin1());

    block = block.next(); // line 7
    QVERIFY(block.isValid());
    blockLayout = block.layout();
    //qDebug() << blockLayout->lineAt(0).y();
    QEXPECT_FAIL("", "FIXME: setLineSpacingFromFont(true) has been set on line 6, so what will the linespacing be then?", Continue);
    QVERIFY2(qAbs(blockLayout->lineAt(0).y() - line7_y) < ROUNDING, QString("Actual: %1 Expected: %2").arg(blockLayout->lineAt(0).y()).arg(line7_y).toLatin1());
}

void TestBlockLayout::testEmptyLineHeights()
{
    // 1) a blank line is affected by the line break after
    //  1b) a line with contents is not affected by the linebreak
    // 2) a final line if blank can have it's height specified by a special textstyle
    //    If the special style is empty the par style is used for the line

    setupTest(QString("")+QChar(0x2028)+QChar(0x2028)+"\nNextBlock");
    QTextCursor cursor(m_doc);

    QTextCharFormat bigCharFormat;
    bigCharFormat.setFontPointSize(20.0);
    QTextCharFormat smallCharFormat;
    smallCharFormat.setFontPointSize(8.0);

    KoParagraphStyle style;
    style.setFontPointSize(12.0);
    style.setLineHeightPercent(100);

    QTextBlock block = m_doc->begin();
    style.applyStyle(block);

    // apply formats
    cursor.setPosition(0);
    cursor.setPosition(1, QTextCursor::KeepAnchor);
    cursor.mergeCharFormat(bigCharFormat);
    cursor.setPosition(1);
    cursor.setPosition(2, QTextCursor::KeepAnchor);
    cursor.mergeCharFormat(smallCharFormat);


    m_layout->layout();
    QTextLayout *blockLayout = block.layout();
    QCOMPARE(blockLayout->lineAt(0).y(), 0.0 + 100.0);

    qreal exp = (20.0 * LINESPACING_FACTOR) + 100.0;
    QVERIFY2(qAbs(blockLayout->lineAt(1).y() - exp) < ROUNDING, QString("Actual: %1 Expected: %2").arg(blockLayout->lineAt(1).y()).arg(exp).toLatin1());

    exp = (20.0 * LINESPACING_FACTOR) + (8.0 * LINESPACING_FACTOR) + 100.0;
    QVERIFY2(qAbs(blockLayout->lineAt(2).y() - exp) < ROUNDING, QString("Actual: %1 Expected: %2").arg(blockLayout->lineAt(2).y()).arg(exp).toLatin1());
    block = block.next();
    QVERIFY(block.isValid());
    blockLayout = block.layout();
    exp = (20.0 * LINESPACING_FACTOR) + (8.0 * LINESPACING_FACTOR) + (12.0 * LINESPACING_FACTOR) + 100.0;
    QVERIFY2(qAbs(blockLayout->lineAt(0).y() - exp) < ROUNDING, QString("Actual: %1 Expected: %2").arg(blockLayout->lineAt(0).y()).arg(exp).toLatin1());

    // Now do the test again but with last line having bigger font
    block = m_doc->begin();
    QTextBlockFormat blockFormat = block.blockFormat();
    KoCharacterStyle charStyle;
    charStyle.setFontPointSize(20.0);
    blockFormat.setProperty(KoParagraphStyle::EndCharStyle, QVariant::fromValue< QSharedPointer<KoCharacterStyle> >(QSharedPointer<KoCharacterStyle>(&charStyle)));
    cursor.setBlockFormat(blockFormat);

    m_layout->layout();
    blockLayout = block.layout();
    QCOMPARE(blockLayout->lineAt(0).y(), 0.0 + 100.0);
    exp = (20.0 * LINESPACING_FACTOR) + 100.0;
    QVERIFY2(qAbs(blockLayout->lineAt(1).y() - exp) < ROUNDING, QString("Actual: %1 Expected: %2").arg(blockLayout->lineAt(1).y()).arg(exp).toLatin1());
    exp = (20.0 * LINESPACING_FACTOR) + (8.0 * LINESPACING_FACTOR) + 100.0;
    QVERIFY2(qAbs(blockLayout->lineAt(2).y() - exp) < ROUNDING, QString("Actual: %1 Expected: %2").arg(blockLayout->lineAt(2).y()).arg(exp).toLatin1());
    block = block.next();
    QVERIFY(block.isValid());
    blockLayout = block.layout();
    exp = (20.0 + 8.0 + 20.0) * LINESPACING_FACTOR + 100.0;
    QVERIFY2(qAbs(blockLayout->lineAt(0).y() - exp) < ROUNDING, QString("Actual: %1 Expected: %2").arg(blockLayout->lineAt(0).y()).arg(exp).toLatin1());

    // Now do the test again but with last line having a small font
    block = m_doc->begin();
    KoCharacterStyle charStyle2;
    charStyle2.setFontPointSize(6.0);
    blockFormat.setProperty(KoParagraphStyle::EndCharStyle, QVariant::fromValue< QSharedPointer<KoCharacterStyle> >(QSharedPointer<KoCharacterStyle>(&charStyle2)));
    cursor.setBlockFormat(blockFormat);

    m_layout->layout();
    blockLayout = block.layout();
    QCOMPARE(blockLayout->lineAt(0).y(), 0.0 + 100.0);
    exp = (20.0 * LINESPACING_FACTOR) + 100.0;
    QVERIFY2(qAbs(blockLayout->lineAt(1).y() - exp) < ROUNDING, QString("Actual: %1 Expected: %2").arg(blockLayout->lineAt(1).y()).arg(exp).toLatin1());
    exp = (20.0 * LINESPACING_FACTOR) + (8.0 * LINESPACING_FACTOR) + 100.0;
    QVERIFY2(qAbs(blockLayout->lineAt(2).y() - exp) < ROUNDING, QString("Actual: %1 Expected: %2").arg(blockLayout->lineAt(2).y()).arg(exp).toLatin1());
    block = block.next();
    QVERIFY(block.isValid());
    blockLayout = block.layout();
    exp = (20.0 * LINESPACING_FACTOR) + (8.0 * LINESPACING_FACTOR) + (6.0 * LINESPACING_FACTOR) + 100.0;
    QVERIFY2(qAbs(blockLayout->lineAt(0).y() - exp) < ROUNDING, QString("Actual: %1 Expected: %2").arg(blockLayout->lineAt(0).y()).arg(exp).toLatin1());
}


// Test that spacing between blocks are the max of bottomMargin and topMargin
// of the top and bottom block respectively
// If the block doesn't connect to another block (top and bottom of pages or
// table cells, if blocks are intersected with say a table. Then it's
// just the plain margin
// For completeness sake we test with 3 blocks just to make sure it works
void TestBlockLayout::testBlockSpacing()
{
    setupTest(m_loremIpsum);
    QTextCursor cursor(m_doc);
    QTextCursor cursor1(m_doc);

    // create second parag
    cursor.setPosition(m_loremIpsum.length());
    cursor.insertText("\n");
    cursor.insertText(m_loremIpsum);

    // create third parag
    cursor.insertText("\n");
    cursor.insertText(m_loremIpsum);
    m_layout->layout();

    QTextBlock block2 = m_doc->begin().next();
    QTextBlock block3 = m_doc->begin().next().next();

    QTextCursor cursor2(block2);
    QTextCursor cursor3(block3);

    // and test spacing between blocks
    QTextBlockFormat bf1 = cursor1.blockFormat();
    QTextLayout *block1Layout = m_block.layout();
    QTextBlockFormat bf2 = cursor2.blockFormat();
    QTextLayout *block2Layout = block2.layout();
    QTextBlockFormat bf3 = cursor3.blockFormat();
    QTextLayout *block3Layout = block3.layout();
    int lastLineNum = block1Layout->lineCount() - 1;
    const qreal lineSpacing = 12.0 * LINESPACING_FACTOR;
    KoTextDocument(m_doc).setParaTableSpacingAtStart(false);
    bool paraTableSpacingAtStart = KoTextDocument(m_doc).paraTableSpacingAtStart();

    qreal spaces[3] = {0.0, 3.0, 6.0};
    for (int t1 = 0; t1 < 3; ++t1) {
        for (int t2 = 0; t2 < 3; ++t2) {
            for (int t3 = 0; t3 < 3; ++t3) {
                for (int b1 = 0; b1 < 3; ++b1) {
                    bf1.setTopMargin(spaces[t1]);
                    bf1.setBottomMargin(spaces[b1]);
                    cursor1.setBlockFormat(bf1);
                    for (int b2 = 0; b2 < 3; ++b2) {
                        bf2.setTopMargin(spaces[t2]);
                        bf2.setBottomMargin(spaces[b2]);
                        cursor2.setBlockFormat(bf2);
                        for (int b3 = 0; b3 < 3; ++b3) {
                            bf3.setTopMargin(spaces[t3]);
                            bf3.setBottomMargin(spaces[b3]);
                            cursor3.setBlockFormat(bf3);
                            m_layout->layout();

                            // Now lets do the actual testing
                            //Above first block is just plain
                            if (paraTableSpacingAtStart) {
                                QVERIFY(qAbs(block1Layout->lineAt(0).y() - spaces[t1]) < ROUNDING);
                            } else {
                                QVERIFY(qAbs(block1Layout->lineAt(0).y() - (0.0 + 100.0)) < ROUNDING);
                            }

                            // Between 1st and 2nd block is max of spaces
                            QVERIFY(qAbs((block2Layout->lineAt(0).y() - block1Layout->lineAt(lastLineNum).y() - lineSpacing) - qMax(spaces[b1], spaces[t2])) < ROUNDING);


                            // Between 2nd and 3rd block is max of spaces
                            QVERIFY(qAbs((block3Layout->lineAt(0).y() - block2Layout->lineAt(lastLineNum).y() - lineSpacing) - qMax(spaces[b2], spaces[t3])) < ROUNDING);

                            //Below 3rd block is just plain
                            //QVERIFY(qAbs(bottom()-block3Layout->lineAt(lastLineNum).y() - lineSpacing - spaces[t1]) < ROUNDING);
                        }
                    }
                }
            }
        }
    }

    KoTextDocument(m_doc).setParaTableSpacingAtStart(true);
    paraTableSpacingAtStart = KoTextDocument(m_doc).paraTableSpacingAtStart();

    for (int t1 = 0; t1 < 3; ++t1) {
        for (int t2 = 0; t2 < 3; ++t2) {
            for (int t3 = 0; t3 < 3; ++t3) {
                for (int b1 = 0; b1 < 3; ++b1) {
                    bf1.setTopMargin(spaces[t1]);
                    bf1.setBottomMargin(spaces[b1]);
                    cursor1.setBlockFormat(bf1);
                    for (int b2 = 0; b2 < 3; ++b2) {
                        bf2.setTopMargin(spaces[t2]);
                        bf2.setBottomMargin(spaces[b2]);
                        cursor2.setBlockFormat(bf2);
                        for (int b3 = 0; b3 < 3; ++b3) {
                            bf3.setTopMargin(spaces[t3]);
                            bf3.setBottomMargin(spaces[b3]);
                            cursor3.setBlockFormat(bf3);
                            m_layout->layout();

                            // Now lets do the actual testing
                            //Above first block is just plain
                            if (paraTableSpacingAtStart) {
                                QVERIFY(qAbs(block1Layout->lineAt(0).y() - (spaces[t1] + 100.0)) < ROUNDING);
                            } else {
                                QVERIFY(qAbs(block1Layout->lineAt(0).y() - (0.0 + 100.0)) < ROUNDING);
                            }

                            // Between 1st and 2nd block is max of spaces
                            QVERIFY(qAbs((block2Layout->lineAt(0).y() - block1Layout->lineAt(lastLineNum).y() - lineSpacing) - qMax(spaces[b1], spaces[t2])) < ROUNDING);


                            // Between 2nd and 3rd block is max of spaces
                            QVERIFY(qAbs((block3Layout->lineAt(0).y() - block2Layout->lineAt(lastLineNum).y() - lineSpacing) - qMax(spaces[b2], spaces[t3])) < ROUNDING);

                            //Below 3rd block is just plain
                            //QVERIFY(qAbs(bottom()-block3Layout->lineAt(lastLineNum).y() - lineSpacing - spaces[t1]) < ROUNDING);
                        }
                    }
                }
            }
        }
    }
}

void TestBlockLayout::testLeftRightMargins()
{
    setupTest(m_loremIpsum);
    QTextCursor cursor(m_doc);
    QTextBlockFormat bf = cursor.blockFormat();
    bf.setLeftMargin(10.0);
    cursor.setBlockFormat(bf);
    m_layout->layout();
    QTextLayout *blockLayout = m_block.layout();
    QCOMPARE(blockLayout->lineAt(0).x(), 10.0 + 100.0);
    QCOMPARE(blockLayout->lineAt(0).width(), 190.0);

    bf.setRightMargin(15.0);
    cursor.setBlockFormat(bf);
    m_layout->layout();
    QCOMPARE(blockLayout->lineAt(0).x(), 10.0 + 100.0);
    QCOMPARE(blockLayout->lineAt(0).width(), 175.0);

    bf.setLeftMargin(0.0);
    cursor.setBlockFormat(bf);
    m_layout->layout();
    QCOMPARE(blockLayout->lineAt(0).x(), 0.0 + 100.0);
    QCOMPARE(blockLayout->lineAt(0).width(), 185.0); // still uses the right margin of 15

    // create second parag
    cursor.setPosition(m_loremIpsum.length());
    cursor.insertText("\n");
    bf.setTopMargin(12);
    cursor.setBlockFormat(bf);
    cursor.insertText(m_loremIpsum);
    m_layout->layout();
    QCOMPARE(blockLayout->lineAt(0).x(), 0.0 + 100.0); // parag 1
    QCOMPARE(blockLayout->lineAt(0).width(), 185.0);

    // and test parag 2
    QTextBlock block2 = m_doc->begin().next();
    QTextLayout *block2Layout = block2.layout();
    QCOMPARE(block2Layout->lineAt(0).x(), 0.0 + 100.0);
    QCOMPARE(block2Layout->lineAt(0).width(), 185.0);
}

void TestBlockLayout::testTextIndent()
{
    setupTest(m_loremIpsum);
    QTextCursor cursor(m_doc);
    QTextBlockFormat bf = cursor.blockFormat();
    bf.setTextIndent(20);
    cursor.setBlockFormat(bf);

    m_layout->layout();
    QTextLayout *blockLayout = m_block.layout();

    QCOMPARE(blockLayout->lineAt(0).x(), 20.0 + 100.0);
    QCOMPARE(blockLayout->lineAt(0).width(), 180.0);
    QCOMPARE(blockLayout->lineAt(1).x(), 0.0 + 100.0);
    QCOMPARE(blockLayout->lineAt(1).width(), 200.0);

    // Add som left margin to check for no correlation
    bf.setLeftMargin(15.0);
    cursor.setBlockFormat(bf);
    m_layout->layout();
    QCOMPARE(blockLayout->lineAt(0).x(), 35.0 + 100.0);
    QCOMPARE(blockLayout->lineAt(0).width(), 165.0);
    QCOMPARE(blockLayout->lineAt(1).x(), 15.0 + 100.0);
    QCOMPARE(blockLayout->lineAt(1).width(), 185.0);

    // create second parag and see it works too
    cursor.setPosition(m_loremIpsum.length());
    cursor.insertText("\n");
    bf.setTopMargin(12);
    cursor.setBlockFormat(bf);
    cursor.insertText(m_loremIpsum);
    m_layout->layout();
    QTextBlock block2 = m_doc->begin().next();
    QTextLayout *block2Layout = block2.layout();
    QCOMPARE(block2Layout->lineAt(0).x(), 35.0 + 100.0);
    QCOMPARE(block2Layout->lineAt(0).width(), 165.0);
    QCOMPARE(block2Layout->lineAt(1).x(), 15.0 + 100.0);
    QCOMPARE(block2Layout->lineAt(1).width(), 185.0);
}

void TestBlockLayout::testTabs_data()
{
    static const struct TestCaseData {
        bool relativeTabs;
        qreal leftMargin;
        qreal textIndent;
        qreal rightMargin;
        qreal expected; // expected value of pos=2 of each line
    } testcaseDataList[] = {
        { true, 0, 0, 0, 50},
        { true, 0, 0, 5, 50},
        { true, 0, 10, 0, 50},
        { true, 0, 10, 5, 50},
        { true, 0, -10, 0, 0},
        { true, 0, -10, 5, 0},
        { true, 20, 0, 0, 70},
        { true, 20, 0, 5, 70},
        { true, 20, 10, 0, 70},
        { true, 20, 10, 5, 70},
        { true, 20, -10, 0, 20},
        { true, 20, -10, 5, 20},
        { true, -20, 0, 0+20, 30}, //+20 to avoid extra tab fitting in
        { true, -20, 0, 5+20, 30}, //+20 to avoid extra tab fitting in
        { true, -20, 10, 0+20, 30}, //+20 to avoid extra tab fitting in
        { true, -20, 10, 5+20, 30}, //+20 to avoid extra tab fitting in
        { true, -20, -10, 0+20, -20}, //+20 to avoid extra tab fitting in
        { true, -20, -10, 5+20, -20}, //+20 to avoid extra tab fitting in

        { false, 0, 0, 0, 50},
        { false, 0, 0, 5, 50},
        { false, 0, 10, 0, 50},
        { false, 0, 10, 5, 50},
        { false, 0, -10, 0, 0},
        { false, 0, -10, 5, 0},
        { false, 20, 0, 0, 50},
        { false, 20, 0, 5, 50},
        { false, 20, 10, 0, 50},
        { false, 20, 10, 5, 50},
        { false, 20, -10, 0, 50},
        { false, 20, -10, 5, 50},
        { false, -20, 0, 0+70, 0}, //+70 to avoid extra tab fitting in
        { false, -20, 0, 5+70, 0}, //+70 to avoid extra tab fitting in
        { false, -20, 10, 0+70, 0}, //+70 to avoid extra tab fitting in
        { false, -20, 10, 5+70, 0}, //+70 to avoid extra tab fitting in
        { false, -20, -10, 0+70, 0}, //+70 to avoid extra tab fitting in
        { false, -20, -10, 5+70, 0}, //+70 to avoid extra tab fitting in
    };
    static const int testcasesCount = sizeof(testcaseDataList)/sizeof(testcaseDataList[0]);

    QTest::addColumn<bool>("relativeTabs");
    QTest::addColumn<qreal>("leftMargin");
    QTest::addColumn<qreal>("textIndent");
    QTest::addColumn<qreal>("rightMargin");
    QTest::addColumn<qreal>("expected");

    for (int i = 0; i < testcasesCount; ++i) {
        const TestCaseData &testcaseData = testcaseDataList[i];

        QTest::newRow(QString::number(i).toLatin1())
            << testcaseData.relativeTabs
            << testcaseData.leftMargin
            << testcaseData.textIndent
            << testcaseData.rightMargin
            << testcaseData.expected;
    }
}

void TestBlockLayout::testTabs()
{
    QFETCH(bool, relativeTabs);
    QFETCH(qreal, leftMargin);
    QFETCH(qreal, textIndent);
    QFETCH(qreal, rightMargin);
    QFETCH(qreal, expected); // expected value of pos=2 of each line

    setupTest("x\tx\tx\tx\tx\tx\tx\tx\tx\tx\tx\tx\tx\tx\tx\te");
    QTextCursor cursor(m_doc);
    QTextBlockFormat bf = cursor.blockFormat();
    cursor.setBlockFormat(bf);

    m_layout->layout();
    QTextLayout *blockLayout = m_block.layout();

    const qreal tabSpacing = 50.0; // in pt
    m_layout->setTabSpacing(tabSpacing);

    KoTextDocument(m_doc).setRelativeTabs(relativeTabs);
    bf.setLeftMargin(leftMargin);
    bf.setTextIndent(textIndent);
    bf.setRightMargin(rightMargin);
    cursor.setBlockFormat(bf);
    m_layout->layout();

    for (int pos=0; pos<4; pos++) {
        if (pos==0)
            QCOMPARE(blockLayout->lineAt(0).cursorToX(pos*2), leftMargin + textIndent);
        else {
            warnTextLayout << blockLayout->lineAt(0).cursorToX(pos*2) << expected+(pos-1)*tabSpacing;
            QVERIFY(qAbs(blockLayout->lineAt(0).cursorToX(pos*2) - (expected+(pos-1)*tabSpacing)) < 1.0);
        }
    }
    if (textIndent == 0.0) { // excluding known fails
        for (int pos=0; pos<4; pos++) {
            // pos==0 is known to fail see https://bugs.kde.org/show_bug.cgi?id=239819
            if (pos!=0)
                QVERIFY(qAbs(blockLayout->lineAt(1).cursorToX(pos*2+8)- (expected+(pos-1)*tabSpacing)) < 1.0);
        }
        for (int pos=0; pos<4; pos++) {
            // pos==0 is known to fail see https://bugs.kde.org/show_bug.cgi?id=239819
            if (pos!=0)
                QVERIFY(qAbs(blockLayout->lineAt(2).cursorToX(pos*2+16)- (expected+(pos-1)*tabSpacing)) < 1.0);
        }
    }
}

void TestBlockLayout::testBasicTextAlignments()
{
    setupTest("Left\nCenter\nRight");

    QTextCursor cursor(m_doc);
    QTextBlockFormat format = cursor.blockFormat();
    format.setAlignment(Qt::AlignLeft);
    cursor.setBlockFormat(format);
    cursor.setPosition(6);
    format.setAlignment(Qt::AlignHCenter);
    cursor.setBlockFormat(format);
    cursor.setPosition(13);
    format.setAlignment(Qt::AlignRight);
    cursor.setBlockFormat(format);

    m_layout->layout();
    QTextLayout *blockLayout = m_block.layout();
    QCOMPARE(blockLayout->lineAt(0).x(), 100.0);

    QTextBlock block = m_doc->begin().next();
    QVERIFY(block.isValid());
    blockLayout = block.layout();

    QRectF rect = blockLayout->lineAt(0).naturalTextRect();
    QVERIFY(rect.x() > 60);
    QCOMPARE(rect.x() + rect.width() + (200 - rect.right()), 200.0);
    block = block.next();
    QVERIFY(block.isValid());
    blockLayout = block.layout();
    rect = blockLayout->lineAt(0).naturalTextRect();
    QVERIFY(rect.x() > 150);
    QVERIFY(rect.right() >= 200.0 + 100.0);
}

void TestBlockLayout::testTextAlignments()
{
    // TODO justified & justified, last line
    setupTest("Left\nRight\nﺵﻻﺆﻴﺜﺒ\nﺵﻻﺆﻴﺜﺒ\nLast Line.");
    KoParagraphStyle start;
    start.setFontPointSize(12.0);
    start.setAlignment(Qt::AlignLeading);
    KoParagraphStyle end;
    end.setFontPointSize(12.0);
    end.setAlignment(Qt::AlignTrailing);

    KoParagraphStyle startRTL;
    startRTL.setFontPointSize(12.0);
    startRTL.setAlignment(Qt::AlignLeading);
    startRTL.setTextProgressionDirection(KoText::RightLeftTopBottom);
    KoParagraphStyle endRTL;
    endRTL.setAlignment(Qt::AlignTrailing);
    endRTL.setTextProgressionDirection(KoText::RightLeftTopBottom);
    endRTL.setFontPointSize(12.0);

    QTextBlock block = m_doc->begin();
    start.applyStyle(block);
    block = block.next();
    end.applyStyle(block);
    block = block.next();
    startRTL.applyStyle(block);
    block = block.next();
    endRTL.applyStyle(block);
    block = block.next();
    endRTL.applyStyle(block);

    m_layout->layout();
    QTextLayout *blockLayout = m_block.layout();

    // line 'Left'
    QRectF rect = blockLayout->lineAt(0).naturalTextRect();
    QCOMPARE(rect.x(), 100.0);

    // line 'Right'
    block = m_doc->begin().next();
    rect = block.layout()->lineAt(0).naturalTextRect();
    QVERIFY(rect.right() - 200 <= (1 + 100.0));
    QVERIFY(rect.left() > 100.0);

    // line with align Leading and RTL progression
    block = block.next();
    rect = block.layout()->lineAt(0).naturalTextRect();
    QVERIFY(rect.right() - 200 <= (1 + 100.0));
    QVERIFY(rect.left() > 100.0); // expect right alignment

    // line with align tailing and RTL progression
    block = block.next();
    rect = block.layout()->lineAt(0).naturalTextRect();
    QCOMPARE(rect.x(), 100.0); // expect left alignment

    // non RTL _text_ but RTL progression as well as align trailing
    block = block.next();
    rect = block.layout()->lineAt(0).naturalTextRect();
    QCOMPARE(rect.x(), 100.0); // expect left alignment
    // TODO can we check if the dot is the left most painted char?
}

void TestBlockLayout::testParagraphBorders()
{
    setupTest("Paragraph with Borders\nAnother parag\n");
    QTextCursor cursor(m_doc->begin());
    QTextBlockFormat bf = cursor.blockFormat();
    bf.setProperty(KoParagraphStyle::LeftBorderStyle, KoBorder::BorderSolid);
    bf.setProperty(KoParagraphStyle::TopBorderStyle, KoBorder::BorderSolid);
    bf.setProperty(KoParagraphStyle::BottomBorderStyle, KoBorder::BorderSolid);
    bf.setProperty(KoParagraphStyle::RightBorderStyle, KoBorder::BorderSolid);
    bf.setProperty(KoParagraphStyle::LeftBorderWidth, 8.0);
    bf.setProperty(KoParagraphStyle::TopBorderWidth, 9.0);
    bf.setProperty(KoParagraphStyle::BottomBorderWidth, 10.0);
    bf.setProperty(KoParagraphStyle::RightBorderWidth, 11.0);
    cursor.setBlockFormat(bf);

    m_layout->layout();
    QTextBlock block = m_doc->begin();
    QTextLayout *blockLayout = block.layout();
    QCOMPARE(blockLayout->lineAt(0).x(), 8.0 + 100.0);
    QCOMPARE(blockLayout->lineAt(0).y(), 9.0 + 100.0);
    QCOMPARE(blockLayout->lineAt(0).width(), 200.0 - 8.0 - 11.0);
    block = block.next();
    blockLayout = block.layout();
    //warnTextLayout << "blockLayout->lineAt(0).y() "<<blockLayout->lineAt(0).y();
    qreal exp = 9.0 + (12.0 * LINESPACING_FACTOR) + 10.0 + 100.0;
    QVERIFY2(qAbs(blockLayout->lineAt(0).y() - exp) < ROUNDING, QString("Actual: %1 Expected: %2").arg(blockLayout->lineAt(0).y()).arg(exp).toLatin1());

    // borders + padding create the total inset.
    bf.setProperty(KoParagraphStyle::LeftPadding, 5.0);
    bf.setProperty(KoParagraphStyle::RightPadding, 5.0);
    bf.setProperty(KoParagraphStyle::TopPadding, 5.0);
    bf.setProperty(KoParagraphStyle::BottomPadding, 5.0);
    cursor.setBlockFormat(bf);

    m_layout->layout();
    block = m_doc->begin();
    blockLayout = block.layout();
    QCOMPARE(blockLayout->lineAt(0).x(), 13.0 + 100.0);
    QCOMPARE(blockLayout->lineAt(0).y(), 14.0 + 100.0);
    QCOMPARE(blockLayout->lineAt(0).width(), 200.0 - 8.0 - 11.0 - 5.0 * 2);
    block = block.next();
    blockLayout = block.layout();
    //qDebug() << blockLayout->lineAt(0).y() << (9.0 + 14.4 + 10 + 5.0 * 2);
    exp = 9.0 + (12.0 * LINESPACING_FACTOR) + 10 + 5.0 * 2 + 100.0;
    QVERIFY2(qAbs(blockLayout->lineAt(0).y() - exp) < ROUNDING, QString("Actual: %1 Expected: %2").arg(blockLayout->lineAt(0).y()).arg(exp).toLatin1());

    // borders are positioned outside the padding, lets check that to be the case.
    block = m_doc->begin();
    KoTextBlockData data(block);
    KoTextBlockBorderData *border = data.border();
    QVERIFY(border);
    QCOMPARE(border->hasBorders(), true);
    /*
    QRectF borderOutline = border->rect();
    QCOMPARE(borderOutline.top(), 0.);
    QCOMPARE(borderOutline.left(), 0.);
    QCOMPARE(borderOutline.right(), 200.);
    */

    // qreal borders.  Specify an additional width for each side.
    bf.setProperty(KoParagraphStyle::LeftBorderStyle, KoBorder::BorderDouble);
    bf.setProperty(KoParagraphStyle::TopBorderStyle, KoBorder::BorderDouble);
    bf.setProperty(KoParagraphStyle::BottomBorderStyle, KoBorder::BorderDouble);
    bf.setProperty(KoParagraphStyle::RightBorderStyle, KoBorder::BorderDouble);
    bf.setProperty(KoParagraphStyle::LeftInnerBorderWidth, 2.0);
    bf.setProperty(KoParagraphStyle::RightInnerBorderWidth, 2.0);
    bf.setProperty(KoParagraphStyle::BottomInnerBorderWidth, 2.0);
    bf.setProperty(KoParagraphStyle::TopInnerBorderWidth, 2.0);
    cursor.setBlockFormat(bf);

    m_layout->layout();
    block = m_doc->begin();
    blockLayout = block.layout();
    QCOMPARE(blockLayout->lineAt(0).x(), 15.0 + 100.0);
    QCOMPARE(blockLayout->lineAt(0).y(), 16.0 + 100.0);
    QCOMPARE(blockLayout->lineAt(0).width(), 200.0 - 8.0 - 11.0 - (5.0 + 2.0) * 2);
    block = block.next();
    blockLayout = block.layout();
    //qDebug() << blockLayout->lineAt(0).y();
    QVERIFY(qAbs(blockLayout->lineAt(0).y() - (9.0 + (12.0  * LINESPACING_FACTOR) + 10 + (5.0 + 2.0) * 2 + 100.0)) < ROUNDING);

    // and last, make the 2 qreal border have a blank space in the middle.
    bf.setProperty(KoParagraphStyle::LeftBorderSpacing, 3.0);
    bf.setProperty(KoParagraphStyle::RightBorderSpacing, 3.0);
    bf.setProperty(KoParagraphStyle::BottomBorderSpacing, 3.0);
    bf.setProperty(KoParagraphStyle::TopBorderSpacing, 3.0);
    cursor.setBlockFormat(bf);

    m_layout->layout();
    block = m_doc->begin();
    blockLayout = block.layout();
    QCOMPARE(blockLayout->lineAt(0).x(), 18.0 + 100.0);
    QCOMPARE(blockLayout->lineAt(0).y(), 19.0 + 100.0);
    QCOMPARE(blockLayout->lineAt(0).width(), 200.0 - 8.0 - 11.0 - (5.0 + 2.0 + 3.0) * 2);
    block = block.next();
    blockLayout = block.layout();
    //qDebug() << blockLayout->lineAt(0).y();
    QVERIFY(qAbs(blockLayout->lineAt(0).y() - (9.0 + (12.0  * LINESPACING_FACTOR) + 10 + (5.0 + 2.0 + 3.0) * 2 + 100.0)) < ROUNDING);
}

void TestBlockLayout::testParagraphMargins()
{
    setupTest("Emtpy\nParagraph\nAnother parag\n");

    KoParagraphStyle style;
    style.setFontPointSize(12.0);
    m_styleManager->add(&style);
    style.setTopMargin(QTextLength(QTextLength::FixedLength, 10));
    KoListStyle listStyle;
    KoListLevelProperties llp = listStyle.levelProperties(1);
    llp.setLabelType(KoListStyle::NumberLabelType);
    llp.setNumberFormat(KoOdfNumberDefinition::Numeric);
    listStyle.setLevelProperties(llp);
    style.setListStyle(&listStyle);
    style.setLeftBorderWidth(3);

    QTextBlock block = m_doc->begin().next();
    style.applyStyle(block);
    block = block.next();
    style.applyStyle(block);

    m_layout->layout();

    block = m_doc->begin().next();
    KoTextBlockData data(block);
    KoTextBlockBorderData *border = data.border();
    QVERIFY(border);
    QCOMPARE(data.counterPosition(), QPointF(3 + 100.0, (12.0 * LINESPACING_FACTOR) + 10.0 + 100.0));

    block = block.next();
    KoTextBlockData data2(block);
    QCOMPARE(data2.counterPosition(), QPointF(3 + 100.0, (12.0 * LINESPACING_FACTOR + 10) * 2 + 100.0));


    style.setBottomMargin(QTextLength(QTextLength::FixedLength, 5)); //bottom spacing
    // manually reapply and relayout to force immediate reaction.
    block = m_doc->begin().next();
    style.applyStyle(block);
    block = block.next();
    style.applyStyle(block);
    m_layout->layout();

    block = m_doc->begin().next();
    border = data2.border();
    QVERIFY(border);

    KoTextBlockData data3(block);
    QCOMPARE(data3.counterPosition(), QPointF(3 + 100.0, (12.0  * LINESPACING_FACTOR) + 10.0 + 100.0));

    block = block.next();
    KoTextBlockData data4(block);
    QCOMPARE(data4.counterPosition(), QPointF(3 + 100.0, (12.0  * LINESPACING_FACTOR + 10.0) * 2 + 100.0)); // same y as before as we take max spacing
}

void TestBlockLayout::testEmptyParag()
{
    setupTest("Foo\n\nBar\n");
    m_layout->layout();
    QTextBlock block = m_doc->begin();
    QTextLayout *lay = block.layout();
    QVERIFY(lay);
    QCOMPARE(lay->lineCount(), 1);
    const qreal y = lay->lineAt(0).position().y();

    block = block.next();
    lay = block.layout();
    QVERIFY(lay);
    QCOMPARE(lay->lineCount(), 1);
    QVERIFY(lay->lineAt(0).position().y() > y);
    qInfo()<<lay->lineAt(0).position().y();
    QVERIFY(qAbs(lay->lineAt(0).position().y() - (12.0*LINESPACING_FACTOR + 100.0)) < ROUNDING);
}

void TestBlockLayout::testDropCapsLongText()
{
    // This text should be so long that it is split into more lines
    // than the number of dropcap lines.
    // This is the normal use case, with a shorter line dropcaps doesn't work well
    setupTest(m_loremIpsum);

    KoParagraphStyle style;
    style.setFontPointSize(12.0);
    style.setDropCaps(false);
    style.setDropCapsLength(1);
    style.setDropCapsLines(4);
    style.setDropCapsDistance(9.0);
    QTextBlock block = m_doc->begin();
    style.applyStyle(block);

    qInfo()<<"Font:"<<style.font();
    qInfo()<<"Dropcaps off";

    m_layout->layout();

    // dummy version, caps is still false.
    QTextLayout *blockLayout = block.layout();
    QVERIFY(blockLayout->lineCount() > 4);
    QTextLine line = blockLayout->lineAt(0);
    QVERIFY(line.textLength() > 1);

    qInfo()<<"Dropcaps on";
    style.setDropCaps(true);
    style.applyStyle(block);
    m_layout->layout();

    // test that the first text line is the dropcaps and the positions are right.
    QVERIFY(blockLayout->lineCount() > 4);
    line = blockLayout->lineAt(0);
    QCOMPARE(line.textLength(), 1);

    QCOMPARE(line.x(), 100.0);
    QVERIFY(line.y() <= 100.0); // can't get a tight-boundingrect here.

    qInfo()<<"dropcaps:"<<line.rect();
    qreal dropCapsRight = line.rect().right();
    qreal dropCapsBottom = 100.0 - line.y() + line.height();

    line = blockLayout->lineAt(1);
    QVERIFY(line.textLength() > 1);
    qreal heightNormalLine = line.height();
    QCOMPARE(line.y(), 100.0); // aligned top
    QCOMPARE(line.x(), dropCapsRight + style.dropCapsDistance());

    line = blockLayout->lineAt(1);
    QVERIFY(line.textLength() > 2);
    QCOMPARE(line.y(), 100.0); // aligned top
    QCOMPARE(line.x(), dropCapsRight + style.dropCapsDistance());

    line = blockLayout->lineAt(3);
    QVERIFY(line.textLength() > 0);
    QCOMPARE(line.x(), dropCapsRight + style.dropCapsDistance());

    line = blockLayout->lineAt(4);
    QVERIFY(line.textLength() > 0);
    QCOMPARE(line.x(), dropCapsRight + style.dropCapsDistance());

    // This should be below the dropcap and thus not indented
    line = blockLayout->lineAt(5);
    QVERIFY(line.textLength() > 0);
    QCOMPARE(line.x(), 100.0); // aligned left
    qInfo()<<"dropCapsBottom"<<dropCapsBottom<<line.y();
    QVERIFY(line.y() >= dropCapsBottom);

    qInfo()<<"Dropcaps off";
    style.setDropCaps(false); // remove it
    style.applyStyle(block);
    m_layout->layout();
    blockLayout = block.layout();

    // test that the first text line is no longer dropcaps
    QVERIFY(blockLayout->lineCount() > 1);
    line = blockLayout->lineAt(0);
    QVERIFY(line.textLength() > 1);
    QCOMPARE(line.height(), heightNormalLine);
}

void TestBlockLayout::testDropCapsShortText()
{
    setupTest(QString("Lorem ipsum")); // short enough to only get one line

    KoParagraphStyle style;
    style.setFontPointSize(12.0);
    style.setDropCaps(false);
    style.setDropCapsLength(1);
    style.setDropCapsLines(4);
    style.setDropCapsDistance(9.0);
    QTextBlock block = m_doc->begin();
    style.applyStyle(block);

    qInfo()<<"Font:"<<style.font()<<"Text"<<block.text();
    qInfo()<<"Dropcaps off";

    m_layout->layout();

    // dummy version, caps is still false.
    QTextLayout *blockLayout = block.layout();
    int lineCount = blockLayout->lineCount();

    QVERIFY(lineCount > 0);
    QTextLine line = blockLayout->lineAt(0);
    QVERIFY(line.textLength() > 1);

    qInfo()<<"Dropcaps on";
    style.setDropCaps(true);
    style.applyStyle(block);
    m_layout->layout();

    // test that the first text line is the dropcaps and the positions are right.
    QCOMPARE(blockLayout->lineCount(), lineCount + 1);
    line = blockLayout->lineAt(0);
    QCOMPARE(line.textLength(), 1);

    QCOMPARE(line.x(), 100.0);
    QVERIFY(line.y() <= 100.0); // can't get a tight-boundingrect here.

    qreal dropCapsRight = line.rect().right();

    line = blockLayout->lineAt(1);
    QVERIFY(line.textLength() > 1);
    qreal heightNormalLine = line.height();
    QCOMPARE(line.y(), 100.0); // aligned top
    QCOMPARE(line.x(), dropCapsRight + style.dropCapsDistance());

    qInfo()<<"Dropcaps off";
    style.setDropCaps(false); // remove it
    style.applyStyle(block);
    m_layout->layout();
    blockLayout = block.layout();

    // test that the first text line is no longer dropcaps
    QCOMPARE(blockLayout->lineCount(), lineCount);
    line = blockLayout->lineAt(0);
    QVERIFY(line.textLength() > 1);
    QCOMPARE(line.height(), heightNormalLine);
}

void TestBlockLayout::testDropCapsWithNewline()
{
    // Some not too long text so the dropcap will be bigger than the block.
    // The text after newline will be in a separate block, but
    // shall also be indented by the dropcap.
    // Note that we cannot be certain of the number of lines we will get
    // as it depends on the actual available font.
    setupTest(QString("Lorem ipsum dolor sit amet, XgXgectetuer adiXiscing elit, sed diam\nsome more text"));

    KoParagraphStyle style;
    style.setFontPointSize(12.0);
    style.setDropCaps(false);
    style.setDropCapsLength(1);
    style.setDropCapsLines(4);
    style.setDropCapsDistance(9.0);
    QTextBlock block = m_doc->begin();
    QTextBlock secondblock = block.next(); // "some more text"
    style.applyStyle(block);

    qInfo()<<"Font:"<<style.font()<<"Text"<<block.text();
    qInfo()<<"Dropcaps off";

    m_layout->layout();

    // dummy version, caps is still false.
    QTextLayout *blockLayout = block.layout();
    int lineCount = blockLayout->lineCount();
    // lineCount must be >= 1 and <= 3 for this test to work
    if (lineCount == 0 || lineCount >= 3) {
        qWarning()<<"The text was not layouted in the required number of lines, so this test will fail";
    }
    QVERIFY(blockLayout->lineCount() >= 1);
    QVERIFY(blockLayout->lineCount() <= 3);
    QTextLine line = blockLayout->lineAt(0);
    QVERIFY(line.textLength() > 1);

    qInfo()<<"Dropcaps on";
    style.setDropCaps(true);
    style.applyStyle(block);
    m_layout->layout();

    // test that the first text line is the dropcaps and the positions are right.
    QVERIFY(blockLayout->lineCount() > 1);
    line = blockLayout->lineAt(0);
    QCOMPARE(line.textLength(), 1);

    QCOMPARE(line.x(), 100.0);
    QVERIFY(line.y() <= 100.0); // can't get a tight-boundingrect here.

    qreal dropCapsRight = line.rect().right();

    line = blockLayout->lineAt(1);
    QVERIFY(line.textLength() > 2);
    qreal heightNormalLine = line.height();
    QCOMPARE(line.y(), 100.0); // aligned top
    QCOMPARE(line.x(), dropCapsRight + style.dropCapsDistance());

    // Now test that a following block is indented by the same amount
    // since it also should be influenced by the dropcap
    blockLayout = secondblock.layout();
    QVERIFY(blockLayout->lineCount() >= 1);
    line = blockLayout->lineAt(0);
    QCOMPARE(line.x(), dropCapsRight + style.dropCapsDistance());

    qInfo()<<"Dropcaps off";
    style.setDropCaps(false); // remove it
    style.applyStyle(block);
    m_layout->layout();
    blockLayout = block.layout();

    // test that the first text line is no longer dropcaps
    QCOMPARE(blockLayout->lineCount(), lineCount);
    line = blockLayout->lineAt(0);
    QVERIFY(line.textLength() > 1);
    QCOMPARE(line.height(), heightNormalLine);
}

QTEST_MAIN(TestBlockLayout)
