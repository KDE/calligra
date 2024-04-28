/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRSALOONDOORWIPEEFFECTFACTORY_H
#define KPRSALOONDOORWIPEEFFECTFACTORY_H

#include <KPrPageEffectFactory.h>

class KPrSaloonDoorWipeEffectFactory : public KPrPageEffectFactory
{
public:
    KPrSaloonDoorWipeEffectFactory();
    ~KPrSaloonDoorWipeEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType { FromTop, FromLeft, FromBottom, FromRight, ToTop, ToLeft, ToBottom, ToRight };
};

#endif /* KPRSALOONDOORWIPEEFFECTFACTORY_H */
