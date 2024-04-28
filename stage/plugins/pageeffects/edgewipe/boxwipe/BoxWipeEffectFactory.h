/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef BOXWIPEEFFECTFACTORY_H
#define BOXWIPEEFFECTFACTORY_H

#include <KPrPageEffectFactory.h>

class BoxWipeEffectFactory : public KPrPageEffectFactory
{
public:
    BoxWipeEffectFactory();
    ~BoxWipeEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType { FromTopLeft, FromTopRight, FromBottomRight, FromBottomLeft, CenterTop, CenterRight, CenterBottom, CenterLeft };
};

#endif // BOXWIPEEFFECTFACTORY_H
