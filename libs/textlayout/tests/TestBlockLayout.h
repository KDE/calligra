/*
 *  This file is part of Calligra tests
 *
 *  SPDX-FileCopyrightText: 2006-2010 Thomas Zander <zander@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef TESTBLOCKLAYOUT_H
#define TESTBLOCKLAYOUT_H

#include <QObject>

#include <KoTextDocumentLayout.h>
#include <KoTextLayoutRootArea.h>

class KoStyleManager;
class QTextDocument;

#define ROUNDING 0.126

class TestBlockLayout : public QObject
{
    Q_OBJECT
public:
    TestBlockLayout() = default;

    /// FIXME: fix these broken tests

    /// Test tabs.
    void testTabs_data();
    void testTabs();

private Q_SLOTS:
    void initTestCase();

    /// make sure empty paragraphs are initialized properly
    void testEmptyParag();

    /// Test breaking lines based on the width of the reference rect.
    void testLineBreaking();

    /// Tests incrementing Y pos based on the font size
    void testBasicLineSpacing();
    /// Tests incrementing Y pos based on the font size
    void testBasicLineSpacing2();
    /// Tests fixed linespacing.
    void testFixedLineSpacing();
    /// Tests percentage linespacing.
    void testPercentageLineSpacing();
    /// Tests advanced linespacing options provided in our style.
    void testAdvancedLineSpacing();
    /// Tests that empty lines are given the correct height like in LibreOffice
    void testEmptyLineHeights();

    /// Test distance above and below paragraphs.
    void testBlockSpacing();

    /// Test left and right margins of paragraphs.
    void testLeftRightMargins();

    /// Test first line indent of paragraphs.
    void testTextIndent();

    void testBasicTextAlignments();
    void testTextAlignments();

    // relativeBulletSize

    // etc
    // void testParagOffset();
    void testParagraphBorders();
    void testParagraphMargins();

    void testDropCapsLongText();
    void testDropCapsShortText();
    void testDropCapsWithNewline();

private:
    void setupTest(const QString &initText = QString());

private:
    QTextDocument *m_doc;
    KoTextDocumentLayout *m_layout;
    QTextBlock m_block;
    QString m_loremIpsum;
    KoStyleManager *m_styleManager;
    KoTextLayoutRootArea *m_area;
};

#endif
