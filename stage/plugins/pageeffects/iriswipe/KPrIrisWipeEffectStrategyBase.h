/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRIRISWIPEEFFECTSTRATEGYBASE_H
#define KPRIRISWIPEEFFECTSTRATEGYBASE_H

// Qt includes
#include <QPainterPath>

// KPresenter includes
#include "pageeffects/KPrPageEffectStrategy.h"

class KPrIrisWipeEffectStrategyBase : public KPrPageEffectStrategy
{
public:
    KPrIrisWipeEffectStrategyBase(QPainterPath shape, int subType, const char *smilType, const char *smilSubType, bool reverse = false);
    ~KPrIrisWipeEffectStrategyBase() override;

    void setup(const KPrPageEffect::Data &data, QTimeLine &timeLine) override;

    void paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data) override;

    void next(const KPrPageEffect::Data &data) override;

    int findMaxScaling(const KPrPageEffect::Data &data);

protected:
    QPainterPath m_shape;
};

#endif // KPRIRISWIPEEFFECTSTRATEGYBASE_H
