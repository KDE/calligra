// This file is part of the KDE project
// SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.net>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef KPRROUNDRECTWIPEEFFECTFACTORY_H
#define KPRROUNDRECTWIPEEFFECTFACTORY_H

#include <KPrPageEffectFactory.h>

class KPrRoundRectWipeEffectFactory : public KPrPageEffectFactory
{
public:
    KPrRoundRectWipeEffectFactory();
    ~KPrRoundRectWipeEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType { Horizontal, HorizontalReverse, Vertical, VerticalReverse };
};

#endif /* KPRROUNDRECTWIPEEFFECTFACTORY_H */
