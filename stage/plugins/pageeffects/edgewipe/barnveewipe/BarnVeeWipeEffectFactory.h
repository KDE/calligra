/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef BARNVEEWIPEEFFECTFACTORY_H
#define BARNVEEWIPEEFFECTFACTORY_H

#include <KPrPageEffectFactory.h>

class BarnVeeWipeEffectFactory : public KPrPageEffectFactory
{
public:
    BarnVeeWipeEffectFactory();
    ~BarnVeeWipeEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType { FromTop, FromRight, FromBottom, FromLeft };
};

#endif // BARNVEEWIPEEFFECTFACTORY_H
