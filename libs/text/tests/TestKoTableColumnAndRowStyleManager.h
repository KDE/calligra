/*
 *  This file is part of Calligra tests
 *
 *  SPDX-FileCopyrightText: 2009-2010 C. Boemann <cbo@kogmbh.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef TESTKOTABLECOLUMNANDROWSTYLEMANAGER_H
#define TESTKOTABLECOLUMNANDROWSTYLEMANAGER_H

#include <QObject>

class TestKoTableColumnAndRowStyleManager : public QObject
{
    Q_OBJECT
public:
    TestKoTableColumnAndRowStyleManager() = default;

private Q_SLOTS:
    // basic testing of the columns insertion and removal
    void testColumns();
    // basic testing of the rows insertion and removal
    void testRows();
};

#endif // TESTKOTABLECOLUMNANDROWSTYLEMANAGER_H
