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
#include <shadowcmd.h>

/******************************************************************/
/* Class: ShadowCmd                                               */
/******************************************************************/

/*======================== constructor ===========================*/
ShadowCmd::ShadowCmd( QString _name, QList<ShadowValues> &_oldShadow, ShadowValues _newShadow,
                      QList<KPObject> &_objects, KPresenterDoc *_doc )
    : Command( _name ), oldShadow( _oldShadow ), objects( _objects )
{
    objects.setAutoDelete( false );
    oldShadow.setAutoDelete( false );
    doc = _doc;
    newShadow = _newShadow;

    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->incCmdRef();
}

/*======================== destructor ============================*/
ShadowCmd::~ShadowCmd()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->decCmdRef();
    oldShadow.setAutoDelete( true );
    oldShadow.clear();
}

/*====================== execute =================================*/
void ShadowCmd::execute()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
    {
        objects.at( i )->setShadowDistance( newShadow.shadowDistance );
        objects.at( i )->setShadowDirection( newShadow.shadowDirection );
        objects.at( i )->setShadowColor( newShadow.shadowColor );
    }
    doc->repaint( false );
}

/*====================== unexecute ===============================*/
void ShadowCmd::unexecute()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
    {
        objects.at( i )->setShadowDistance( oldShadow.at( i )->shadowDistance );
        objects.at( i )->setShadowDirection( oldShadow.at( i )->shadowDirection );
        objects.at( i )->setShadowColor( oldShadow.at( i )->shadowColor );
    }
    doc->repaint( false );
}
