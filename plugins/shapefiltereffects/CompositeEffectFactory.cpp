/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "CompositeEffectFactory.h"
#include "CompositeEffect.h"
#include "CompositeEffectConfigWidget.h"

#include <KLocalizedString>

CompositeEffectFactory::CompositeEffectFactory()
    : KoFilterEffectFactoryBase(CompositeEffectId, i18n("Composite"))
{
}

KoFilterEffect *CompositeEffectFactory::createFilterEffect() const
{
    return new CompositeEffect();
}

KoFilterEffectConfigWidgetBase *CompositeEffectFactory::createConfigWidget() const
{
    return new CompositeEffectConfigWidget();
}
