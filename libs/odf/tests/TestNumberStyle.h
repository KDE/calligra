/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Sebastian Sauer <sebsauer@kdab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef TESTNUMBERSTYLE_H
#define TESTNUMBERSTYLE_H

#include <QObject>

class TestNumberStyle : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void testEmpty();
    void testText();
    void testNumber();
    void testDate();
    void testTime();
    void testBoolean();
    void testPercent();
    void testScientific();
    void testFraction();
    void testCurrency();
};

#endif
