/*
  SPDX-FileCopyrightText: 2008 Brad Hards <bradh@frogmouth.net>

  SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "bad_header.h"
#include <EnhMetaFile.h>

using namespace EnhancedMetafile;

void BadHeader::checkWmfHeader()
{
    QTest::ignoreMessage(QtWarningMsg, "Failed to parse header, perhaps not an EMF file");
    Parser parser;
    DebugOutput output;
    parser.setOutput(&output);
    QVERIFY(!parser.load(QString("cof.wmf")));
}

QTEST_MAIN(BadHeader)
#include <bad_header.moc>
