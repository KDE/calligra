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
#include <pievaluecmd.h>
#include <kppieobject.h>

/******************************************************************/
/* Class: PieValueCmd                                             */
/******************************************************************/

/*======================== constructor ===========================*/
PieValueCmd::PieValueCmd( QString _name, QList<PieValues> &_oldValues, PieValues _newValues,
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
PieValueCmd::~PieValueCmd()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->decCmdRef();
    oldValues.setAutoDelete( true );
    oldValues.clear();
}

/*====================== execute =================================*/
void PieValueCmd::execute()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
    {
        dynamic_cast<KPPieObject*>( objects.at( i ) )->setPieType( newValues.pieType );
        dynamic_cast<KPPieObject*>( objects.at( i ) )->setPieAngle( newValues.pieAngle );
        dynamic_cast<KPPieObject*>( objects.at( i ) )->setPieLength( newValues.pieLength );
    }
    doc->repaint( false );
}

/*====================== unexecute ===============================*/
void PieValueCmd::unexecute()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
    {
        dynamic_cast<KPPieObject*>( objects.at( i ) )->setPieType( oldValues.at( i )->pieType );
        dynamic_cast<KPPieObject*>( objects.at( i ) )->setPieAngle( oldValues.at( i )->pieAngle );
        dynamic_cast<KPPieObject*>( objects.at( i ) )->setPieLength( oldValues.at( i )->pieLength );
    }
    doc->repaint( false );
}


