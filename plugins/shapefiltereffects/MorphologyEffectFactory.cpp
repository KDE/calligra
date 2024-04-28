/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "MorphologyEffectFactory.h"
#include "MorphologyEffect.h"
#include "MorphologyEffectConfigWidget.h"

#include <KLocalizedString>

MorphologyEffectFactory::MorphologyEffectFactory()
    : KoFilterEffectFactoryBase(MorphologyEffectId, i18n("Morphology"))
{
}

KoFilterEffect *MorphologyEffectFactory::createFilterEffect() const
{
    return new MorphologyEffect();
}

KoFilterEffectConfigWidgetBase *MorphologyEffectFactory::createConfigWidget() const
{
    return new MorphologyEffectConfigWidget();
}
