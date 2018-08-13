/* This file is part of the KDE project

   Copyright 2010 Johannes Simon <johannes.simon@gmail.com>
   Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
     Contact: Suresh Chande suresh.chande@nokia.com

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KOCHART_TESTMULTIPLEFILES_H
#define KOCHART_TESTMULTIPLEFILES_H

#include "TestLoadingBase.h"


class QStringList;

namespace KoChart {

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
