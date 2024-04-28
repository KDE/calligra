/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2010 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPRFADEEFFECTFACTORY_H
#define KPRFADEEFFECTFACTORY_H

#include "pageeffects/KPrPageEffectFactory.h"

class KPrFadeEffectFactory : public KPrPageEffectFactory
{
public:
    KPrFadeEffectFactory();
    ~KPrFadeEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType { CrossFade, FadeOverColor };
};

#endif /* KPRFADEEFFECTFACTORY_H */
