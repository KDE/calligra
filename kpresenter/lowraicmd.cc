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
#include <lowraicmd.h>

/******************************************************************/
/* Class: LowerRaiseCmd                                           */
/******************************************************************/

/*======================== constructor ===========================*/
LowerRaiseCmd::LowerRaiseCmd( QString _name, QList<KPObject> *_old, QList<KPObject> *__new, KPresenterDoc *_doc )
    : Command( _name )
{
    old = _old;
    _new = __new;
    old->setAutoDelete( false );
    _new->setAutoDelete( false );
    doc = _doc;

    for ( unsigned int i = 0; i < old->count(); i++ )
        old->at( i )->incCmdRef();
}

/*======================== destructor ============================*/
LowerRaiseCmd::~LowerRaiseCmd()
{
    for ( unsigned int i = 0; i < old->count(); i++ )
        old->at( i )->decCmdRef();

    // I'm not sure how to handle this here correctly ( to avoid memory leaks and not to crash... )
    //delete old;
    //delete _new;
}

/*====================== execute =================================*/
void LowerRaiseCmd::execute()
{
    doc->setObjectList( _new );
    doc->repaint( false );
}

/*====================== unexecute ===============================*/
void LowerRaiseCmd::unexecute()
{
    doc->setObjectList( old );
    doc->repaint( false );
}
