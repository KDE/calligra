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
#include <movecmd.h>
#include <kptextobject.h>

/******************************************************************/
/* Class: MoveByCmd						  */
/******************************************************************/

/*======================== constructor ===========================*/
MoveByCmd::MoveByCmd( QString _name, QPoint _diff, QList<KPObject> &_objects, KPresenterDoc *_doc )
    : Command( _name ), diff( _diff ), objects( _objects )
{
    objects.setAutoDelete( false );
    doc = _doc;
    for ( unsigned int i = 0; i < objects.count(); i++ ) {
	if ( objects.at( i )->getType() == OT_TEXT ) {
	    ( (KPTextObject*)objects.at( i ) )->recalcPageNum( doc );
	    doc->repaint( objects.at( i ) );
	}
	objects.at( i )->incCmdRef();
    }
}

/*======================== destructor ============================*/
MoveByCmd::~MoveByCmd()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
	objects.at( i )->decCmdRef();
}

/*====================== execute =================================*/
void MoveByCmd::execute()
{
    QRect oldRect;

    for ( unsigned int i = 0; i < objects.count(); i++ ) {
	oldRect = objects.at( i )->getBoundingRect( 0, 0 );
	objects.at( i )->moveBy( diff );
	if ( objects.at( i )->getType() == OT_TEXT )
	    ( (KPTextObject*)objects.at( i ) )->recalcPageNum( doc );
	doc->repaint( oldRect );
	doc->repaint( objects.at( i ) );
    }
}

/*====================== unexecute ===============================*/
void MoveByCmd::unexecute()
{
    QRect oldRect;

    for ( unsigned int i = 0; i < objects.count(); i++ ) {
	oldRect = objects.at( i )->getBoundingRect( 0, 0 );
	objects.at( i )->moveBy( -diff.x(), -diff.y() );
	if ( objects.at( i )->getType() == OT_TEXT )
	    ( (KPTextObject*)objects.at( i ) )->recalcPageNum( doc );
	doc->repaint( oldRect );
	doc->repaint( objects.at( i ) );
    }
}

/******************************************************************/
/* Class: MoveByCmd2						  */
/******************************************************************/

/*======================== constructor ===========================*/
MoveByCmd2::MoveByCmd2( QString _name, QList<QPoint> &_diffs,
			QList<KPObject> &_objects, KPresenterDoc *_doc )
    : Command( _name ), diffs( _diffs ), objects( _objects )
{
    objects.setAutoDelete( false );
    diffs.setAutoDelete( true );
    doc = _doc;
    for ( unsigned int i = 0; i < objects.count(); i++ ) {
	if ( objects.at( i )->getType() == OT_TEXT ) {
	    ( (KPTextObject*)objects.at( i ) )->recalcPageNum( doc );
	    doc->repaint( objects.at( i ) );
	}
	objects.at( i )->incCmdRef();
    }
}

/*======================== destructor ============================*/
MoveByCmd2::~MoveByCmd2()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
	objects.at( i )->decCmdRef();

    diffs.clear();
}

/*====================== execute =================================*/
void MoveByCmd2::execute()
{
    QRect oldRect;

    for ( unsigned int i = 0; i < objects.count(); i++ ) {
	oldRect = objects.at( i )->getBoundingRect( 0, 0 );
	objects.at( i )->moveBy( *diffs.at( i ) );
	if ( objects.at( i )->getType() == OT_TEXT )
	    ( (KPTextObject*)objects.at( i ) )->recalcPageNum( doc );
	doc->repaint( oldRect );
	doc->repaint( objects.at( i ) );
    }
}

/*====================== unexecute ===============================*/
void MoveByCmd2::unexecute()
{
    QRect oldRect;

    for ( unsigned int i = 0; i < objects.count(); i++ ) {
	oldRect = objects.at( i )->getBoundingRect( 0, 0 );
	objects.at( i )->moveBy( -diffs.at( i )->x(), -diffs.at( i )->y() );
	if ( objects.at( i )->getType() == OT_TEXT )
	    ( (KPTextObject*)objects.at( i ) )->recalcPageNum( doc );
	doc->repaint( oldRect );
	doc->repaint( objects.at( i ) );
    }
}
