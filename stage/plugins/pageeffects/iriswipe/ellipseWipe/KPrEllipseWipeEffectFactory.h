/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRELLIPSEWIPEEFFECTFACTORY_H
#define KPRELLIPSEWIPEEFFECTFACTORY_H

#include <KPrPageEffectFactory.h>

class KPrEllipseWipeEffectFactory : public KPrPageEffectFactory
{
public:
    KPrEllipseWipeEffectFactory();
    ~KPrEllipseWipeEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType { Circle, CircleReverse, Horizontal, HorizontalReverse, Vertical, VerticalReverse };
};

#endif /* KPRELLIPSEWIPEEFFECTFACTORY_H */
