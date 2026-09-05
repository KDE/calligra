/*
 *  SPDX-FileCopyrightText: 2008 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "TestConvolutionOpImpl.h"

#include <QTest>

#include <array>

#include "../DebugPigment.h"
#include "../KoColorSpaceAbstract.h"
#include "../KoColorSpaceTraits.h"

void TestConvolutionOpImpl::testConvolutionOpImpl()
{
    KoConvolutionOpImpl<KoBgrU16Traits> op;
    std::array<quint8, KoBgrU16Traits::pixelSize> color0;
    std::array<quint8, KoBgrU16Traits::pixelSize> color1;
    std::array<quint8, KoBgrU16Traits::pixelSize> color2;
    ((quint16 *)color0.data())[0] = 100;
    ((quint16 *)color0.data())[1] = 200;
    ((quint16 *)color0.data())[2] = 300;
    ((quint16 *)color0.data())[3] = 0xFFFF;
    ((quint16 *)color1.data())[0] = 50;
    ((quint16 *)color1.data())[1] = 150;
    ((quint16 *)color1.data())[2] = 0;
    ((quint16 *)color1.data())[3] = 0xFFFF;
    ((quint16 *)color2.data())[0] = 100;
    ((quint16 *)color2.data())[1] = 300;
    ((quint16 *)color2.data())[2] = 50;
    ((quint16 *)color2.data())[3] = 0xFFFF;
    std::array<const quint8 *, 3> colors = {color0.data(), color1.data(), color2.data()};
    std::array<quint8, KoBgrU16Traits::pixelSize> dst;
    quint16 *dst16 = (quint16 *)dst.data();
    {
        memcpy(dst16, colors[0], KoBgrU16Traits::pixelSize);
        QVERIFY2(dst16[0] == 100, QString("%1 100").arg(dst16[0]).toLatin1());
        QVERIFY2(dst16[1] == 200, QString("%1 200").arg(dst16[1]).toLatin1());
        QVERIFY2(dst16[2] == 300, QString("%1 300").arg(dst16[2]).toLatin1());
        QVERIFY2(dst16[3] == 0xFFFF, QString("%1 0xFFFF").arg(dst16[3]).toLatin1());
        memcpy(dst16, colors[1], KoBgrU16Traits::pixelSize);
        QVERIFY2(dst16[0] == 50, QString("%1 50").arg(dst16[0]).toLatin1());
        QVERIFY2(dst16[1] == 150, QString("%1 150").arg(dst16[1]).toLatin1());
        QVERIFY2(dst16[2] == 0, QString("%1 0").arg(dst16[2]).toLatin1());
        QVERIFY2(dst16[3] == 0xFFFF, QString("%1 0xFFFF").arg(dst16[3]).toLatin1());
        memcpy(dst16, colors[2], KoBgrU16Traits::pixelSize);
        QVERIFY2(dst16[0] == 100, QString("%1 100").arg(dst16[0]).toLatin1());
        QVERIFY2(dst16[1] == 300, QString("%1 300").arg(dst16[1]).toLatin1());
        QVERIFY2(dst16[2] == 50, QString("%1 50").arg(dst16[2]).toLatin1());
        QVERIFY2(dst16[3] == 0xFFFF, QString("%1 0xFFFF").arg(dst16[3]).toLatin1());
    }

    memset(dst16, 0, KoBgrU16Traits::pixelSize);

    // Tests for Case A)

    {
        qreal kernelValues[] = {1, 1, 1};
        op.convolveColors(colors.data(), kernelValues, dst.data(), 1, 0, 3, QBitArray());
        dbgPigment << dst16[0] << " " << dst16[1] << " " << dst16[2] << " " << QBitArray().isEmpty();
        QVERIFY2(dst16[0] == 250, QString("%1 250").arg(dst16[0]).toLatin1());
        QVERIFY2(dst16[1] == 650, QString("%1 650").arg(dst16[1]).toLatin1());
        QVERIFY2(dst16[2] == 350, QString("%1 350").arg(dst16[2]).toLatin1());
        QVERIFY2(dst16[3] == 0xFFFF, QString("%1 0xFFFF").arg(dst16[3]).toLatin1());
        op.convolveColors(colors.data(), kernelValues, dst.data(), 3, 0, 3, QBitArray());
        QVERIFY2(dst16[0] == 83, QString("%1 83").arg(dst16[0]).toLatin1());
        QVERIFY2(dst16[1] == 216, QString("%1 216").arg(dst16[1]).toLatin1());
        QVERIFY2(dst16[2] == 116, QString("%1 116").arg(dst16[2]).toLatin1());
        QVERIFY2(dst16[3] == 0xFFFF, QString("%1 0xFFFF").arg(dst16[3]).toLatin1());
    }
    {
        qreal kernelValues[] = {-1, 1, -1};
        op.convolveColors(colors.data(), kernelValues, dst.data(), 1, 0, 3, QBitArray());
        QVERIFY2(dst16[0] == 0, QString("%1 0").arg(dst16[0]).toLatin1());
        QVERIFY2(dst16[1] == 0, QString("%1 0").arg(dst16[1]).toLatin1());
        QVERIFY2(dst16[2] == 0, QString("%1 0").arg(dst16[2]).toLatin1());
        QVERIFY2(dst16[3] == 0, QString("%1 0").arg(dst16[3]).toLatin1());
    }
    {
        qreal kernelValues[] = {1, 2, 1};
        op.convolveColors(colors.data(), kernelValues, dst.data(), 1, 0, 3, QBitArray());
        QVERIFY2(dst16[0] == 300, QString("%1 300").arg(dst16[0]).toLatin1());
        QVERIFY2(dst16[1] == 800, QString("%1 800").arg(dst16[1]).toLatin1());
        QVERIFY2(dst16[2] == 350, QString("%1 350").arg(dst16[2]).toLatin1());
        QVERIFY2(dst16[3] == 0xFFFF, QString("%1 0xFFFF").arg(dst16[3]).toLatin1());
    }
    {
        qreal kernelValues[] = {1, -1, 1};
        op.convolveColors(colors.data(), kernelValues, dst.data(), 1, 0, 3, QBitArray());
        QVERIFY2(dst16[0] == 150, QString("%1 150").arg(dst16[0]).toLatin1());
        QVERIFY2(dst16[1] == 350, QString("%1 350").arg(dst16[1]).toLatin1());
        QVERIFY2(dst16[2] == 350, QString("%1 350").arg(dst16[2]).toLatin1());
        QVERIFY2(dst16[3] == 0xFFFF, QString("%1 0xFFFF").arg(dst16[3]).toLatin1());
    }
    {
        qreal kernelValues[] = {1, -1, 1};
        op.convolveColors(colors.data(), kernelValues, dst.data(), 1, 100, 3, QBitArray());
        QVERIFY2(dst16[0] == 250, QString("%1 250").arg(dst16[0]).toLatin1());
        QVERIFY2(dst16[1] == 450, QString("%1 450").arg(dst16[1]).toLatin1());
        QVERIFY2(dst16[2] == 450, QString("%1 450").arg(dst16[2]).toLatin1());
        QVERIFY2(dst16[3] == 0xFFFF, QString("%1 0xFFFF").arg(dst16[3]).toLatin1());
        op.convolveColors(colors.data(), kernelValues, dst.data(), 1, -100, 3, QBitArray());
        QVERIFY2(dst16[0] == 50, QString("%1 50").arg(dst16[0]).toLatin1());
        QVERIFY2(dst16[1] == 250, QString("%1 250").arg(dst16[1]).toLatin1());
        QVERIFY2(dst16[2] == 250, QString("%1 250").arg(dst16[2]).toLatin1());
        QVERIFY2(dst16[3] == 0xFF9B, QString("%1 0xFF9B").arg(dst16[3]).toLatin1());
    }
}

void TestConvolutionOpImpl::testOneSemiTransparent()
{
    KoConvolutionOpImpl<KoBgrU16Traits> op;
    quint8 color0[KoBgrU16Traits::pixelSize];
    quint8 color1[KoBgrU16Traits::pixelSize];
    quint8 color2[KoBgrU16Traits::pixelSize];
    ((quint16 *)color0)[0] = 100;
    ((quint16 *)color0)[1] = 200;
    ((quint16 *)color0)[2] = 300;
    ((quint16 *)color0)[3] = 0x00FF;
    ((quint16 *)color1)[0] = 50;
    ((quint16 *)color1)[1] = 150;
    ((quint16 *)color1)[2] = 0;
    ((quint16 *)color1)[3] = 0xFFFF;
    ((quint16 *)color2)[0] = 100;
    ((quint16 *)color2)[1] = 300;
    ((quint16 *)color2)[2] = 50;
    ((quint16 *)color2)[3] = 0xFFFF;
    const quint8 *colors[3] = {color0, color1, color2};
    quint8 dst[KoBgrU16Traits::pixelSize];
    quint16 *dst16 = (quint16 *)dst;
    memset(dst16, 0, KoBgrU16Traits::pixelSize);

    {
        // Tests for Case A)

        qreal kernelValues[] = {1, 1, 1};
        op.convolveColors(colors, kernelValues, dst, 1, 0, 3, QBitArray());
        QVERIFY2(dst16[0] == 250, QString("%1 250").arg(dst16[0]).toLatin1());
        QVERIFY2(dst16[1] == 650, QString("%1 650").arg(dst16[1]).toLatin1());
        QVERIFY2(dst16[2] == 350, QString("%1 350").arg(dst16[2]).toLatin1());

        QVERIFY2(dst16[3] == 0xFFFF, QString("%1 0xFFFF").arg(dst16[3]).toLatin1());

        op.convolveColors(colors, kernelValues, dst, 3, 0, 3, QBitArray());
        QVERIFY2(dst16[0] == 83, QString("%1 83").arg(dst16[0]).toLatin1());
        QVERIFY2(dst16[1] == 216, QString("%1 216").arg(dst16[1]).toLatin1());
        QVERIFY2(dst16[2] == 116, QString("%1 116").arg(dst16[2]).toLatin1());

        QVERIFY2(dst16[3] == 0xAAFF, QString("%1 0xAAFF").arg(dst16[3]).toLatin1());
    }
}

void TestConvolutionOpImpl::testOneFullyTransparent()
{
    KoConvolutionOpImpl<KoBgrU16Traits> op;
    quint8 color0[KoBgrU16Traits::pixelSize];
    quint8 color1[KoBgrU16Traits::pixelSize];
    quint8 color2[KoBgrU16Traits::pixelSize];
    ((quint16 *)color0)[0] = 100;
    ((quint16 *)color0)[1] = 200;
    ((quint16 *)color0)[2] = 300;
    ((quint16 *)color0)[3] = 0x0000;
    ((quint16 *)color1)[0] = 50;
    ((quint16 *)color1)[1] = 150;
    ((quint16 *)color1)[2] = 0;
    ((quint16 *)color1)[3] = 0xFFFF;
    ((quint16 *)color2)[0] = 100;
    ((quint16 *)color2)[1] = 300;
    ((quint16 *)color2)[2] = 50;
    ((quint16 *)color2)[3] = 0xFFFF;
    const quint8 *colors[3] = {color0, color1, color2};
    quint8 dst[KoBgrU16Traits::pixelSize];
    quint16 *dst16 = (quint16 *)dst;
    memset(dst16, 0, KoBgrU16Traits::pixelSize);

    {
        qreal kernelValues[] = {1, 1, 1};

        // Test for Case C)
        op.convolveColors(colors, kernelValues, dst, 1, 0, 3, QBitArray());
        QVERIFY2(dst16[0] == 225, QString("%1 225").arg(dst16[0]).toLatin1());
        QVERIFY2(dst16[1] == 675, QString("%1 675").arg(dst16[1]).toLatin1());
        QVERIFY2(dst16[2] == 75, QString("%1  75").arg(dst16[2]).toLatin1());
        QVERIFY2(dst16[3] == 0xFFFF, QString("%1 0xFFFF").arg(dst16[3]).toLatin1());

        // Test for Case B)
        op.convolveColors(colors, kernelValues, dst, 3, 0, 3, QBitArray());
        QVERIFY2(dst16[0] == 75, QString("%1 75").arg(dst16[0]).toLatin1());
        QVERIFY2(dst16[1] == 225, QString("%1 225").arg(dst16[1]).toLatin1());
        QVERIFY2(dst16[2] == 25, QString("%1 25").arg(dst16[2]).toLatin1());
        QVERIFY2(dst16[3] == 0xAAAA, QString("%1 0xAAAA").arg(dst16[3]).toLatin1());

        // Test for Case C)
        op.convolveColors(colors, kernelValues, dst, 15, 0, 3, QBitArray());
        QVERIFY2(dst16[0] == 15, QString("%1 15").arg(dst16[0]).toLatin1());
        QVERIFY2(dst16[1] == 45, QString("%1 45").arg(dst16[1]).toLatin1());
        QVERIFY2(dst16[2] == 5, QString("%1  5").arg(dst16[2]).toLatin1());
        QVERIFY2(dst16[3] == 0x2222, QString("%1 0x2222").arg(dst16[3]).toLatin1());
    }
}

QTEST_GUILESS_MAIN(TestConvolutionOpImpl)
