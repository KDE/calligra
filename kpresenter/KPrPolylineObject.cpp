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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KPrPolylineObject.h"
#include "KPrUtils.h"
#include "KPrPolyLineObjectAdaptor.h"

#include <qpainter.h>
#include <qmatrix.h>
#include <qdom.h>

#include <kdebug.h>
#include <KoTextZoomHandler.h>
#include <KoOasisContext.h>
#include <KoUnit.h>

#include <math.h>
using namespace std;

KPrPolylineObject::KPrPolylineObject()
    : KPrPointObject()
{
    dbus = new KPrPolyLineObjectAdaptor( this );
}

KPrObjectAdaptor* KPrPolylineObject::dbusObject()
{
    return dbus;
}

KPrPolylineObject::KPrPolylineObject(  const KoPointArray &_points, const KoSize &_size,
                                     const KoPen &_pen, LineEnd _lineBegin, LineEnd _lineEnd )
    : KPrPointObject( _pen, _lineBegin, _lineEnd )
{
    points = KoPointArray( _points );
    ext = _size;
}

KPrPolylineObject &KPrPolylineObject::operator=( const KPrPolylineObject & )
{
    return *this;
}

QDomDocumentFragment KPrPolylineObject::save( QDomDocument& doc, double offset )
{
    return KPrPointObject::save( doc, offset );
}

double KPrPolylineObject::load(const QDomElement &element)
{
    return KPrPointObject::load( element );
}

bool KPrPolylineObject::saveOasisObjectAttributes( KPOasisSaveContext &sc ) const
{
    KPrShadowObject::saveOasisDrawPoints( points, sc );
    return true;
}

const char * KPrPolylineObject::getOasisElementName() const
{
    return "draw:polyline";
}


void KPrPolylineObject::loadOasis(const QDomElement &element, KoOasisContext & context, KPrLoadingInfo *info)
{
    kDebug()<<"void KPrPolylineObject::loadOasis(const QDomElement &element)************\n";
    KPrShadowObject::loadOasis( element, context, info );
    KPrShadowObject::loadOasisDrawPoints( points, element, context, info );
    loadOasisMarker( context );
}
