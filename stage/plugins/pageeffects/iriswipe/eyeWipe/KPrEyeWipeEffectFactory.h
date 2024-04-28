/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPREYEWIPEEFFECTFACTORY_H
#define KPREYEWIPEEFFECTFACTORY_H

#include <KPrPageEffectFactory.h>

class KPrEyeWipeEffectFactory : public KPrPageEffectFactory
{
public:
    KPrEyeWipeEffectFactory();
    ~KPrEyeWipeEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType { Vertical, VerticalReverse, Horizontal, HorizontalReverse };
};

#endif /* KPREYEWIPEEFFECTFACTORY_H */
