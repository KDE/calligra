/*
 *  SPDX-FileCopyrightText: 2005 Adrian Page <adrian@pagenet.plus.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "TestKoIntegerMaths.h"
#include "KoIntegerMaths.h"

#include <QTest>

void TestKoIntegerMaths::UINT8Tests()
{
    QCOMPARE((int)UINT8_MULT(0, 255), 0);
    QCOMPARE((int)UINT8_MULT(255, 255), 255);

    QCOMPARE((int)UINT8_MULT(128, 255), 128);
    QCOMPARE((int)UINT8_MULT(255, 128), 128);

    QCOMPARE((int)UINT8_MULT(1, 255), 1);
    QCOMPARE((int)UINT8_MULT(1, 127), 0);
    QCOMPARE((int)UINT8_MULT(64, 128), 32);

    QCOMPARE((int)UINT8_DIVIDE(255, 255), 255);
    QCOMPARE((int)UINT8_DIVIDE(64, 128), 128);
    QCOMPARE((int)UINT8_DIVIDE(1, 64), 4);
    QCOMPARE((int)UINT8_DIVIDE(0, 1), 0);

    for (int i = 0; i < 256; i++) {
        QCOMPARE((int)UINT8_BLEND(255, 0, i), i);
    }
    for (int i = 0; i < 256; i++) {
        QCOMPARE((int)UINT8_BLEND(0, 255, i), int(255 - i));
    }
    for (int i = 0; i < 256; i++) {
        QVERIFY(qAbs(int(UINT8_BLEND(0, i, 128)) - int(i * (255 - 128) / 255.0 + 0.5)) <= 1);
    }
    QCOMPARE((int)UINT8_BLEND(255, 128, 128), 192);
    QCOMPARE((int)UINT8_BLEND(128, 64, 255), 128);
}

void TestKoIntegerMaths::UINT16Tests()
{
    QCOMPARE((int)UINT16_MULT(0, 65535), 0);
    QCOMPARE((int)UINT16_MULT(65535, 65535), 65535);

    QCOMPARE((int)UINT16_MULT(32768, 65535), 32768);
    QCOMPARE((int)UINT16_MULT(65535, 32768), 32768);

    QCOMPARE((int)UINT16_MULT(1, 65535), 1);
    QCOMPARE((int)UINT16_MULT(1, 32767), 0);
    QCOMPARE((int)UINT16_MULT(16384, 32768), 8192);

    QCOMPARE((int)UINT16_DIVIDE(65535, 65535), 65535);
    QCOMPARE((int)UINT16_DIVIDE(16384, 32768), 32768);
    QCOMPARE((int)UINT16_DIVIDE(1, 16384), 4);
    QCOMPARE((int)UINT16_DIVIDE(0, 1), 0);

    QCOMPARE((int)UINT16_BLEND(65535, 0, 0), 0);
    // All these tests gave off-by one errors that apparently aren't
    // errors.
    // So -- are we officially expecting 32767 instead of 32768 and
    // 49151 instead of 49152 here?
    QCOMPARE((int)UINT16_BLEND(65535, 0, 32768), 32767);
    QCOMPARE((int)UINT16_BLEND(65535, 32768, 32768), 49151);
    QCOMPARE((int)UINT16_BLEND(32768, 16384, 65535), 32767);
}

void TestKoIntegerMaths::conversionTests()
{
    QCOMPARE((int)UINT8_TO_UINT16(255), 65535);
    QCOMPARE((int)UINT8_TO_UINT16(254), 65278); // Erm, is this right?
    QCOMPARE((int)UINT8_TO_UINT16(0), 0);
    QCOMPARE((int)UINT8_TO_UINT16(128), 128 * 257);

    QCOMPARE((int)UINT16_TO_UINT8(65535), 255);
    QCOMPARE((int)UINT16_TO_UINT8(65280), 254);
    QCOMPARE((int)UINT16_TO_UINT8(0), 0);
    QCOMPARE((int)UINT16_TO_UINT8(128 * 257), 128);
}

QTEST_GUILESS_MAIN(TestKoIntegerMaths)
