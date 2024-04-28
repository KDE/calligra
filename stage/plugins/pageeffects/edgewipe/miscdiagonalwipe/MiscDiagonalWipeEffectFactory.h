/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef MISCDIAGONALWIPEEFFECTFACTORY_H
#define MISCDIAGONALWIPEEFFECTFACTORY_H

#include <KPrPageEffectFactory.h>

class MiscDiagonalWipeEffectFactory : public KPrPageEffectFactory
{
public:
    MiscDiagonalWipeEffectFactory();
    ~MiscDiagonalWipeEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType { DoubleBarnDoor, DoubleDiamond };
};

#endif // MISCDIAGONALWIPEEFFECTFACTORY_H
