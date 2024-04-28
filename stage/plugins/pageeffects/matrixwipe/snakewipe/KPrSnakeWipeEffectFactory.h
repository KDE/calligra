/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRSNAKEWIPEEFFECTFACTORY_H
#define KPRSNAKEWIPEEFFECTFACTORY_H

#include <KPrPageEffectFactory.h>

class KPrSnakeWipeEffectFactory : public KPrPageEffectFactory
{
public:
    KPrSnakeWipeEffectFactory();
    ~KPrSnakeWipeEffectFactory() override;
    QString subTypeName(int subTypeId) const override;

    enum SubType {
        FromLeft, // the new page is coming from the left
        FromRight, // the new page is coming from the right
        FromTop, // the new page is coming from the top
        FromBottom, // the new page is coming from the bottom
        FromTopLeft, // the new page is coming from the top-left
        FromTopRight, // the new page is coming from the top-right
        FromBottomLeft, // the new page is coming from the bottom-left
        FromBottomRight // the new page is coming from the bottom-right
    };
};

#endif /* KPRSNAKEWIPEEFFECTFACTORY_H */
