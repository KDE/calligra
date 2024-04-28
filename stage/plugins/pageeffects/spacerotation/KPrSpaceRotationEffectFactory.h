/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Benjamin Port <port.benjamin@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRSPACEROTATIONEFFECTFACTORY_H
#define KPRSPACEROTATIONEFFECTFACTORY_H

#include "pageeffects/KPrPageEffectFactory.h"

class KPrSpaceRotationEffectFactory : public KPrPageEffectFactory
{
public:
    KPrSpaceRotationEffectFactory();
    ~KPrSpaceRotationEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType { FromBottom, FromTop, FromLeft, FromRight };
};

#endif /* KPRSPACEROTATIONEFFECTFACTORY_H */
