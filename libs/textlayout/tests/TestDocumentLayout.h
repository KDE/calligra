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

class TestDocumentLayout : public QObject
{
    Q_OBJECT
public:
    TestDocumentLayout() = default;

private Q_SLOTS:
    void initTestCase();

    /**
     * Test the hittest of KoTextDocumentLayout.
     */
    void testHitTest();

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

private:
    QTextDocument *m_doc;
    KoStyleManager *m_styleManager;
    KoTextDocumentLayout *m_layout;
};

#endif
