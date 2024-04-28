/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "ComponentTransferEffectFactory.h"
#include "ComponentTransferEffect.h"
#include "ComponentTransferEffectConfigWidget.h"

#include <KLocalizedString>

ComponentTransferEffectFactory::ComponentTransferEffectFactory()
    : KoFilterEffectFactoryBase(ComponentTransferEffectId, i18n("Component transfer"))
{
}

KoFilterEffect *ComponentTransferEffectFactory::createFilterEffect() const
{
    return new ComponentTransferEffect();
}

KoFilterEffectConfigWidgetBase *ComponentTransferEffectFactory::createConfigWidget() const
{
    return new ComponentTransferEffectConfigWidget();
}
