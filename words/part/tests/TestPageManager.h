/* This file is part of the Calligra project
 * SPDX-FileCopyrightText: 2005-2006, 2008 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef TESTPAGEMANAGER_H
#define TESTPAGEMANAGER_H

#include <QObject>
#include <QtTest>
#include <qtest_kde.h>

#include <tests/MockShapes.h> // from flake

class TestPageManager : public QObject
{
    Q_OBJECT
private Q_SLOTS: // tests
    void init();
    void getAddPages();
    void getAddPages2();
    void createInsertPages();
    void removePages();
    void pageInfo();
    void testClipToDocument();
    void testOrientationHint();
    void testDirectionHint();
    void testPageNumber();
    void testPageTraversal();
    void testSetPageStyle();
    void testPageCount();
    void testPageSpreadLayout();
    void testInsertPage();
    void testPadding();
    void testPageOffset();
    void testBackgroundRefCount();
    void testAppendPageSpread();
    void testRemovePageSpread();
};

#endif
