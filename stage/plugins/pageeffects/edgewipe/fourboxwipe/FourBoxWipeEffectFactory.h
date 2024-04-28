/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef FOURBOXWIPEEFFECTFACTORY_H
#define FOURBOXWIPEEFFECTFACTORY_H

#include <KPrPageEffectFactory.h>

class FourBoxWipeEffectFactory : public KPrPageEffectFactory
{
public:
    FourBoxWipeEffectFactory();
    ~FourBoxWipeEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType { CornersIn, CornersInReverse, CornersOut, CornersOutReverse };
};

#endif // FOURBOXWIPEEFFECTFACTORY_H
