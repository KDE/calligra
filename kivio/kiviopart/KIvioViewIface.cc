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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "KIvioViewIface.h"

#include "kivio_view.h"
#include "kivio_doc.h"

#include <dcopclient.h>

KIvioViewIface::KIvioViewIface( KivioView *view_ )
    : KoViewIface( view_ )
{
    view = view_;
}

void KIvioViewIface::paperLayoutDlg()
{
    view->paperLayoutDlg();
}

void KIvioViewIface::insertPage()
{
    view->insertPage();
}

void KIvioViewIface::removePage()
{
    view->removePage();
}

void KIvioViewIface::renamePage()
{
    view->renamePage();
}

void KIvioViewIface::hidePage()
{
    view->hidePage();
}

void KIvioViewIface::showPage()
{
    view->showPage();
}

void KIvioViewIface::optionsDialog()
{
    view->optionsDialog();
}

bool KIvioViewIface::isSnapGuides()const
{
    return view->isSnapGuides();
}

bool KIvioViewIface::isShowGuides()const
{
    return view->isShowGuides();
}

bool KIvioViewIface::isShowRulers()const
{
    return view->isShowRulers();
}

bool KIvioViewIface::isShowPageMargins()const
{
    return view->isShowPageMargins();
}

int KIvioViewIface::leftBorder() const
{
    return view->leftBorder();
}

int KIvioViewIface::rightBorder() const
{
    return view->rightBorder();
}

int KIvioViewIface::topBorder() const
{
    return view->topBorder();
}

int KIvioViewIface::bottomBorder() const
{
    return view->bottomBorder();
}

void KIvioViewIface::togglePageMargins(bool b)
{
    view->togglePageMargins(b);
}

void KIvioViewIface::toggleShowRulers(bool b)
{
    view->toggleShowRulers(b);
}

void KIvioViewIface::toggleShowGrid(bool b)
{
    view->toggleShowGrid(b);
}

void KIvioViewIface::toggleSnapGrid(bool b)
{
    view->toggleSnapGrid(b);
}

void KIvioViewIface::toggleShowGuides(bool b)
{
    view->toggleShowGuides(b);
}

void KIvioViewIface::toggleViewManager(bool b)
{
    view->toggleViewManager(b);
}

void KIvioViewIface::groupStencils()
{
    view->groupStencils();
}

void KIvioViewIface::ungroupStencils()
{
    view->ungroupStencils();
}

void KIvioViewIface::selectAllStencils()
{
    view->selectAllStencils();
}

void KIvioViewIface::unselectAllStencils()
{
    view->unselectAllStencils();
}

double KIvioViewIface::lineWidth() const
{
    return view->lineWidth();
}

void KIvioViewIface::alignStencilsDlg()
{
    view->alignStencilsDlg();
}

QColor KIvioViewIface::foreGroundColor()const
{
    return view->fgColor();
}

QColor KIvioViewIface::backGroundColor()const
{
    return view->bgColor();
}
