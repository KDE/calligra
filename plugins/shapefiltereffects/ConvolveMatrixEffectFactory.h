/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef CONVOLVEMATRIXEFFECTFACTORY_H
#define CONVOLVEMATRIXEFFECTFACTORY_H

#include "KoFilterEffectFactoryBase.h"

class KoFilterEffect;

class ConvolveMatrixEffectFactory : public KoFilterEffectFactoryBase
{
public:
    ConvolveMatrixEffectFactory();
    KoFilterEffect *createFilterEffect() const override;
    KoFilterEffectConfigWidgetBase *createConfigWidget() const override;
};

#endif // CONVOLVEMATRIXEFFECTFACTORY_H
