// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2001 Toshitaka Fujioka <fujioka@kde.org>
   Copyright (C) 2005 Thorsten Zachmann <zachmann@kde.org>

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

#include "kppolylineobject.h"
#include "kpresenter_utils.h"
#include "KPPolyLineObjectIface.h"

#include <qpainter.h>
#include <qwmatrix.h>
#include <qdom.h>

#include <kdebug.h>
#include <kozoomhandler.h>
#include <kooasiscontext.h>
#include <koUnit.h>

#include <math.h>
using namespace std;

KPPolylineObject::KPPolylineObject()
    : KPPointObject()
{
}

DCOPObject* KPPolylineObject::dcopObject()
{
    if ( !dcop )
        dcop = new KPPolyLineObjectIface( this );
    return dcop;
}

KPPolylineObject::KPPolylineObject(  const KoPointArray &_points, const KoSize &_size,
                                     const QPen &_pen, LineEnd _lineBegin, LineEnd _lineEnd )
    : KPPointObject( _pen, _lineBegin, _lineEnd )
{
    points = KoPointArray( _points );
    ext = _size;
}

KPPolylineObject &KPPolylineObject::operator=( const KPPolylineObject & )
{
    return *this;
}

QDomDocumentFragment KPPolylineObject::save( QDomDocument& doc, double offset )
{
    return KPPointObject::save( doc, offset );
}

double KPPolylineObject::load(const QDomElement &element)
{
    return KPPointObject::load( element );
}

bool KPPolylineObject::saveOasisObjectAttributes( KPOasisSaveContext &sc ) const
{
    QString listOfPoint;
    int maxX=0;
    int maxY=0;
    KoPointArray::ConstIterator it;
    for ( it = points.begin(); it != points.end(); ++it ) {
        int tmpX = 0;
        int tmpY = 0;
        tmpX = ( int ) ( KoUnit::toMM( ( *it ).x() )*100 );
        tmpY = ( int ) ( KoUnit::toMM( ( *it ).y() )*100 );
        kdDebug(33001) << "poly (x,y) (sx,xy) (" << ( *it ).x() << "," << ( *it ).y() << ") (" << tmpX << "," << tmpY << ")" << endl;
        if ( !listOfPoint.isEmpty() )
            listOfPoint += QString( " %1,%2" ).arg( tmpX ).arg( tmpY );
        else
            listOfPoint = QString( "%1,%2" ).arg( tmpX ).arg( tmpY );
        maxX = QMAX( maxX, tmpX );
        maxY = QMAX( maxY, tmpY );
    }
    sc.xmlWriter.addAttribute("draw:points", listOfPoint );
    sc.xmlWriter.addAttribute("svg:viewBox", QString( "0 0 %1 %2" ).arg( maxX ).arg( maxY ) );
    return true;
}

const char * KPPolylineObject::getOasisElementName() const
{
    return "draw:polyline";
}


void KPPolylineObject::loadOasis(const QDomElement &element, KoOasisContext & context, KPRLoadingInfo *info)
{
    kdDebug()<<"void KPPolylineObject::loadOasis(const QDomElement &element)************\n";
    KPPointObject::loadOasis( element,context, info );
}
