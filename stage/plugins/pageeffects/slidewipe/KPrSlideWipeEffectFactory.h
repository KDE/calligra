/* This file is part of the KDE project
   Copyright (C) 2007 Thorsten Zachmann <zachmann@kde.org>

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
        FromLeft,       // the new page is coming from the left
        FromRight,      // the new page is coming from the right
        FromTop,        // the new page is coming from the top
        FromBottom,     // the new page is coming from the bottom
        ToLeft,         // the old page is leaving from the left
        ToRight,        // the old page is leaving from the right
        ToTop,          // the old page is leaving from the top
        ToBottom        // the old page is leaving from the bottom
    };
};

#endif /* KPRSLIDEWIPEEFFECTFACTORY_H */
