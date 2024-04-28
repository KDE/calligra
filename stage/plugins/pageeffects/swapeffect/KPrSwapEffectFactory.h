/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Benjamin Port <port.benjamin@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRSWAPEFFECTFACTORY_H
#define KPRSWAPEFFECTFACTORY_H

#include "pageeffects/KPrPageEffectFactory.h"

class KPrSwapEffectFactory : public KPrPageEffectFactory
{
public:
    KPrSwapEffectFactory();
    ~KPrSwapEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType { Horizontal };
};

#endif /* KPRSWAPEFFECTFACTORY_H */
