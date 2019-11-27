/* This file is part of the KDE project
   Copyright (C) 2008 Sven Langkamp <sven.langkamp@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KPRFANWIPEEFFECTFACTORY_H
#define KPRFANWIPEEFFECTFACTORY_H

#include <KPrPageEffectFactory.h>

class KPrFanWipeEffectFactory : public KPrPageEffectFactory
{
public:
    KPrFanWipeEffectFactory();
    ~KPrFanWipeEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType {
        CenterRight,
        CenterTop,
        CenterLeft,
        CenterBottom,
        FanOutTop,
        FanOutRight,
        FanOutBottom,
        FanOutLeft,
        FanInTop,
        FanInRight,
        FanInBottom,
        FanInLeft
    };
};

#endif /* KPRFANWIPEEFFECTFACTORY_H */
