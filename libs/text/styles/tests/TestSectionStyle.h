/* This file is part of the Calligra project
 * SPDX-FileCopyrightText: 2010 KO GmbH <cbo@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef TESTSECTIONSTYLE_H
#define TESTSECTIONSTYLE_H

#include <QObject>

class TestSectionStyle : public QObject
{
    Q_OBJECT
public:
    TestSectionStyle() = default;

private Q_SLOTS:
    void testSectionStyle();
};

#endif // TESTSECTIONSTYLE_H
