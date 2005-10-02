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
#include "KPFreehandObjectIface.h"
#include "kpfreehandobject.h"
#include "kpresenter_utils.h"
#include <kozoomhandler.h>
#include <qpainter.h>
#include <qwmatrix.h>
#include <qdom.h>

#include <kdebug.h>
#include <math.h>
using namespace std;

KPFreehandObject::KPFreehandObject()
    : KPPointObject()
{
}

KPFreehandObject::KPFreehandObject( const KoPointArray &_points, const KoSize &_size,
                                    const KPPen &_pen, LineEnd _lineBegin, LineEnd _lineEnd )
    : KPPointObject( _pen, _lineBegin, _lineEnd )
{
    points = KoPointArray( _points );
    ext = _size;
}

KPFreehandObject &KPFreehandObject::operator=( const KPFreehandObject & )
{
    return *this;
}

DCOPObject* KPFreehandObject::dcopObject()
{
    if ( !dcop )
        dcop = new KPFreehandObjectIface( this );
    return dcop;
}

bool KPFreehandObject::saveOasisObjectAttributes( KPOasisSaveContext &sc ) const
{
    kdDebug()<<"bool KPFreehandObject::saveOasisObjectAttributes( KPOasisSaveContext &sc ) not implemented\n";
    return true;
}

const char * KPFreehandObject::getOasisElementName() const
{
    return "draw:path";
}

void KPFreehandObject::loadOasis( const QDomElement &element, KoOasisContext & context, KPRLoadingInfo* info )
{
    //todo
    //we use draw:path

    //load marker
    loadOasisMarker( context );
}

QDomDocumentFragment KPFreehandObject::save( QDomDocument& doc,double offset )
{
    return KPPointObject::save( doc, offset );
}

double KPFreehandObject::load( const QDomElement &element )
{
    return KPPointObject::load( element );
}
