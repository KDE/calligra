/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Timothee Lacroix <dakeyras.khan@gmail.com>
   SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRBARWIPEEFFECTFACTORY_H
#define KPRBARWIPEEFFECTFACTORY_H

#include "pageeffects/KPrPageEffectFactory.h"

class KPrBarWipeEffectFactory : public KPrPageEffectFactory
{
public:
    KPrBarWipeEffectFactory();
    ~KPrBarWipeEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType { FromLeft, FromTop, FromRight, FromBottom };
};

#endif /* KPRBARWIPEEFFECTFACTORY_H */
