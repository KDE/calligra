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
#include <rotatecmd.h>

/******************************************************************/
/* Class: RotateCmd                                               */
/******************************************************************/

/*======================== constructor ===========================*/
RotateCmd::RotateCmd( QString _name, QList<RotateValues> &_oldRotate, float _newAngle,
                      QList<KPObject> &_objects, KPresenterDoc *_doc )
    : Command( _name ), oldRotate( _oldRotate ), objects( _objects )
{
    objects.setAutoDelete( false );
    oldRotate.setAutoDelete( false );
    doc = _doc;
    newAngle = _newAngle;

    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->incCmdRef();
}

/*======================== destructor ============================*/
RotateCmd::~RotateCmd()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->decCmdRef();
    oldRotate.setAutoDelete( true );
    oldRotate.clear();
}

/*====================== execute =================================*/
void RotateCmd::execute()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->rotate( newAngle );

    doc->repaint( false );
}

/*====================== unexecute ===============================*/
void RotateCmd::unexecute()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->rotate( oldRotate.at( i )->angle );

    doc->repaint( false );
}
