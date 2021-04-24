/* This file is part of the Calligra project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2008 Girish Ramakrishnan <girish@forwardbias.in>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef TESTSTYLES_H
#define TESTSTYLES_H

#include <QObject>

class TestStyles : public QObject
{
    Q_OBJECT
private Q_SLOTS: // tests
    void testStyleInheritance();
    void testChangeParent();
    void testTabsStorage();
    void testApplyParagraphStyle();
    void testApplyParagraphStyleWithParent();
    void testCopyParagraphStyle();
    void testUnapplyStyle();
};

#endif
