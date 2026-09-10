/*
 *  This file is part of Calligra tests
 *
 *  SPDX-FileCopyrightText: 2006-2010 Thomas Zander <zander@kde.org>
 *  SPDX-FileCopyrightText: 2009-2010 C. Boemann <cbo@kogmbh.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef TESTDOCUMENTLAYOUT_H
#define TESTDOCUMENTLAYOUT_H

#include <QObject>
#include <QPainter>

class QTextDocument;
class KoTextDocumentLayout;
class KoStyleManager;
class KoParagraphStyle;
class MockRootAreaProvider;

class TestDocumentLayout : public QObject
{
    Q_OBJECT
public:
    TestDocumentLayout() = default;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    /**
     * Test the hittest of KoTextDocumentLayout.
     */
    void testHitTest();
    void testInlineObjectCallbacks();

    /**
     * Test root-area with zero width.
     */
    void testRootAreaZeroWidth();

    /**
     * Test root-area with zero height.
     */
    void testRootAreaZeroHeight();

    /**
     * Test root-area with zero width and height.
     */
    void testRootAreaZeroWidthAndHeight();

private:
    void setupTest(const QString &initText = QString());
    void cleanupTest();

private:
    QTextDocument *m_doc;
    KoStyleManager *m_styleManager;
    KoTextDocumentLayout *m_layout;
    MockRootAreaProvider *m_provider;
    KoParagraphStyle *m_paragraphStyle;
};

#endif
