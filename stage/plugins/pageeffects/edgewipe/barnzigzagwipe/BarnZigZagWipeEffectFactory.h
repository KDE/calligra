/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef BARNZIGZAGWIPEEFFECTFACTORY_H
#define BARNZIGZAGWIPEEFFECTFACTORY_H

#include <KPrPageEffectFactory.h>

class BarnZigZagWipeEffectFactory : public KPrPageEffectFactory
{
public:
    BarnZigZagWipeEffectFactory();
    ~BarnZigZagWipeEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType { Vertical, Horizontal, VerticalReversed, HorizontalReversed };
};

#endif // BARNZIGZAGWIPEEFFECTFACTORY_H
