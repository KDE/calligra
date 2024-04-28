/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRSLIDEWIPEEFFECTFACTORY_H
#define KPRSLIDEWIPEEFFECTFACTORY_H

#include "pageeffects/KPrPageEffectFactory.h"

class KPrSlideWipeEffectFactory : public KPrPageEffectFactory
{
public:
    KPrSlideWipeEffectFactory();
    ~KPrSlideWipeEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType {
        FromLeft, // the new page is coming from the left
        FromRight, // the new page is coming from the right
        FromTop, // the new page is coming from the top
        FromBottom, // the new page is coming from the bottom
        ToLeft, // the old page is leaving from the left
        ToRight, // the old page is leaving from the right
        ToTop, // the old page is leaving from the top
        ToBottom // the old page is leaving from the bottom
    };
};

#endif /* KPRSLIDEWIPEEFFECTFACTORY_H */
