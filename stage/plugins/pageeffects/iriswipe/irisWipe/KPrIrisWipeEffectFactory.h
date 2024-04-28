/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRIRISWIPEEFFECTFACTORY_H
#define KPRIRISWIPEEFFECTFACTORY_H

#include <KPrPageEffectFactory.h>

class KPrIrisWipeEffectFactory : public KPrPageEffectFactory
{
public:
    KPrIrisWipeEffectFactory();
    ~KPrIrisWipeEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType { Rectangle, RectangleReverse, Diamond, DiamondReverse };
};

#endif /* KPRIRISWIPEEFFECTFACTORY_H */
