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
#include <setoptionscmd.h>

/******************************************************************/
/* Class: SetOptionsCmd                                           */
/******************************************************************/

/*======================== constructor ===========================*/
SetOptionsCmd::SetOptionsCmd( QString _name, QList<QPoint> &_diffs, QList<KPObject> &_objects,
                              int _rastX, int _rastY, int _orastX, int _orastY,
                              QColor _txtBackCol, QColor _otxtBackCol, KPresenterDoc *_doc )
    : Command( _name ), diffs( _diffs ), objects( _objects ), txtBackCol( _txtBackCol ), otxtBackCol( _otxtBackCol )
{
    rastX = _rastX;
    rastY = _rastY;
    orastX = _orastX;
    orastY = _orastY;
    doc = _doc;
    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->incCmdRef();
}

/*======================== destructor ============================*/
SetOptionsCmd::~SetOptionsCmd()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->decCmdRef();
}

/*====================== execute =================================*/
void SetOptionsCmd::execute()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->moveBy( *diffs.at( i ) );

    doc->setRasters( rastX, rastY, false );
    doc->setTxtBackCol( txtBackCol );
    doc->repaint( false );
}

/*====================== unexecute ===============================*/
void SetOptionsCmd::unexecute()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->moveBy( -diffs.at( i )->x(), -diffs.at( i )->y() );

    doc->setRasters( orastX, orastY, false );
    doc->setTxtBackCol( otxtBackCol );
    doc->repaint( false );
}
