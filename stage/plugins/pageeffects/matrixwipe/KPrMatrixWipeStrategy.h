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

#ifndef KPRMATRIXWIPESTRATEGY_H
#define KPRMATRIXWIPESTRATEGY_H

#include <KPrPageEffectStrategy.h>

class KPrMatrixWipeStrategy : public KPrPageEffectStrategy
{
public:
    KPrMatrixWipeStrategy(int subType, const char * smilType, const char *smilSubType, bool reverse, bool smooth = false);
    ~KPrMatrixWipeStrategy() override;

    void setup( const KPrPageEffect::Data &data, QTimeLine &timeLine ) override;

    void paintStep( QPainter &p, int currPos, const KPrPageEffect::Data &data ) override;

    void next( const KPrPageEffect::Data &data ) override;
    enum Direction {
        NotSmooth,
        TopToBottom,
        BottomToTop,
        LeftToRight,
        RightToLeft
    };
protected:
    virtual int squareIndex(int x, int y, int columns, int rows) = 0;
    virtual Direction squareDirection(int x, int y, int columns, int rows);
    virtual int maxIndex(int columns, int rows) = 0;
    void setNeedEvenSquares(bool hor = true, bool vert = true);
    bool m_smooth;
private:
    int m_squaresPerRow, m_squaresPerCol;
};

#endif // KPRMATRIXWIPESTRATEGY_H
