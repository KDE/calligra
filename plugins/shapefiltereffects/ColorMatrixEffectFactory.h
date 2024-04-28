/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef COLORMATRIXEFFECTFACTORY_H
#define COLORMATRIXEFFECTFACTORY_H

#include "KoFilterEffectFactoryBase.h"

class KoFilterEffect;

class ColorMatrixEffectFactory : public KoFilterEffectFactoryBase
{
public:
    ColorMatrixEffectFactory();
    KoFilterEffect *createFilterEffect() const override;
    KoFilterEffectConfigWidgetBase *createConfigWidget() const override;
};

#endif // COLORMATRIXEFFECTFACTORY_H
