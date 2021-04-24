/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2010 Johannes Simon <johannes.simon@gmail.com>
   SPDX-FileCopyrightText: 2010 Nokia Corporation and /or its subsidiary(-ies).
     Contact: Suresh Chande suresh.chande@nokia.com

   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KCHART_TESTCELLREGION_H
#define KCHART_TESTCELLREGION_H

// Qt
#include <QObject>
#include <QStandardItemModel>

// KoChart
#include "CellRegion.h"
#include "TableSource.h"

using namespace KoChart;

class TestCellRegion : public QObject
{
    Q_OBJECT
public:
    TestCellRegion();

private Q_SLOTS:
    void init();
    void testToStringSingleTable();
    void testSkippedTableEntry();
    void testFromStringSingleTable();
    void testToStringWithSpecialCharactersSingleTable();
    void testFromStringWithSpecialCharactersSingleTable();
    void testTableNameChangeSingleTable();
    void testToStringMultipleTables();
    void testFromStringMultipleTables();
    void testToStringWithSpecialCharactersMultipleTables();
    void testFromStringWithSpecialCharactersMultipleTables();
    void testTableNameChangeMultipleTables();
    void testListOfRegions();
    void testListOfRegions2();

private:
    TableSource m_source;
    CellRegion m_region1, m_region2;
    QStandardItemModel m_model1, m_model2;
};

#endif // KCHART_TESTCELLREGION_H
