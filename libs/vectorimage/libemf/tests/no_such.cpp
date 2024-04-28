/*
  SPDX-FileCopyrightText: 2008 Brad Hards <bradh@frogmouth.net>

  SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "no_such.h"
#include <EnhMetaFile.h>

using namespace EnhancedMetafile;

void NoSuch::checkNonExisting()
{
    QTest::ignoreMessage(QtWarningMsg, "Request to load file (nosuchfilename.emf) that does not exist");
    Parser parser;
    QVERIFY(!parser.load(QString("nosuchfilename.emf")));
}

QTEST_MAIN(NoSuch)
#include <no_such.moc>
