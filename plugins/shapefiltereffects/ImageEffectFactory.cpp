/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "ImageEffectFactory.h"
#include "ImageEffect.h"
#include "ImageEffectConfigWidget.h"
#include <KLocalizedString>

ImageEffectFactory::ImageEffectFactory()
    : KoFilterEffectFactoryBase(ImageEffectId, i18n("Image"))
{
}

KoFilterEffect *ImageEffectFactory::createFilterEffect() const
{
    return new ImageEffect();
}

KoFilterEffectConfigWidgetBase *ImageEffectFactory::createConfigWidget() const
{
    return new ImageEffectConfigWidget();
}
