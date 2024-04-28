/*
  SPDX-FileCopyrightText: 2008 Brad Hards <bradh@frogmouth.net>

  SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "snp_tests.h"
#include <EnhMetaFile.h>

using namespace EnhancedMetafile;

void SnpTests::test1()
{
    Parser parser;
    DebugOutput output;
    parser.setOutput(&output);
    QVERIFY(parser.load(QString("snp-1.emf")));
}

void SnpTests::test2()
{
    Parser parser;
    DebugOutput output;
    parser.setOutput(&output);
    QVERIFY(parser.load(QString("snp-2.emf")));
}

void SnpTests::test3()
{
    Parser parser;
    DebugOutput output;
    parser.setOutput(&output);
    QVERIFY(parser.load(QString("snp-3.emf")));
}

QTEST_MAIN(SnpTests)
#include <snp_tests.moc>
