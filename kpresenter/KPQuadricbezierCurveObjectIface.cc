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

#include "KPQuadricbezierCurveObjectIface.h"
#include "kpquadricbeziercurveobject.h"
#include "kpresenter_utils.h"
#include "kpobject.h"
#include <kdebug.h>

#include <kapplication.h>
#include <dcopclient.h>

KPQuadricBezierCurveObjectIface::KPQuadricBezierCurveObjectIface( KPQuadricBezierCurveObject *_obj )
    : KPresenterObjectIface(_obj)
{
   obj = _obj;
}


void KPQuadricBezierCurveObjectIface::setLineBegin( const QString & type)
{

    obj->setLineBegin(lineEndBeginFromString( type ));
}

void KPQuadricBezierCurveObjectIface::setLineEnd( const QString & type)
{
    obj->setLineEnd(lineEndBeginFromString( type ));
}

QString KPQuadricBezierCurveObjectIface::lineBegin()const
{
    LineEnd type=obj->getLineBegin();
    return lineEndBeginName( type );
}

QString KPQuadricBezierCurveObjectIface::lineEnd() const
{
    LineEnd type=obj->getLineEnd();
    return lineEndBeginName( type );
}

void KPQuadricBezierCurveObjectIface::horizontalFlips()
{
    obj->flip( true );
}

void KPQuadricBezierCurveObjectIface::verticalFlips()
{
    obj->flip( false );
}

void KPQuadricBezierCurveObjectIface::closeObject(bool close)
{
    obj->closeObject( close );
}

bool KPQuadricBezierCurveObjectIface::isClosed()const
{
    return obj->isClosed();
}
