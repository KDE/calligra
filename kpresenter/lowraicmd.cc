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
#include <kdebug.h>

/******************************************************************/
/* Class: LowerRaiseCmd                                           */
/******************************************************************/

/*======================== constructor ===========================*/
LowerRaiseCmd::LowerRaiseCmd( QString _name, QList<KPObject> *_oldList, QList<KPObject> *_newList, KPresenterDoc *_doc )
    : Command( _name )
{
    oldList = _oldList;
    newList = _newList;
    m_executed = false;
    oldList->setAutoDelete( false );
    newList->setAutoDelete( false );
    doc = _doc;

    for ( unsigned int i = 0; i < oldList->count(); i++ )
        oldList->at( i )->incCmdRef();
}

/*======================== destructor ============================*/
LowerRaiseCmd::~LowerRaiseCmd()
{
    for ( unsigned int i = 0; i < oldList->count(); i++ )
        oldList->at( i )->decCmdRef();

    // I'm not sure how to handle this here correctly ( to avoid memory leaks and not to crash... )
    //delete oldList;
    //delete newList;

    // David: well, it's simple: if execute() was done last, only oldList should be deleted
    // and if unexecute() was done last, only newList should be deleted
    // (reason: the doc - or another command - holds the other list).
    if ( m_executed )
        delete oldList;
    else
        delete newList;
}

/*====================== execute =================================*/
void LowerRaiseCmd::execute()
{
    doc->setObjectList( newList );
    doc->repaint( false );
    m_executed = true;
}

/*====================== unexecute ===============================*/
void LowerRaiseCmd::unexecute()
{
    doc->setObjectList( oldList );
    doc->repaint( false );
    m_executed = false;
}
