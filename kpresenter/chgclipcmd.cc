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
#include <chgclipcmd.h>
#include <kpclipartobject.h>

/******************************************************************/
/* Class: ChgClipCmd                                              */
/******************************************************************/

/*======================== constructor ===========================*/
ChgClipCmd::ChgClipCmd( QString _name, KPClipartObject *_object, KPClipartCollection::Key _oldKey,
                        KPClipartCollection::Key _newKey, KPresenterDoc *_doc )
    : Command( _name ), oldKey( _oldKey ), newKey( _newKey )
{
    object = _object;
    doc = _doc;
    object->incCmdRef();
}

/*======================== destructor ============================*/
ChgClipCmd::~ChgClipCmd()
{
    object->decCmdRef();
}

/*======================== execute ===============================*/
void ChgClipCmd::execute()
{
    object->setFileName( newKey.filename, newKey.lastModified );
    doc->repaint( object );
}

/*====================== unexecute ===============================*/
void ChgClipCmd::unexecute()
{
    object->setFileName( oldKey.filename, oldKey.lastModified );
    doc->repaint( object );
}
