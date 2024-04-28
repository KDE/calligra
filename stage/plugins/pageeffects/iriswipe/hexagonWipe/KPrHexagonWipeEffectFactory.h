// This file is part of the KDE project
// SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.net>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef KPRHEXAGONWIPEEFFECTFACTORY_H
#define KPRHEXAGONWIPEEFFECTFACTORY_H

#include <KPrPageEffectFactory.h>

class KPrHexagonWipeEffectFactory : public KPrPageEffectFactory
{
public:
    KPrHexagonWipeEffectFactory();
    ~KPrHexagonWipeEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType { Horizontal, HorizontalReverse, Vertical, VerticalReverse };
};

#endif /* KPRHEXAGONWIPEEFFECTFACTORY_H */
