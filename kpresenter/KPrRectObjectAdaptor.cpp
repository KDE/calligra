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

#include "KPrRectObjectAdaptor.h"
#include "KPrRectObject.h"
#include "KPrObject.h"

#include <kapplication.h>

KPrRectObjectAdaptor::KPrRectObjectAdaptor( KPrRectObject *_obj )
    : KPrObject2DAdaptor(_obj)

{
    obj = _obj;
}

void KPrRectObjectAdaptor::setRnds( int _xRnd, int _yRnd )
{
    obj->setRnds( _xRnd, _yRnd );
}

int KPrRectObjectAdaptor::xRnd() const
{
    int _xRnd,_yRnd;
    obj->getRnds( _xRnd, _yRnd );
    return _xRnd;
}

int KPrRectObjectAdaptor::yRnd() const
{
    int _xRnd,_yRnd;
    obj->getRnds( _xRnd, _yRnd );
    return _yRnd;
}

#include <KPrRectObjectAdaptor.moc>
