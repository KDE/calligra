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

#include "kpresenter_doc.h"
#include "grpobjcmd.h"
#include "kpgroupobject.h"

/******************************************************************/
/* Class: GroupObjCmd						  */
/******************************************************************/

/*==============================================================*/
GroupObjCmd::GroupObjCmd( const QString &_name, 
			  const QList<KPObject> &_objects, 
			  KPresenterDoc *_doc )
    : Command( _name ), objects( _objects )
{
    objects.setAutoDelete( FALSE );
    doc = _doc;
    for ( unsigned int i = 0; i < objects.count(); i++ )
	objects.at( i )->incCmdRef();
}

/*==============================================================*/
GroupObjCmd::~GroupObjCmd()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
	objects.at( i )->decCmdRef();
}

/*==============================================================*/
void GroupObjCmd::execute()
{
    qDebug( "GroupObjCmd::execute()" );
}

/*==============================================================*/
void GroupObjCmd::unexecute()
{
    qDebug( "GroupObjCmd::unexecute()" );
}
