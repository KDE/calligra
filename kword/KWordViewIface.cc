/* This file is part of the KDE project
   Copyright (C) 2001 Laurent Montel <lmontel@mandrakesoft.com>

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

#include "KWordViewIface.h"

#include "kwview.h"
#include "kwdoc.h"

#include <dcopclient.h>

KWordViewIface::KWordViewIface( KWView *view_ )
    : KoViewIface( view_ )
{
    view = view_;
}

void KWordViewIface::fileStatistics()
{
    view->fileStatistics();
}

void KWordViewIface::editFind()
{
    view->editFind();
}

void KWordViewIface::editReplace()
{
    view->editReplace();
}

void KWordViewIface::editCustomVars()
{
    view->editCustomVars();
}

void KWordViewIface::editMailMergeDataBase()
{
    view->editMailMergeDataBase();
}

void KWordViewIface::viewPageMode()
{
    view->viewPageMode();
}

void KWordViewIface::viewPreviewMode()
{
    view->viewPreviewMode();
}

void KWordViewIface::configure()
{
    view->configure();
}

void KWordViewIface::extraSpelling()
{
    view->extraSpelling();
}

void KWordViewIface::extraAutoFormat()
{
    view->extraAutoFormat();
}

void KWordViewIface::extraStylist()
{
    view->extraStylist();
}

void KWordViewIface::extraCreateTemplate()
{
    view->extraCreateTemplate();
}

void KWordViewIface::insertTable()
{
    view->insertTable();
}

void KWordViewIface::insertPicture()
{
    view->insertPicture();
}

void KWordViewIface::formatPage()
{
    view->formatPage();
}

void KWordViewIface::toolsPart()
{
    view->toolsPart();
}

double KWordViewIface::zoom()
{
    return view->kWordDocument()->zoom();
}


void KWordViewIface::setZoom( int zoom)
{
    view->showZoom( zoom ); // share the same zoom
    view->setZoom( zoom,true);
    view->kWordDocument()->updateZoomRuler();
}

void KWordViewIface::editPersonalExpression()
{
    view->editPersonalExpr();
}

void KWordViewIface::insertLink()
{
    view->insertLink();
}

void KWordViewIface::insertFormula()
{
    view->insertFormula();
}

void KWordViewIface::formatFont()
{
    view->formatFont();
}

void KWordViewIface::formatParagraph()
{
    view->formatParagraph();
}

void KWordViewIface::raiseFrame()
{
  view->raiseFrame();
}

void KWordViewIface::lowerFrame()
{
  view->lowerFrame();
}

void KWordViewIface::bringToFront()
{
    view->bringToFront();
}

void KWordViewIface::sendToBack()
{
    view->sendToBack();
}


void KWordViewIface::applyAutoFormat()
{
    view->applyAutoFormat();
}


void KWordViewIface::createStyleFromSelection()
{
    view->createStyleFromSelection();
}
