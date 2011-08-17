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

#include "KPrBoxSnakesWipeStrategy.h"
#include "KPrBoxSnakesWipeEffectFactory.h"

static int getSubType(int horRepeat, int verRepeat, bool clockwise, bool reverse)
{
    if (!reverse) {
        if (horRepeat == 2 && verRepeat == 1) return clockwise ? KPrBoxSnakesWipeEffectFactory::TwoBoxBottomIn : KPrBoxSnakesWipeEffectFactory::TwoBoxTopIn;
        if (horRepeat == 1 && verRepeat == 2) return clockwise ? KPrBoxSnakesWipeEffectFactory::TwoBoxLeftIn : KPrBoxSnakesWipeEffectFactory::TwoBoxRightIn;
        return clockwise ? KPrBoxSnakesWipeEffectFactory::FourBoxHorizontalIn : KPrBoxSnakesWipeEffectFactory::FourBoxVerticalIn;
    } else {
        if (horRepeat == 2 && verRepeat == 1) return clockwise ? KPrBoxSnakesWipeEffectFactory::TwoBoxBottomOut : KPrBoxSnakesWipeEffectFactory::TwoBoxTopOut;
        if (horRepeat == 1 && verRepeat == 2) return clockwise ? KPrBoxSnakesWipeEffectFactory::TwoBoxLeftOut : KPrBoxSnakesWipeEffectFactory::TwoBoxRightOut;
        return clockwise ? KPrBoxSnakesWipeEffectFactory::FourBoxHorizontalOut : KPrBoxSnakesWipeEffectFactory::FourBoxVerticalOut;
    }
}

static const char * getSmilSubType(int horRepeat, int verRepeat, bool clockwise)
{
    if (horRepeat == 2 && verRepeat == 1) return clockwise ? "twoBoxBottom" : "twoBoxTop";
    if (horRepeat == 1 && verRepeat == 2) return clockwise ? "twoBoxLeft" : "twoBoxRight";
    return clockwise ? "fourBoxHorizontal" : "fourBoxVertical";
}

KPrBoxSnakesWipeStrategy::KPrBoxSnakesWipeStrategy(int horRepeat, int verRepeat, bool clockwise, bool reverse)
    : KPrMatrixWipeStrategy( getSubType(horRepeat, verRepeat, clockwise, reverse), "spiralWipe", getSmilSubType(horRepeat, verRepeat, clockwise), reverse ),
    m_horRepeat(horRepeat), m_verRepeat(verRepeat), m_clockwise(clockwise)
{
    setNeedEvenSquares();
}

KPrBoxSnakesWipeStrategy::~KPrBoxSnakesWipeStrategy()
{
}

static int getFirstLeg(bool clockwise, int verrepeat, int horrepeat) {
    if (verrepeat == 1 && horrepeat == 2) return clockwise ? 3 : 0;
    if (verrepeat == 2 && horrepeat == 1) return clockwise ? 0 : 3;
    if (verrepeat == 2 && horrepeat == 2) return clockwise ? 0 : 0;
    return 0;
}

int KPrBoxSnakesWipeStrategy::squareIndex(int x, int y, int columns, int rows)
{
    if (m_horRepeat == 2) {
        if (x >= columns / 2) {
            x = columns - x - 1;
        }
        columns /= 2;
    }
    if (m_verRepeat == 2) {
        if (y >= rows / 2) {
            y = rows - y - 1;
        }
        rows /= 2;
    }
    int m_firstLeg = getFirstLeg(m_clockwise, m_verRepeat, m_horRepeat);

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

int KPrBoxSnakesWipeStrategy::maxIndex(int columns, int rows)
{
    return columns * rows / m_horRepeat / m_verRepeat;
}

