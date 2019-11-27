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

#ifndef KPRBOXSNAKESWIPESTRATEGY_H
#define KPRBOXSNAKESWIPESTRATEGY_H

#include "../KPrMatrixWipeStrategy.h"

class KPrBoxSnakesWipeStrategy : public KPrMatrixWipeStrategy
{
public:
    KPrBoxSnakesWipeStrategy(int horRepeat, int verRepeat, bool clockwise, bool reverse);
    ~KPrBoxSnakesWipeStrategy() override;
protected:
    int maxIndex(int columns, int rows) override;
    int squareIndex(int x, int y, int columns, int rows) override;
private:
    int m_horRepeat;
    int m_verRepeat;
    bool m_clockwise;
};

#endif // KPRBOXSNAKESWIPESTRATEGY_H
