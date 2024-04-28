/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "ConvolveMatrixEffectFactory.h"
#include "ConvolveMatrixEffect.h"
#include "ConvolveMatrixEffectConfigWidget.h"

#include <KLocalizedString>

ConvolveMatrixEffectFactory::ConvolveMatrixEffectFactory()
    : KoFilterEffectFactoryBase(ConvolveMatrixEffectId, i18n("Convolve Matrix"))
{
}

KoFilterEffect *ConvolveMatrixEffectFactory::createFilterEffect() const
{
    return new ConvolveMatrixEffect();
}

KoFilterEffectConfigWidgetBase *ConvolveMatrixEffectFactory::createConfigWidget() const
{
    return new ConvolveMatrixEffectConfigWidget();
}
