/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Elvis Stansvik <elvstone@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef TESTSTYLEMANAGER_H
#define TESTSTYLEMANAGER_H

#include <QObject>

class KoStyleManager;
class KoTextDocument;

class QTextDocument;

class TestStyleManager : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void init();
    void testAddRemoveCharacterStyle();
    void testAddRemoveParagraphStyle();
    void testAddRemoveListStyle();
    void testAddRemoveTableStyle();
    void testAddRemoveTableColumnStyle();
    void testAddRemoveTableRowStyle();
    void testAddRemoveTableCellStyle();
    void testAddRemoveSectionStyle();
    void testAddAppliedCharacterStyle();
    void testApplyAddedCharacterStyle();
    void testAddAppliedParagraphStyle();
    void testApplyAddedParagraphStyle();
    void cleanup();

private:
    KoStyleManager *m_styleManager;
    KoTextDocument *m_koDoc;
    QTextDocument *m_doc;
};

#endif // TESTSTYLEMANAGER_H
