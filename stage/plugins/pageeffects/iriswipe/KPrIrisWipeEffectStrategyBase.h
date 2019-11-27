/* This file is part of the KDE project
   Copyright (C) 2008 Carlos Licea <carlos.licea@kdemail.net>

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

#ifndef KPRIRISWIPEEFFECTSTRATEGYBASE_H
#define KPRIRISWIPEEFFECTSTRATEGYBASE_H

//Qt includes
#include <QPainterPath>

//KPresenter includes
#include "pageeffects/KPrPageEffectStrategy.h"

class KPrIrisWipeEffectStrategyBase : public KPrPageEffectStrategy
{
public:
    KPrIrisWipeEffectStrategyBase(QPainterPath shape, int subType, const char * smilType, const char *smilSubType, bool reverse = false );
    ~KPrIrisWipeEffectStrategyBase() override;

    void setup( const KPrPageEffect::Data &data, QTimeLine &timeLine ) override;

    void paintStep( QPainter &p, int currPos, const KPrPageEffect::Data &data ) override;

    void next( const KPrPageEffect::Data &data ) override;

    int findMaxScaling( const KPrPageEffect::Data &data );

protected:
    QPainterPath m_shape;
};

#endif // KPRIRISWIPEEFFECTSTRATEGYBASE_H
