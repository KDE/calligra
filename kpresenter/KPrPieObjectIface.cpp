// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include "KPrPieObjectIface.h"
#include "KPrPieObject.h"
#include "KPrObject.h"
#include "KPrUtils.h"
#include <kapplication.h>
#include <dcopclient.h>
#include <kdebug.h>

KPrPieObjectIface::KPrPieObjectIface( KPrPieObject *_obj )
    : KPrObject2DIface(_obj)
{
    obj = _obj;
}

void KPrPieObjectIface::setPieAngle( int _p_angle )
{
    obj->setPieAngle(_p_angle);
}

void KPrPieObjectIface::setPieLength( int _p_len )
{
    obj->setPieLength(_p_len);
}

int KPrPieObjectIface::pieAngle() const
{
    return obj->getPieAngle();
}

int KPrPieObjectIface::pieLength() const
{
    return obj->getPieLength();
}

void KPrPieObjectIface::setPieType( const QString & type )
{
    if( type =="Pie")
        obj->setPieType(PT_PIE);
    else if(type=="Arc")
        obj->setPieType(PT_ARC);
    else if(type=="Chord")
        obj->setPieType(PT_CHORD);
}

void KPrPieObjectIface::setLineBegin( const QString & type)
{
    obj->setLineBegin(lineEndBeginFromString( type ));
}

void KPrPieObjectIface::setLineEnd( const QString & type)
{
    obj->setLineEnd(lineEndBeginFromString( type ));
}

void KPrPieObjectIface::horizontalFlip()
{
    obj->flip( true);
}

void KPrPieObjectIface::verticalFlip()
{
    obj->flip( false );
}
