/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef COMPONENTTRANSFEREFFECTFACTORY_H
#define COMPONENTTRANSFEREFFECTFACTORY_H

#include "KoFilterEffectFactoryBase.h"

class KoFilterEffect;

class ComponentTransferEffectFactory : public KoFilterEffectFactoryBase
{
public:
    ComponentTransferEffectFactory();
    KoFilterEffect *createFilterEffect() const override;
    KoFilterEffectConfigWidgetBase *createConfigWidget() const override;
};

#endif // COMPONENTTRANSFEREFFECTFACTORY_H
