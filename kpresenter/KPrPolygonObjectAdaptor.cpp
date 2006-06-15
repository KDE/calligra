// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2002 Laurent MONTEL <lmontel@mandrakesoft.com>

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

#include "KPrPolygonObjectAdaptor.h"
#include "KPrPolygonObject.h"
#include "KPrObject.h"

#include <kapplication.h>

KPrPolygonObjectAdaptor::KPrPolygonObjectAdaptor( KPrPolygonObject *_obj )
    : KPrObject2DAdaptor(_obj)
{
    obj = _obj;
}

void KPrPolygonObjectAdaptor::setCheckConcavePolygon(bool _concavePolygon)
{
    obj->setCheckConcavePolygon(_concavePolygon);
}

void KPrPolygonObjectAdaptor::setCornersValue(int _cornersValue)
{
    obj->setCheckConcavePolygon(_cornersValue);
}

void KPrPolygonObjectAdaptor::setSharpnessValue(int _sharpnessValue)
{
    obj->setSharpnessValue(_sharpnessValue);
}

int KPrPolygonObjectAdaptor::cornersValue()const
{
    return obj->getCornersValue();
}

int KPrPolygonObjectAdaptor::sharpnessValue()const
{
    return obj->getSharpnessValue();
}

bool KPrPolygonObjectAdaptor::checkConcavePolygon()const
{
    return obj->getCheckConcavePolygon();
}
