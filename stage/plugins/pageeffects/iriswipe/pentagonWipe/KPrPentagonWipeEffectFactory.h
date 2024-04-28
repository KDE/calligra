/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRPENTAGONWIPEEFFECTFACTORY_H
#define KPRPENTAGONWIPEEFFECTFACTORY_H

#include <KPrPageEffectFactory.h>

class KPrPentagonWipeEffectFactory : public KPrPageEffectFactory
{
public:
    KPrPentagonWipeEffectFactory();
    ~KPrPentagonWipeEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType { Up, UpReverse, Down, DownReverse };
};

#endif /* KPRPENTAGONWIPEEFFECTFACTORY_H */
