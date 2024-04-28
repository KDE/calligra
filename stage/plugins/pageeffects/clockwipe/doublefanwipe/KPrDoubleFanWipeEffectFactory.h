/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRDOUBLEFANWIPEEFFECTFACTORY_H
#define KPRDOUBLEFANWIPEEFFECTFACTORY_H

#include <KPrPageEffectFactory.h>

class KPrDoubleFanWipeEffectFactory : public KPrPageEffectFactory
{
public:
    KPrDoubleFanWipeEffectFactory();
    ~KPrDoubleFanWipeEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType { FanOutVertical, FanOutHorizontal, FanInVertical, FanInHorizontal, FanInVerticalReverse, FanInHorizontalReverse };
};

#endif /* KPRDOUBLEFANWIPEEFFECTFACTORY_H */
