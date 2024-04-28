/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "BlurEffectFactory.h"
#include "BlurEffect.h"
#include "BlurEffectConfigWidget.h"

#include <KLocalizedString>

BlurEffectFactory::BlurEffectFactory()
    : KoFilterEffectFactoryBase(BlurEffectId, i18n("Gaussian blur"))
{
}

KoFilterEffect *BlurEffectFactory::createFilterEffect() const
{
    return new BlurEffect();
}

KoFilterEffectConfigWidgetBase *BlurEffectFactory::createConfigWidget() const
{
    return new BlurEffectConfigWidget();
}
