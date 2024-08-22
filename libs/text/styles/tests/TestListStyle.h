/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007-2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef TESTLISTSTYLE_H
#define TESTLISTSTYLE_H

#include <QObject>

class TestListStyle : public QObject
{
    Q_OBJECT
public:
    TestListStyle() = default;

private Q_SLOTS:
    void testListStyle();
};

#endif
