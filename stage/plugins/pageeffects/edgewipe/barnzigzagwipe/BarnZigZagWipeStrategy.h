/* This file is part of the KDE project
 * Copyright (C) 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef BARNZIGZAGWIPESTRATEGY_H
#define BARNZIGZAGWIPESTRATEGY_H

#include <KPrPageEffectStrategy.h>
#include <KPrPageEffect.h>

class BarnZigZagWipeStrategy : public KPrPageEffectStrategy
{
public:
    BarnZigZagWipeStrategy( int subtype, const char *smilSubType, bool reverse );
    ~BarnZigZagWipeStrategy() override;

    /// reimplemented from KPrPageEffectStrategy
    void setup( const KPrPageEffect::Data &data, QTimeLine &timeLine ) override;
    /// reimplemented from KPrPageEffectStrategy
    void paintStep( QPainter &p, int currPos, const KPrPageEffect::Data &data ) override;
    /// reimplemented from KPrPageEffectStrategy
    void next( const KPrPageEffect::Data &data ) override;

private:
    /// calculates clip path for given step
    QPainterPath clipPath( int step, const QRect &area );
};

#endif // BARNZIGZAGWIPESTRATEGY_H
