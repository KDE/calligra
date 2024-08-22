/*
 *  This file is part of Calligra tests
 *
 *  SPDX-FileCopyrightText: 2006-2010 Thomas Zander <zander@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef TESTTABLELAYOUT_H
#define TESTTABLELAYOUT_H

#include <QObject>

#include <KoTextDocumentLayout.h>
#include <KoTextLayoutRootArea.h>
#include <KoTextLayoutTableArea.h>

class KoStyleManager;
class KoTableStyle;
class QTextDocument;
class QTextTable;

#define ROUNDING 0.126

class TestTableLayout : public QObject
{
    Q_OBJECT
public:
    TestTableLayout() = default;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    /**
     * make sure our private method setupTest() does what we think it does
     */
    void testSetupTest();

    /**
     * Test merged cells. In this testcase the table is modelled after a special case where we happened
     * to end in an infinite layout-loop in the past. The loop was caused by us not being able to place
     * content in a cell and therefore we keeped on to ask for new root-areas. This test should verify
     * exactly this case.
     */
    void testMergedCells();
    /**
     * If no column-width is defined then the available width should be distributed among
     * the available columns. Since the provided rootArea's have a width of 200 and we
     * have 3 columns it is expected that every of the columns has a width of 200/3.
     */
    void testColumnWidthUndefined();

    /**
     * If the column-width is explicit defined then we expect that those widths are used.
     */
    void testColumnWidthFixed();

    /**
     * Test fixed column-width of zero.
     */
    void testColumnWidthFixedZero();

    /**
     * If the table-width is not defined then the table get's the width of it's parent
     * what is the rootArea in our case. If now the defined fixed column-width's are
     * in total larger then those table-width then they need to be shrink proportional
     * to match into the available table-width.
     */
    void testColumnWidthFixedShrink();

    /**
     * Test relative column-width.
     */
    void testColumnWidthRelative();

    /**
     * Test fixed row height. The row is smaller then the text that is within the cells. Expected
     * is that the resulting table will still match to the available space and won't be spread
     * over multiple pages. This tests for bug #275409.
     */
    void testRowHeightFixed();

    /**
     * Test minimum row-height.
     */
    void testRowHeightMinimum();

    void testOneTableNeedsTwoRootAreas();
    void testTwoTablesNeedsTwoRootAreas();
    void testMergedRowsSpansAreas();
    void testMergedRowsSpansAreas2();
    void testTwoTablesMergedRowsSpansAreas();
    void testTwoTablesMergedRowsSpansThreeAreas();
    void testRowHeightLargerThanAreaHeight();

    // With header rows
    /// Table consisting of a single header row
    void testSingleHeaderRow();
    /// Table consisting of a two header rows
    void testMultipleHeaderRows();
    /// Table consisting two rows with a single header row
    void testTableWithSingleHeaderRow();
    /// Table consisting three rows with two header rows
    void testTableWithMultipleHeaderRow();
    /// Table consisting of a single header row
    /// with height > root area height
    void testHeaderRowHeightLargerThanAreaHeight();

private:
    QTextCursor setupTest();
    void setupTest(const QString &mergedText,
                   const QString &topRightText,
                   const QString &midRightText,
                   const QString &bottomLeftText,
                   const QString &bottomMidText,
                   const QString &bottomRightText,
                   KoTableStyle *tableStyle = nullptr);
    QTextTable *addTable(QTextCursor cursor, int rows, int columns, KoTableStyle *tableStyle = nullptr);

private:
    QTextDocument *m_doc;
    KoTextDocumentLayout *m_layout;
    QTextBlock m_block;
    QTextBlock mergedCellBlock() const;
    QTextBlock topRightCellBlock() const;
    QTextBlock midRightCellBlock() const;
    QTextBlock bottomLeftCellBlock() const;
    QTextBlock bottomMidCellBlock() const;
    QTextBlock bottomRightCellBlock() const;
    QString m_loremIpsum;
    KoStyleManager *m_styleManager;
    QTextTable *m_table;
};

#endif
