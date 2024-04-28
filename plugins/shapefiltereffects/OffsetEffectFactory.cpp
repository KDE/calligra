/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "OffsetEffectFactory.h"
#include "OffsetEffect.h"
#include "OffsetEffectConfigWidget.h"
#include <KLocalizedString>

OffsetEffectFactory::OffsetEffectFactory()
    : KoFilterEffectFactoryBase(OffsetEffectId, i18n("Offset"))
{
}

KoFilterEffect *OffsetEffectFactory::createFilterEffect() const
{
    return new OffsetEffect();
}

KoFilterEffectConfigWidgetBase *OffsetEffectFactory::createConfigWidget() const
{
    return new OffsetEffectConfigWidget();
}
