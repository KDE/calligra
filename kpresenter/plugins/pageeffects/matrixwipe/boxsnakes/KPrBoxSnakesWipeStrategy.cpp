/* This file is part of the KDE project
   Copyright (C) 2008 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>

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
#include <kdebug.h>

static KPrPageEffect::SubType getSubType(int horRepeat, int verRepeat, bool clockwise, bool reverse)
{
    if (!reverse) {
        if (horRepeat == 2 && verRepeat == 1) return clockwise ? KPrPageEffect::TwoBoxBottomIn : KPrPageEffect::TwoBoxTopIn;
        if (horRepeat == 1 && verRepeat == 2) return clockwise ? KPrPageEffect::TwoBoxLeftIn : KPrPageEffect::TwoBoxRightIn;
        return clockwise ? KPrPageEffect::FourBoxHorizontalIn : KPrPageEffect::FourBoxVerticalIn;
    } else {
        if (horRepeat == 2 && verRepeat == 1) return clockwise ? KPrPageEffect::TwoBoxBottomOut : KPrPageEffect::TwoBoxTopOut;
        if (horRepeat == 1 && verRepeat == 2) return clockwise ? KPrPageEffect::TwoBoxLeftOut : KPrPageEffect::TwoBoxRightOut;
        return clockwise ? KPrPageEffect::FourBoxHorizontalOut : KPrPageEffect::FourBoxVerticalOut;
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

int KPrBoxSnakesWipeStrategy::squareIndex(int x, int y, int collumns, int rows)
{
    if (m_horRepeat == 2) {
        if (x >= collumns / 2) {
            x = collumns - x - 1;
        }
        collumns /= 2;
    }
    if (m_verRepeat == 2) {
        if (y >= rows / 2) {
            y = rows - y - 1;
        }
        rows /= 2;
    }
    int m_firstLeg = getFirstLeg(m_clockwise, m_verRepeat, m_horRepeat);

    int curRing = qMin(qMin(x, y), qMin(collumns - x - 1, rows - y - 1));
    int maxRingSize = (collumns + rows - 2) * 2;
    int passed = 0;
    if (curRing > 0) passed = curRing * (maxRingSize + maxRingSize - (curRing-1) * 8) / 2;
    int leg = 0;
    if (m_clockwise) {
        if (y == curRing) {
            leg = 0;
        }
        if (x == collumns - curRing - 1) {
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
        if (leg > m_firstLeg && leg < m_firstLeg+4) passed += (m_firstLeg&1 ? rows : collumns) - 2*curRing - 1;
        if (leg > m_firstLeg+1 && leg < m_firstLeg+4) passed += (m_firstLeg&1 ? collumns : rows) - 2*curRing - 1;
        if (leg > m_firstLeg+2 && leg < m_firstLeg+4) passed += (m_firstLeg&1 ? rows : collumns) - 2*curRing - 1;
        if (leg > 3) leg -= 4;

        if (leg == 0) {
            passed += x - curRing;
        } else if (leg == 1) {
            passed += y - curRing;
        } else if (leg == 2) {
            passed += collumns - x - curRing - 1;
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
        if (x == collumns - curRing - 1) {
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
        if (leg > m_firstLeg && leg < m_firstLeg+4) passed += (m_firstLeg&1 ? collumns : rows) - 2*curRing - 1;
        if (leg > m_firstLeg+1 && leg < m_firstLeg+4) passed += (m_firstLeg&1 ? rows : collumns) - 2*curRing - 1;
        if (leg > m_firstLeg+2 && leg < m_firstLeg+4) passed += (m_firstLeg&1 ? collumns : rows) - 2*curRing - 1;
        if (leg > 3) leg -= 4;

        if (leg == 0) {
            passed += y - curRing;
        } else if (leg == 1) {
            passed += x - curRing;
        } else if (leg == 2) {
            passed += rows - y - curRing - 1;
        } else if (leg == 3) {
            passed += collumns - x - curRing - 1;
        }
    }
    return reverse() ? collumns * rows - passed - 1 : passed;
}

int KPrBoxSnakesWipeStrategy::maxIndex(int collumns, int rows)
{
    return collumns * rows / m_horRepeat / m_verRepeat;
}

