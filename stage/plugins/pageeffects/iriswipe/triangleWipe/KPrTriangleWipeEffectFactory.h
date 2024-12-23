/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRTRIANGLEWIPEEFFECTFACTORY_H
#define KPRTRIANGLEWIPEEFFECTFACTORY_H

#include <KPrPageEffectFactory.h>

class KPrTriangleWipeEffectFactory : public KPrPageEffectFactory
{
public:
    KPrTriangleWipeEffectFactory();
    ~KPrTriangleWipeEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType {
        Up,
        UpReverse,
        Right,
        RightReverse,
        Down,
        DownReverse,
        Left,
        LeftReverse
    };
};

#endif /* KPRTRIANGLEWIPEEFFECTFACTORY_H */
