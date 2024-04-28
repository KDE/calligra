/* This file is part of the Calligra project
 * SPDX-FileCopyrightText: 2010 KO GmbH <cbo@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "TestSectionStyle.h"

#include "styles/KoSectionStyle.h"

#include <QTest>
#include <QTextFrameFormat>

void TestSectionStyle::testSectionStyle()
{
    // Test basic functionality of the section style (roundtripping to format and back).
    KoSectionStyle *style1 = new KoSectionStyle();
    QVERIFY(style1);
    style1->setLeftMargin(1.0);
    style1->setRightMargin(2.0);

    QTextFrameFormat format;
    style1->applyStyle(format);

    KoSectionStyle *style2 = new KoSectionStyle(format);
    QVERIFY(style2);
    QCOMPARE(style2->leftMargin(), 1.0);
    QCOMPARE(style2->rightMargin(), 2.0);
}

QTEST_MAIN(TestSectionStyle)
