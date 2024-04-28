/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRSTARWIPEEFFECTFACTORY_H
#define KPRSTARWIPEEFFECTFACTORY_H

#include <KPrPageEffectFactory.h>

class KPrStarWipeEffectFactory : public KPrPageEffectFactory
{
public:
    KPrStarWipeEffectFactory();
    ~KPrStarWipeEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType { FourPoint, FourPointReverse, FivePoint, FivePointReverse, SixPoint, SixPointReverse };
};

#endif /* KPRSTARWIPEEFFECTFACTORY_H */
