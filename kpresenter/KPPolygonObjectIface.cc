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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "KPPolygonObjectIface.h"
#include "kppolygonobject.h"
#include "kpobject.h"

#include <kapplication.h>
#include <dcopclient.h>

KPPolygonObjectIface::KPPolygonObjectIface( KPPolygonObject *_obj )
    : KPresenterObjectIface(_obj),KPresenterObject2DIface(_obj)

{
   obj = _obj;
}

void KPPolygonObjectIface::setCheckConcavePolygon(bool _concavePolygon)
{
    obj->setCheckConcavePolygon(_concavePolygon);
}

void KPPolygonObjectIface::setCornersValue(int _cornersValue)
{
    obj->setCheckConcavePolygon(_cornersValue);
}

void KPPolygonObjectIface::setSharpnessValue(int _sharpnessValue)
{
    obj->setSharpnessValue(_sharpnessValue);
}

int KPPolygonObjectIface::cornersValue()const
{
    return obj->getCornersValue();
}

int KPPolygonObjectIface::sharpnessValue()const
{
    return obj->getSharpnessValue();
}

bool KPPolygonObjectIface::checkConcavePolygon()const
{
    return obj->getCheckConcavePolygon();
}
