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
#include <grpobjcmd.h>
#include <kpgroupobject.h>

/******************************************************************/
/* Class: GroupObjCmd						  */
/******************************************************************/

/*==============================================================*/
GroupObjCmd::GroupObjCmd( const QString &_name,
			  const QList<KPObject> &_objects,
			  KPresenterDoc *_doc )
    : Command( _name ), objects( _objects )
{
    objects.setAutoDelete( false );
    doc = _doc;
    for ( unsigned int i = 0; i < objects.count(); i++ )
	objects.at( i )->incCmdRef();
    grpObj = new KPGroupObject( objects );
    grpObj->incCmdRef();
}

/*==============================================================*/
GroupObjCmd::~GroupObjCmd()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
	objects.at( i )->decCmdRef();
    grpObj->decCmdRef();
}

/*==============================================================*/
void GroupObjCmd::execute()
{
    QRect r = objects.first()->getBoundingRect( 0, 0 );
    KPObject *obj = 0;

    for ( unsigned int i = 0; i < objects.count(); i++ ) {
	obj = objects.at( i );
	obj->setSelected( false );
	doc->objectList()->take( doc->objectList()->findRef( obj ) );
	obj->removeFromObjList();
	r = r.unite( obj->getBoundingRect( 0, 0 ) );
    }

    grpObj->setUpdateObjects( false );
    grpObj->setOrig( r.x(), r.y() );
    grpObj->setSize( r.width(), r.height() );
    doc->objectList()->append( grpObj );
    grpObj->addToObjList();
    grpObj->setUpdateObjects( true );
    grpObj->setSelected( true );

    doc->repaint( false );
}

/*==============================================================*/
void GroupObjCmd::unexecute()
{
    grpObj->setUpdateObjects( false );
    KPObject *obj = 0;

    for ( unsigned int i = 0; i < objects.count(); i++ ) {
	obj = objects.at( i );
	doc->objectList()->append( obj );
	obj->addToObjList();
	obj->setSelected( true );
    }

    doc->objectList()->take( doc->objectList()->findRef( grpObj ) );
    grpObj->removeFromObjList();

    doc->repaint( false );
}

/******************************************************************/
/* Class: UnGroupObjCmd						  */
/******************************************************************/

/*==============================================================*/
UnGroupObjCmd::UnGroupObjCmd( const QString &_name,
			  KPGroupObject *grpObj_,
			  KPresenterDoc *_doc )
    : Command( _name ), objects( grpObj_->getObjects() )
{
    objects.setAutoDelete( false );
    doc = _doc;
    for ( unsigned int i = 0; i < objects.count(); i++ )
	objects.at( i )->incCmdRef();
    grpObj = grpObj_;
    grpObj->incCmdRef();
}

/*==============================================================*/
UnGroupObjCmd::~UnGroupObjCmd()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
	objects.at( i )->decCmdRef();
    grpObj->decCmdRef();
}

/*==============================================================*/
void UnGroupObjCmd::execute()
{
    KPObject *obj = 0;
    grpObj->setUpdateObjects( false );

    for ( unsigned int i = 0; i < objects.count(); i++ ) {
	obj = objects.at( i );
	doc->objectList()->append( obj );
	obj->addToObjList();
	obj->setSelected( true );
    }

    doc->objectList()->take( doc->objectList()->findRef( grpObj ) );
    grpObj->removeFromObjList();

    doc->repaint( false );
}

/*==============================================================*/
void UnGroupObjCmd::unexecute()
{
    QRect r = objects.first()->getBoundingRect( 0, 0 );

    KPObject *obj = 0;

    for ( unsigned int i = 0; i < objects.count(); i++ ) {
	obj = objects.at( i );
	obj->setSelected( false );
	doc->objectList()->take( doc->objectList()->findRef( obj ) );
	obj->removeFromObjList();
	r = r.unite( obj->getBoundingRect( 0, 0 ) );
    }

    grpObj->setUpdateObjects( false );
    grpObj->setOrig( r.x(), r.y() );
    grpObj->setSize( r.width(), r.height() );
    doc->objectList()->append( grpObj );
    grpObj->addToObjList();
    grpObj->setUpdateObjects( true );
    grpObj->setSelected( true );

    doc->repaint( false );
}
