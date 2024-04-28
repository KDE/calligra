/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2010 Johannes Simon <johannes.simon@gmail.com>
   SPDX-FileCopyrightText: 2010 Nokia Corporation and /or its subsidiary(-ies).
     Contact: Suresh Chande suresh.chande@nokia.com

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOCHART_TESTMULTIPLEFILES_H
#define KOCHART_TESTMULTIPLEFILES_H

#include "TestLoadingBase.h"

namespace KoChart
{

/**
 * Base class for testing of multiple files in one test
 */
class TestMultipleFiles : public QObject, public LoadingTests
{
    Q_OBJECT

public:
    TestMultipleFiles();

protected Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

protected:
    int currentDoc;
    QStringList documents;
};

} // namespace KoChart

#endif // KOCHART_TESTMULTIPLEFILES_H
