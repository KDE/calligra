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

#include <kpresenter_doc.h>
#include <rectvaluecmd.h>
#include <kprectobject.h>

/******************************************************************/
/* Class: RectValueCmd                                            */
/******************************************************************/

/*======================== constructor ===========================*/
RectValueCmd::RectValueCmd( QString _name, QList<RectValues> &_oldValues, RectValues _newValues,
                            QList<KPObject> &_objects, KPresenterDoc *_doc )
    : Command( _name ), oldValues( _oldValues ), objects( _objects )
{
    objects.setAutoDelete( false );
    oldValues.setAutoDelete( false );
    doc = _doc;
    newValues = _newValues;

    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->incCmdRef();
}

/*======================== destructor ============================*/
RectValueCmd::~RectValueCmd()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->decCmdRef();
    oldValues.setAutoDelete( true );
    oldValues.clear();
}

/*====================== execute =================================*/
void RectValueCmd::execute()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
        dynamic_cast<KPRectObject*>( objects.at( i ) )->setRnds( newValues.xRnd, newValues.yRnd );

    doc->repaint( false );
}

/*====================== unexecute ===============================*/
void RectValueCmd::unexecute()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
        dynamic_cast<KPRectObject*>( objects.at( i ) )->setRnds( oldValues.at( i )->xRnd, oldValues.at( i )->yRnd );

    doc->repaint( false );
}
