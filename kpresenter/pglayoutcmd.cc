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

#include <kpresenter_view.h>
#include <pglayoutcmd.h>
#include <kpbackground.h>

#include <koRuler.h>

/******************************************************************/
/* Class: PgLayoutCmd                                             */
/******************************************************************/

/*======================== constructor ===========================*/
PgLayoutCmd::PgLayoutCmd( QString _name, KoPageLayout _layout, KoPageLayout _oldLayout,
                          KPresenterView *_view )
    : Command( _name )
{
    layout = _layout;
    oldLayout = _oldLayout;
    view = _view;
}

/*====================== execute =================================*/
void PgLayoutCmd::execute()
{
    view->kPresenterDoc()->setPageLayout( layout, view->getDiffX(), view->getDiffY() );
    view->getHRuler()->setPageLayout( layout );
    view->getVRuler()->setPageLayout( layout );
    view->setRanges();
}

/*====================== unexecute ===============================*/
void PgLayoutCmd::unexecute()
{
    view->kPresenterDoc()->setPageLayout( oldLayout, view->getDiffX(), view->getDiffY() );
    view->getHRuler()->setPageLayout( oldLayout );
    view->getVRuler()->setPageLayout( oldLayout );
    view->setRanges();
}
