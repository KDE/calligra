/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kpresenter_doc.h"
#include "resizecmd.h"
#include "kpobject.h"
#include "kptextobject.h"

#include <qrect.h>

/******************************************************************/
/* Class: ResizeCmd						  */
/******************************************************************/

/*======================== constructor ===========================*/
ResizeCmd::ResizeCmd( QString _name, QPoint _m_diff, QSize _r_diff, KPObject *_object, KPresenterDoc *_doc )
    : Command( _name ), m_diff( _m_diff ), r_diff( _r_diff )
{
    object = _object;
    doc = _doc;
    object->incCmdRef();
}

/*======================== destructor ============================*/
ResizeCmd::~ResizeCmd()
{
    object->decCmdRef();
}

/*====================== execute =================================*/
void ResizeCmd::execute()
{
    QRect oldRect;

    oldRect = object->getBoundingRect( 0, 0 );
    object->moveBy( m_diff );
    object->resizeBy( r_diff );
    if ( object->getType() == OT_TEXT )
	( (KPTextObject*)object )->recalcPageNum( doc );
    doc->repaint( oldRect );
    doc->repaint( object );
}

/*====================== unexecute ===============================*/
void ResizeCmd::unexecute()
{
    unexecute(true);
}

/*====================== unexecute ===============================*/
void ResizeCmd::unexecute( bool _repaint )
{
    QRect oldRect;

    oldRect = object->getBoundingRect( 0, 0 );
    object->moveBy( -m_diff.x(), -m_diff.y() );
    object->resizeBy( -r_diff.width(), -r_diff.height() );
    if ( object->getType() == OT_TEXT )
	( (KPTextObject*)object )->recalcPageNum( doc );

    if ( _repaint ) {
	doc->repaint( oldRect );
	doc->repaint( object );
    }
}

