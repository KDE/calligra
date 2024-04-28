/* This file is part of the Calligra project
   SPDX-FileCopyrightText: 2008 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TESTVIEWMODE_H
#define TESTVIEWMODE_H

#include <QObject>
#include <QtTest>
#include <qtest_kde.h>

#include <KWViewModeNormal.h>

class MyViewModeNormal : public KWViewModeNormal
{
public:
    MyViewModeNormal()
    {
    }

    const QList<qreal> &pageTops()
    {
        return m_pageTops;
    }
    bool pageSpreadMode() const
    {
        return m_pageSpreadMode;
    }
    const QSizeF &contents() const
    {
        return m_contents;
    }
};

class TestViewMode : public QObject
{
    Q_OBJECT
private Q_SLOTS: // tests
    void init();
    void testBasicConversion();
    void testPageSetupChanged();
    void testClipRectForPageSpread();
};

#endif
