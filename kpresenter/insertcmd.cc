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

#include <kpresenter_doc.h>
#include <insertcmd.h>
#include <kptextobject.h>

/******************************************************************/
/* Class: InsertCmd						  */
/******************************************************************/

/*======================== constructor ===========================*/
InsertCmd::InsertCmd( QString _name, KPObject *_object, KPresenterDoc *_doc )
    : Command( _name )
{
    object = _object;
    doc = _doc;
    object->incCmdRef();
}

/*======================== destructor ============================*/
InsertCmd::~InsertCmd()
{
    object->decCmdRef();
}

/*====================== execute =================================*/
void InsertCmd::execute()
{
    doc->objectList()->append( object );
    object->addToObjList();
    if ( object->getType() == OT_TEXT )
	( (KPTextObject*)object )->recalcPageNum( doc );
    doc->repaint( object );
}

/*====================== unexecute ===============================*/
void InsertCmd::unexecute()
{
    QRect oldRect = object->getBoundingRect( 0, 0 );
    if ( doc->objectList()->findRef( object ) != -1 ) {
	doc->objectList()->take( doc->objectList()->findRef( object ) );
	object->removeFromObjList();
    }
    doc->repaint( oldRect );
}
