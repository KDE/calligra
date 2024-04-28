/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef DIAGONALWIPEEFFECTFACTORY_H
#define DIAGONALWIPEEFFECTFACTORY_H

#include <KPrPageEffectFactory.h>

class DiagonalWipeEffectFactory : public KPrPageEffectFactory
{
public:
    DiagonalWipeEffectFactory();
    ~DiagonalWipeEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType { FromTopLeft, FromBottomRight, FromTopRight, FromBottomLeft };
};

#endif // DIAGONALWIPEEFFECTFACTORY_H
