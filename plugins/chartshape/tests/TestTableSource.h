/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2010 Johannes Simon <johannes.simon@gmail.com>
   SPDX-FileCopyrightText: 2010 Nokia Corporation and /or its subsidiary(-ies).
     Contact: Suresh Chande suresh.chande@nokia.com

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCHART_TESTTABLESOURCE_H
#define KCHART_TESTTABLESOURCE_H

// Qt
#include <QObject>
#include <QStandardItemModel>

// KoChart
#include "TableSource.h"

using namespace KoChart;

class TestTableSource : public QObject
{
    Q_OBJECT

public:
    TestTableSource();

private Q_SLOTS:
    void init();
    void testAdding();
    void testRenaming();
    void testRemoval();

    /**
     * Tests for sheet access model functionality
     */
    void testAdding_SAM();
    void testRenaming_SAM();
    void testRemoval_SAM();

private:
    TableSource m_source;
    QStandardItemModel m_table1, m_table2, m_table3, m_table4;
    QStandardItemModel m_sheetAccessModel;
};

#endif // KCHART_TESTTABLESOURCE_H
