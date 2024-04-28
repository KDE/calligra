/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef VEEWIPEEFFECTFACTORY_H
#define VEEWIPEEFFECTFACTORY_H

#include <KPrPageEffectFactory.h>

class VeeWipeEffectFactory : public KPrPageEffectFactory
{
public:
    VeeWipeEffectFactory();
    ~VeeWipeEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType { FromTop, FromRight, FromBottom, FromLeft };
};

#endif // VEEWIPEEFFECTFACTORY_H
