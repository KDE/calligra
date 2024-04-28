/*
 *  This file is part of Calligra tests
 *
 *  SPDX-FileCopyrightText: 2006-2010 Thomas Zander <zander@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef TESTDOCUMENTAYOUT_H
#define TESTDOCUMENTAYOUT_H

#include <QObject>
#include <qtest_kde.h>

#include "../TextShapeLayout.h"
#include "MockTextShape.h"

#include <KoShape.h>
#include <KoTextDocumentLayout.h>
#include <KoTextShapeData.h>

class KoStyleManager;
class QTextDocument;

#define ROUNDING 0.126

class TestDocumentLayout : public QObject
{
    Q_OBJECT
public:
    TestDocumentLayout()
    {
    }

private Q_SLOTS:
    void initTestCase();

    /// make sure empty paragraphs are initialized properly
    void testEmptyParag();

    /// Test breaking lines based on the width of the shape.
    void testLineBreaking();
    /// Test breaking lines for frames with different widths.
    void testMultiFrameLineBreaking();
    /// Tests incrementing Y pos based on the font size
    void testBasicLineSpacing();
    /// Tests incrementing Y pos based on the font size
    void testBasicLineSpacing2();
    /// Tests advanced linespacing options provided in our style.
    void testAdvancedLineSpacing();
    /// test data integrity for multiple shapes.
    void testShapePosition();
    void testShapePosition2();

    // Block styles
    /// Test top, left, right and bottom margins of paragraphs.
    void testMargins();
    void testMultipageMargins();
    void testTextIndent();
    void testBasicTextAlignments();
    void testTextAlignments();
    void testPageBreak();
    void testPageBreak2();
    void testNonBreakableLines();

    // Lists
    void testBasicList();
    void testNumberedList();
    void testInterruptedLists(); // consecutiveNumbering
    void testNestedLists();
    void testNestedPrefixedLists();
    void testAutoRestartList();
    void testListParagraphIndent();
    void testRomanNumbering();
    void testUpperAlphaNumbering();
    void testRestartNumbering();
    void testRightToLeftList();
    void testLetterSynchronization();
    void testInvalidateLists();
    void testCenteredItems();
    void testMultiLevel();

    // relativeBulletSize

    // etc
    void testParagOffset();
    void testParagraphBorders();
    void testBorderData();
    void testDropCaps();

private:
    void initForNewTest(const QString &initText = QString());

private:
    MockTextShape *m_shape1;
    QTextDocument *m_doc;
    KoTextDocumentLayout *m_layout;
    QTextBlock m_block;
    QString m_loremIpsum;
    KoStyleManager *m_styleManager;
    TextShapeLayout *m_textLayout;
};

#endif
