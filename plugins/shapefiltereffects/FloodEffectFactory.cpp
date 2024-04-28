/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "FloodEffectFactory.h"
#include "FloodEffect.h"
#include "FloodEffectConfigWidget.h"

#include <KLocalizedString>

FloodEffectFactory::FloodEffectFactory()
    : KoFilterEffectFactoryBase(FloodEffectId, i18n("Flood fill"))
{
}

KoFilterEffect *FloodEffectFactory::createFilterEffect() const
{
    return new FloodEffect();
}

KoFilterEffectConfigWidgetBase *FloodEffectFactory::createConfigWidget() const
{
    return new FloodEffectConfigWidget();
}
