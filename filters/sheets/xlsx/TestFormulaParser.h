/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * SPDX-FileCopyrightText: 2010 Sebastian Sauer <sebsauer@kdab.com>
 * SPDX-FileCopyrightText: 2009-2010 Nokia Corporation and /or its subsidiary(-ies).
 * SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */
#ifndef TEST_FORMULAPARSER_H
#define TEST_FORMULAPARSER_H

#include <QObject>

class TestFormulaParser : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testConvertFormula_data();
    void testConvertFormula();
    void testSharedFormulaReferences();
};

#endif // TEST_FORMULAPARSER_H
