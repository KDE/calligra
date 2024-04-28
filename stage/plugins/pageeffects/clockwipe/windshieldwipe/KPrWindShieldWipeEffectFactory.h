/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRWINDSHIELDWIPEEFFECTFACTORY_H
#define KPRWINDSHIELDWIPEEFFECTFACTORY_H

#include <KPrPageEffectFactory.h>

class KPrWindShieldWipeEffectFactory : public KPrPageEffectFactory
{
public:
    KPrWindShieldWipeEffectFactory();
    ~KPrWindShieldWipeEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType { Right, Up, Vertical, Horizontal, RightReverse, UpReverse, VerticalReverse, HorizontalReverse };
};

#endif /* KPRWINDSHIELDWIPEEFFECTFACTORY_H */
