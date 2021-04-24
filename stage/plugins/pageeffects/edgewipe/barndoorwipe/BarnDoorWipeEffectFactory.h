/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef BARNDOORWIPEEFFECTFACTORY_H
#define BARNDOORWIPEEFFECTFACTORY_H

#include <KPrPageEffectFactory.h>

class BarnDoorWipeEffectFactory : public KPrPageEffectFactory
{
public:
    BarnDoorWipeEffectFactory();
    ~BarnDoorWipeEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType {
        Vertical,
        VerticalReverse,
        Horizontal,
        HorizontalReverse,
        DiagonalBottomLeft,
        DiagonalBottomLeftReverse,
        DiagonalTopLeft,
        DiagonalTopLeftReverse
    };
};

#endif // BARNDOORWIPEEFFECTFACTORY_H
