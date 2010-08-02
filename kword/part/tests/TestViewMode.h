/* This file is part of the KOffice project
   Copyright (C) 2008 Thomas Zander <zander@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef TESTVIEWMODE_H
#define TESTVIEWMODE_H

#include <QObject>
#include <QtTest/QtTest>
#include <qtest_kde.h>

#include <KWViewModeNormal.h>

class MyViewModeNormal : public KWViewModeNormal
{
public:
    MyViewModeNormal() { }

    const QList<qreal>& pageTops() { return m_pageTops; }
    bool pageSpreadMode() const { return m_pageSpreadMode; }
    const QSizeF& contents() const { return  m_contents; }
};

class TestViewMode : public QObject
{
    Q_OBJECT
private slots: // tests
    void init();
    void testBasicConversion();
    void testPageSetupChanged();
    void testClipRectForPageSpread();
};

#endif
