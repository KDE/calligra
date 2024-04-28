/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef ZIGZAGWIPEEFFECTFACTORY_H
#define ZIGZAGWIPEEFFECTFACTORY_H

#include <KPrPageEffectFactory.h>

class ZigZagWipeEffectFactory : public KPrPageEffectFactory
{
public:
    ZigZagWipeEffectFactory();
    ~ZigZagWipeEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType { FromLeft, FromTop, FromRight, FromBottom };
};

#endif // ZIGZAGWIPEEFFECTFACTORY_H
