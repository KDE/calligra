/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@kogmbh.com>
 * SPDX-FileCopyrightText: 2012 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef TEST_KO_BOOKMARK_MANAGER_H
#define TEST_KO_BOOKMARK_MANAGER_H

#include <QObject>

class TestKoBookmarkManager : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testCreation();
    void testInsertAndRetrieve();
    void testRemove();
    void testRename();
};

#endif // TEST_KO_BOOKMARK_MANAGER_H
