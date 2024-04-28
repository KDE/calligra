/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef FLOODEFFECTFACTORY_H
#define FLOODEFFECTFACTORY_H

#include "KoFilterEffectFactoryBase.h"

class KoFilterEffect;

class FloodEffectFactory : public KoFilterEffectFactoryBase
{
public:
    FloodEffectFactory();
    KoFilterEffect *createFilterEffect() const override;
    KoFilterEffectConfigWidgetBase *createConfigWidget() const override;
};

#endif // FLOODEFFECTFACTORY_H
