/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "BlendEffectFactory.h"
#include "BlendEffect.h"
#include "BlendEffectConfigWidget.h"

#include <KLocalizedString>

BlendEffectFactory::BlendEffectFactory()
    : KoFilterEffectFactoryBase(BlendEffectId, i18n("Blend"))
{
}

KoFilterEffect *BlendEffectFactory::createFilterEffect() const
{
    return new BlendEffect();
}

KoFilterEffectConfigWidgetBase *BlendEffectFactory::createConfigWidget() const
{
    return new BlendEffectConfigWidget();
}
