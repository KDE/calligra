/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRARROWHEADWIPEEFFECTFACTORY_H
#define KPRARROWHEADWIPEEFFECTFACTORY_H

#include <KPrPageEffectFactory.h>

class KPrArrowHeadWipeEffectFactory : public KPrPageEffectFactory
{
public:
    KPrArrowHeadWipeEffectFactory();
    ~KPrArrowHeadWipeEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType { Up, UpReverse, Right, RightReverse, Down, DownReverse, Left, LeftReverse };
};

#endif /* KPRARROWHEADWIPEEFFECTFACTORY_H */
