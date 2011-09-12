/* This file is part of the KDE project
   Copyright (C) 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#include "KPrSpiralWipeStrategy.h"
#include "KPrSpiralWipeEffectFactory.h"

static int getSubType(int firstLeg, bool clockwise, bool reverse)
{
    if (!reverse) {
        switch (firstLeg) {
            case 0: return clockwise ? KPrSpiralWipeEffectFactory::ClockwiseTopLeftIn : KPrSpiralWipeEffectFactory::CounterClockwiseTopLeftIn;
            case 1: return clockwise ? KPrSpiralWipeEffectFactory::ClockwiseTopRightIn : KPrSpiralWipeEffectFactory::CounterClockwiseBottomLeftIn;
            case 2: return clockwise ? KPrSpiralWipeEffectFactory::ClockwiseBottomRightIn : KPrSpiralWipeEffectFactory::CounterClockwiseBottomRightIn;
            case 3: return clockwise ? KPrSpiralWipeEffectFactory::ClockwiseBottomLeftIn : KPrSpiralWipeEffectFactory::CounterClockwiseTopRightIn;
        }
    } else {
        switch (firstLeg) {
            case 0: return !clockwise ? KPrSpiralWipeEffectFactory::ClockwiseTopLeftOut : KPrSpiralWipeEffectFactory::CounterClockwiseTopLeftOut;
            case 1: return !clockwise ? KPrSpiralWipeEffectFactory::ClockwiseTopRightOut : KPrSpiralWipeEffectFactory::CounterClockwiseBottomLeftOut;
            case 2: return !clockwise ? KPrSpiralWipeEffectFactory::ClockwiseBottomRightOut : KPrSpiralWipeEffectFactory::CounterClockwiseBottomRightOut;
            case 3: return !clockwise ? KPrSpiralWipeEffectFactory::ClockwiseBottomLeftOut : KPrSpiralWipeEffectFactory::CounterClockwiseTopRightOut;
        }
    }
    // not reached
    return KPrSpiralWipeEffectFactory::ClockwiseTopLeftIn;
}

static const char * getSmilSubType(int firstLeg, bool clockwise)
{
    switch (firstLeg) {
        case 0: return clockwise ? "topLeftClockwise" : "topLeftCounterClockwise";
        case 1: return clockwise ? "topRightClockwise" : "bottomLeftCounterClockwise";
        case 2: return clockwise ? "bottomRightClockwise" : "bottomRightCounterClockwise";
        default:
        case 3: return clockwise ? "bottomLeftClockwise" : "topRightCounterClockwise";
    }
}

KPrSpiralWipeStrategy::KPrSpiralWipeStrategy(int firstLeg, bool clockwise, bool reverse)
    : KPrMatrixWipeStrategy( getSubType(firstLeg, clockwise, reverse), "spiralWipe", getSmilSubType(firstLeg, clockwise), reverse ),
    m_firstLeg(firstLeg), m_clockwise(clockwise)
{
}

KPrSpiralWipeStrategy::~KPrSpiralWipeStrategy()
{
}

int KPrSpiralWipeStrategy::squareIndex(int x, int y, int columns, int rows)
{
    int curRing = qMin(qMin(x, y), qMin(columns - x - 1, rows - y - 1));
    int maxRingSize = (columns + rows - 2) * 2;
    int passed = 0;
    if (curRing > 0) passed = curRing * (maxRingSize + maxRingSize - (curRing-1) * 8) / 2;
    int leg = 0;
    if (m_clockwise) {
        if (y == curRing) {
            leg = 0;
        }
        if (x == columns - curRing - 1) {
            leg = 1;
        }
        if (y == rows - curRing - 1) {
            leg = 2;
        }
        if (x == curRing && y != curRing) {
            leg = 3;
        }
        if (curRing * 2 + 1 == rows) {
            if (m_firstLeg == 0 || m_firstLeg == 3) {
                leg = 0;
            }
        }
        if (leg < m_firstLeg) leg += 4;
        if (leg > m_firstLeg && leg < m_firstLeg+4) passed += (m_firstLeg&1 ? rows : columns) - 2*curRing - 1;
        if (leg > m_firstLeg+1 && leg < m_firstLeg+4) passed += (m_firstLeg&1 ? columns : rows) - 2*curRing - 1;
        if (leg > m_firstLeg+2 && leg < m_firstLeg+4) passed += (m_firstLeg&1 ? rows : columns) - 2*curRing - 1;
        if (leg > 3) leg -= 4;

        if (leg == 0) {
            passed += x - curRing;
        } else if (leg == 1) {
            passed += y - curRing;
        } else if (leg == 2) {
            passed += columns - x - curRing - 1;
        } else if (leg == 3) {
            passed += rows - y - curRing - 1;
        }
    } else {
        if (x == curRing) {
            leg = 0;
        }
        if (y == rows - curRing - 1) {
            leg = 1;
        }
        if (x == columns - curRing - 1) {
            leg = 2;
        }
        if (y == curRing && x != curRing) {
            leg = 3;
        }
        if (curRing * 2 + 1 == rows) {
            if (m_firstLeg == 0 || m_firstLeg == 1) {
                leg = 1;
            }
        }
        if (leg < m_firstLeg) leg += 4;
        if (leg > m_firstLeg && leg < m_firstLeg+4) passed += (m_firstLeg&1 ? columns : rows) - 2*curRing - 1;
        if (leg > m_firstLeg+1 && leg < m_firstLeg+4) passed += (m_firstLeg&1 ? rows : columns) - 2*curRing - 1;
        if (leg > m_firstLeg+2 && leg < m_firstLeg+4) passed += (m_firstLeg&1 ? columns : rows) - 2*curRing - 1;
        if (leg > 3) leg -= 4;

        if (leg == 0) {
            passed += y - curRing;
        } else if (leg == 1) {
            passed += x - curRing;
        } else if (leg == 2) {
            passed += rows - y - curRing - 1;
        } else if (leg == 3) {
            passed += columns - x - curRing - 1;
        }
    }
    return reverse() ? columns * rows - passed - 1 : passed;
}

int KPrSpiralWipeStrategy::maxIndex(int columns, int rows)
{
    return columns * rows;
}

