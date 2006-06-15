// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2001 Toshitaka Fujioka <fujioka@kde.org>
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
#include "KPrFreehandObjectAdaptor.h"
#include "KPrFreehandObject.h"
#include "KPrUtils.h"
#include <KoTextZoomHandler.h>
#include <qpainter.h>
#include <qmatrix.h>
#include <qdom.h>

#include <kdebug.h>
#include <math.h>
using namespace std;

KPrFreehandObject::KPrFreehandObject()
    : KPrPointObject()
{
}

KPrFreehandObject::KPrFreehandObject( const KoPointArray &_points, const KoSize &_size,
                                    const KoPen &_pen, LineEnd _lineBegin, LineEnd _lineEnd )
    : KPrPointObject( _pen, _lineBegin, _lineEnd )
{
    points = KoPointArray( _points );
    ext = _size;
}

KPrFreehandObject &KPrFreehandObject::operator=( const KPrFreehandObject & )
{
    return *this;
}

KPrObjectAdaptor* KPrFreehandObject::dbusObject()
{
    if ( !dbus )
        dbus = new KPrFreehandObjectAdaptor( this );
    return dbus;
}

bool KPrFreehandObject::saveOasisObjectAttributes( KPOasisSaveContext &sc ) const
{
    // the rect for the view box have to be the rect and not the real rect
    KoRect rect( getRect() );
    sc.xmlWriter.addAttribute("svg:viewBox", QString( "0 0 %1 %2" ).arg( int( rect.width() * 100 ) )
                                                                   .arg( int( rect.height() * 100 ) ) );
    unsigned int pointCount = points.count();
    unsigned int pos = 0;

    QString d;
    d += QString( "M%1 %2" ).arg( int( points.at(pos).x() * 100 ) )
                            .arg( int( points.at(pos).y() * 100 ) );
    ++pos;

    while ( pos < pointCount )
    {
        d += QString( "L%1 %2" ).arg( int( points.at( pos ).x() * 100 ) )
                                .arg( int( points.at( pos ).y() * 100 ) );
        ++pos;
    }

    sc.xmlWriter.addAttribute( "svg:d", d );

    return true;
}

const char * KPrFreehandObject::getOasisElementName() const
{
    return "draw:path";
}

void KPrFreehandObject::loadOasis( const QDomElement &element, KoOasisContext & context, KPrLoadingInfo* info )
{
    kDebug(33001) << "KPrFreehandObject::loadOasis" << endl;
    KPrPointObject::loadOasis( element, context, info );

    //load marker
    loadOasisMarker( context );
}

QDomDocumentFragment KPrFreehandObject::save( QDomDocument& doc,double offset )
{
    return KPrPointObject::save( doc, offset );
}

double KPrFreehandObject::load( const QDomElement &element )
{
    return KPrPointObject::load( element );
}
