/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2010 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef TESTFINDMATCH_H
#define TESTFINDMATCH_H

#include <QObject>

class TestFindMatch : public QObject
{
    Q_OBJECT
public:
private Q_SLOTS:
    void testBasic();
    void testCopyAssign();
};

#endif // TESTFINDMATCH_H
