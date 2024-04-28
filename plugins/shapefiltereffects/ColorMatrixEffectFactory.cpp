/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "ColorMatrixEffectFactory.h"
#include "ColorMatrixEffect.h"
#include "ColorMatrixEffectConfigWidget.h"

#include <KLocalizedString>

ColorMatrixEffectFactory::ColorMatrixEffectFactory()
    : KoFilterEffectFactoryBase(ColorMatrixEffectId, i18n("Color matrix"))
{
}

KoFilterEffect *ColorMatrixEffectFactory::createFilterEffect() const
{
    return new ColorMatrixEffect();
}

KoFilterEffectConfigWidgetBase *ColorMatrixEffectFactory::createConfigWidget() const
{
    return new ColorMatrixEffectConfigWidget();
}
