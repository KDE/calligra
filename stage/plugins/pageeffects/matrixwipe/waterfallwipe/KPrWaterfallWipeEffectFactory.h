/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRWATERFALLWIPEEFFECTFACTORY_H
#define KPRWATERFALLWIPEEFFECTFACTORY_H

#include <KPrPageEffectFactory.h>

class KPrWaterfallWipeEffectFactory : public KPrPageEffectFactory
{
public:
    KPrWaterfallWipeEffectFactory();
    ~KPrWaterfallWipeEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType {
        TopLeftVertical,
        TopLeftHorizontal,
        TopRightVertical,
        TopRightHorizontal,
        BottomLeftVertical,
        BottomLeftHorizontal,
        BottomRightVertical,
        BottomRightHorizontal
    };
};

#endif /* KPRWATERFALLWIPEEFFECTFACTORY_H */
