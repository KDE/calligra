/*
  SPDX-FileCopyrightText: 2008 Brad Hards <bradh@frogmouth.net>

  SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "visio_tests.h"
#include <EnhMetaFile.h>

using namespace EnhancedMetafile;

void VisioTests::test1()
{
    Parser parser;
    DebugOutput output;
    parser.setOutput(&output);
    QVERIFY(parser.load(QString("visio-1.emf")));
}

void VisioTests::test_kde41()
{
    Parser parser;
    DebugOutput output;
    parser.setOutput(&output);
    QVERIFY(parser.load(QString("visio-kde41.emf")));
}

QTEST_MAIN(VisioTests)

#include <visio_tests.moc>
