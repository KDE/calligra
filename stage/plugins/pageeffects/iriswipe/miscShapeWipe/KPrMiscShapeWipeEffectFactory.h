/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRMISCSHAPEWIPEEFFECTFACTORY_H
#define KPRMISCSHAPEWIPEEFFECTFACTORY_H

#include <KPrPageEffectFactory.h>

class KPrMiscShapeWipeEffectFactory : public KPrPageEffectFactory
{
public:
    KPrMiscShapeWipeEffectFactory();
    ~KPrMiscShapeWipeEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType { Heart, HeartReverse, Keyhole, KeyholeReverse };
};

#endif /* KPRMISCSHAPEWIPEEFFECTFACTORY_H */
