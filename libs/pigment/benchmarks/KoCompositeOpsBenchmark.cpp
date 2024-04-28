/*
 *  SPDX-FileCopyrightText: 2010 Lukáš Tvrdý <lukast.dev@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "KoCompositeOpsBenchmark.h"

#include "../compositeops/KoCompositeOpAlphaDarken.h"
#include "../compositeops/KoCompositeOpOver.h"
#include <KoOptimizedCompositeOpFactory.h>

#include <KoColorSpaceRegistry.h>
#include <KoColorSpaceTraits.h>

#include <QTest>

const int TILE_WIDTH = 64;
const int TILE_HEIGHT = 64;

const int IMG_WIDTH = 4096;
const int IMG_HEIGHT = 4096;

const quint8 OPACITY_HALF = 128;

const int TILES_IN_WIDTH = IMG_WIDTH / TILE_WIDTH;
const int TILES_IN_HEIGHT = IMG_HEIGHT / TILE_HEIGHT;

#define COMPOSITE_BENCHMARK                                                                                                                                    \
    for (int y = 0; y < TILES_IN_HEIGHT; y++) {                                                                                                                \
        for (int x = 0; x < TILES_IN_WIDTH; x++) {                                                                                                             \
            compositeOp->composite(m_dstBuffer,                                                                                                                \
                                   TILE_WIDTH *KoBgrU16Traits::pixelSize,                                                                                      \
                                   m_srcBuffer,                                                                                                                \
                                   TILE_WIDTH *KoBgrU16Traits::pixelSize,                                                                                      \
                                   0,                                                                                                                          \
                                   0,                                                                                                                          \
                                   TILE_WIDTH,                                                                                                                 \
                                   TILE_HEIGHT,                                                                                                                \
                                   OPACITY_HALF);                                                                                                              \
        }                                                                                                                                                      \
    }

void KoCompositeOpsBenchmark::initTestCase()
{
    m_dstBuffer = new quint8[TILE_WIDTH * TILE_HEIGHT * KoBgrU16Traits::pixelSize];
    m_srcBuffer = new quint8[TILE_WIDTH * TILE_HEIGHT * KoBgrU16Traits::pixelSize];
}

// this is called before every benchmark
void KoCompositeOpsBenchmark::init()
{
    memset(m_dstBuffer, 42, TILE_WIDTH * TILE_HEIGHT * KoBgrU16Traits::pixelSize);
    memset(m_srcBuffer, 42, TILE_WIDTH * TILE_HEIGHT * KoBgrU16Traits::pixelSize);
}

void KoCompositeOpsBenchmark::cleanupTestCase()
{
    delete[] m_dstBuffer;
    delete[] m_srcBuffer;
}

void KoCompositeOpsBenchmark::benchmarkCompositeOver()
{
    KoCompositeOp *compositeOp = KoOptimizedCompositeOpFactory::createOverOp32(KoColorSpaceRegistry::instance()->rgb16());
    QBENCHMARK {
        COMPOSITE_BENCHMARK
    }
}

void KoCompositeOpsBenchmark::benchmarkCompositeAlphaDarken()
{
    // KoCompositeOpAlphaDarken<KoBgrU16Traits> compositeOp(0);
    KoCompositeOp *compositeOp = KoOptimizedCompositeOpFactory::createAlphaDarkenOp32(KoColorSpaceRegistry::instance()->rgb16());
    QBENCHMARK {
        COMPOSITE_BENCHMARK
    }
}

QTEST_GUILESS_MAIN(KoCompositeOpsBenchmark)
