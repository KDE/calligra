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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kpgroupobject.h"

#include <qpainter.h>

/******************************************************************/
/* Class: KPGroupObject						  */
/******************************************************************/

/*================================================================*/
KPGroupObject::KPGroupObject()
    : KPObject(), objects()
{
    objects.setAutoDelete( FALSE );
}

/*================================================================*/
KPGroupObject::KPGroupObject( const QList<KPObject> &objs )
    : KPObject(), objects( objs )
{
    objects.setAutoDelete( FALSE );
}

/*================================================================*/
KPGroupObject &KPGroupObject::operator=( const KPGroupObject & )
{
    return *this;
}

/*================================================================*/
void KPGroupObject::setSize( int _width, int _height )
{
    KPObject::setSize( _width, _height );
}

/*================================================================*/
void KPGroupObject::resizeBy( int _dx, int _dy )
{
    KPObject::resizeBy( _dx, _dy );
}

/*================================================================*/
void KPGroupObject::save( ostream& out )
{
}

/*================================================================*/
void KPGroupObject::load( KOMLParser& parser, vector<KOMLAttrib>& lst )
{
}

/*================================================================*/
void KPGroupObject::draw( QPainter *_painter, int _diffx, int _diffy )
{
    if ( move ) {
	KPObject::draw( _painter, _diffx, _diffy );
	return;
    }

    KPObject::draw( _painter, _diffx, _diffy );
}

/*================================================================*/
void KPGroupObject::paint( QPainter* _painter )
{
}




