/*
 *  SPDX-FileCopyrightText: 2007 Boudewijn Rempt boud @valdyas.org
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPROGRESSUPDATER_TEST_H
#define KOPROGRESSUPDATER_TEST_H

#include <QObject>
#include <ThreadWeaver/Job>

class KoProgressUpdaterTest : public QObject
{
    Q_OBJECT

public Q_SLOTS:

    void jobDone(ThreadWeaver::JobPointer);

private Q_SLOTS:

    void testCreation();
    void testSimpleProgress();
    void testSubUpdaters();
    void testThreadedSubUpdaters();
    void testSimpleThreadedProgress();
    void testRecursiveProgress();
    void testThreadedRecursiveProgress();
    void testFromWeaver();

private:

    int jobsdone;
};
#endif
