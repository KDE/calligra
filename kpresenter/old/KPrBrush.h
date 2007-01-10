// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2005-2006 Thorsten Zachmann <zachmann@kde.org>

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

#ifndef KPRBRUSH_H
#define KPRBRUSH_H

#include <qbrush.h>
#include <KoBrush.h>

#include "global.h"

class KoGenStyle;
class KoGenStyles;
class KoOasisContext;
class QDomElement;

class KPrBrush : public KoBrush
{
public:
    KPrBrush();
    KPrBrush(  const QBrush &brush, const QColor &gColor1, const QColor &gColor2,
              BCType gType, FillType fillType, bool unbalanced,
              int xfactor, int yfactor );

    void saveOasisFillStyle( KoGenStyle &styleObjectAuto, KoGenStyles& mainStyles ) const;
    void loadOasisFillStyle( KoOasisContext &context, const char * propertyType );

private:
    QString saveOasisGradientStyle( KoGenStyles& mainStyles ) const;
};

#endif /* KPRBRUSH_H */

