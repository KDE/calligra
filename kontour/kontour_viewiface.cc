/* This file is part of the KDE project
   Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>

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

#include "kontour_viewiface.h"

#include "kontour_view.h"

#include <dcopclient.h>

KOntourViewIface::KOntourViewIface( KontourView *view_ )
    : KoViewIface( view_ )
{
    m_view = view_;
}

void KOntourViewIface::slotCopy()
{
  m_view->slotCopy();
}

void KOntourViewIface::slotPaste()
{
  m_view->slotPaste();
}

void KOntourViewIface::slotCut()
{
  m_view->slotCut();
}

void KOntourViewIface::slotDuplicate()
{
  m_view->slotDuplicate();
}

void KOntourViewIface::slotDelete()
{
  m_view->slotDelete();
}

void KOntourViewIface::slotSelectAll()
{
  m_view->slotSelectAll();
}

void KOntourViewIface::slotZoomIn()
{
  m_view->slotZoomIn();
}

void KOntourViewIface::slotZoomOut()
{
  m_view->slotZoomOut();
}

void KOntourViewIface::slotOutline()
{
  m_view->slotOutline();
}

void KOntourViewIface::slotNormal()
{
  m_view->slotNormal();
}

void KOntourViewIface::slotShowRuler(bool b)
{
  m_view->slotCopy();
}

void KOntourViewIface::slotShowGrid(bool b)
{
  m_view->slotShowGrid(b);
}

void KOntourViewIface::slotShowHelplines(bool b)
{
  m_view->slotShowHelplines(b);
}

void KOntourViewIface::slotShowPaintPanel(bool b)
{
  m_view->slotShowPaintPanel(b);
}

void KOntourViewIface::slotShowOutlinePanel(bool b)
{
  m_view->slotShowOutlinePanel(b);
}

void KOntourViewIface::slotAlignToGrid(bool b)
{
  m_view->slotAlignToGrid(b);
}

void KOntourViewIface::slotAlignToHelplines(bool b)
{
  m_view->slotAlignToHelplines(b);
}

void KOntourViewIface::slotToFront()
{
  m_view->slotToFront();
}

void KOntourViewIface::slotToBack()
{
  m_view->slotToBack();
}

void KOntourViewIface::slotForwardOne()
{
  m_view->slotForwardOne();
}

void KOntourViewIface::slotBackOne()
{
  m_view->slotBackOne();
}

void KOntourViewIface::slotGroup()
{
  m_view->slotGroup();
}

void KOntourViewIface::slotUngroup()
{
  m_view->slotUngroup();
}

void KOntourViewIface::slotAddStyle()
{
  m_view->slotAddStyle();
}

void KOntourViewIface::slotDeleteStyle()
{
  m_view->slotDeleteStyle();
}

void KOntourViewIface::slotDistribute()
{
  m_view->slotDistribute();
}

void KOntourViewIface::slotConvertToPath()
{
  m_view->slotConvertToPath();
}

void KOntourViewIface::slotBlend()
{
  m_view->slotBlend();
}

void KOntourViewIface::slotOptions()
{
  m_view->slotOptions();
}







