// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2001 Toshitaka Fujioka <fujioka@kde.org>

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

bool KPPolylineObject::saveOasis( KoXmlWriter &xmlWriter, KoSavingContext& context, int indexObj ) const
{
    xmlWriter.startElement( "draw:polyline" );
    saveOasisPosObject(xmlWriter, indexObj );
    xmlWriter.addAttribute( "draw:style-name", saveOasisStrokeElement( context.mainStyles() ) );

    KPPointObject::saveOasis( xmlWriter, context );
    if( !objectName.isEmpty())
        xmlWriter.addAttribute( "draw:name", objectName );
    xmlWriter.endElement();
    return true;
}


void KPPolylineObject::loadOasis(const QDomElement &element, KoOasisContext & context, KPRLoadingInfo *info)
{
    kdDebug()<<"void KPPolylineObject::loadOasis(const QDomElement &element)************\n";
    KPPointObject::loadOasis( element,context, info );
}
