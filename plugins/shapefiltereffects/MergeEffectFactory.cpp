/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "MergeEffectFactory.h"
#include "MergeEffect.h"
#include "MergeEffectConfigWidget.h"

#include <KLocalizedString>

MergeEffectFactory::MergeEffectFactory()
    : KoFilterEffectFactoryBase(MergeEffectId, i18n("Merge"))
{
}

KoFilterEffect *MergeEffectFactory::createFilterEffect() const
{
    return new MergeEffect();
}

KoFilterEffectConfigWidgetBase *MergeEffectFactory::createConfigWidget() const
{
    return new MergeEffectConfigWidget();
}
