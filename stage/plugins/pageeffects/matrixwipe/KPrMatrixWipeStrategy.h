/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRMATRIXWIPESTRATEGY_H
#define KPRMATRIXWIPESTRATEGY_H

#include <KPrPageEffectStrategy.h>

class KPrMatrixWipeStrategy : public KPrPageEffectStrategy
{
public:
    KPrMatrixWipeStrategy(int subType, const char *smilType, const char *smilSubType, bool reverse, bool smooth = false);
    ~KPrMatrixWipeStrategy() override;

    void setup(const KPrPageEffect::Data &data, QTimeLine &timeLine) override;

    void paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data) override;

    void next(const KPrPageEffect::Data &data) override;
    enum Direction { NotSmooth, TopToBottom, BottomToTop, LeftToRight, RightToLeft };

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
