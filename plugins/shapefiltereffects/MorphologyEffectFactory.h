/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef MORPHOLOGYEFFECTFACTORY_H
#define MORPHOLOGYEFFECTFACTORY_H

#include "KoFilterEffectFactoryBase.h"

class KoFilterEffect;

class MorphologyEffectFactory : public KoFilterEffectFactoryBase
{
public:
    MorphologyEffectFactory();
    KoFilterEffect *createFilterEffect() const override;
    KoFilterEffectConfigWidgetBase *createConfigWidget() const override;
};

#endif // MORPHOLOGYEFFECTFACTORY_H
