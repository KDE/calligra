/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 2002 Laurent Montel (lmontel@mandrakesoft.com)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include "kontour_view_iface.h"

#include <dcopclient.h>

#include "kontour_view.h"

KontourViewIface::KontourViewIface(KontourView *aView):
KoViewIface(aView)
{
  mView = aView;
}

void KontourViewIface::slotCopy()
{
  mView->slotCopy();
}

void KontourViewIface::slotPaste()
{
  mView->slotPaste();
}

void KontourViewIface::slotCut()
{
  mView->slotCut();
}

void KontourViewIface::slotDuplicate()
{
  mView->slotDuplicate();
}

void KontourViewIface::slotDelete()
{
  mView->slotDelete();
}

void KontourViewIface::slotSelectAll()
{
  mView->slotSelectAll();
}

void KontourViewIface::slotZoomIn()
{
  mView->slotZoomIn();
}

void KontourViewIface::slotZoomOut()
{
  mView->slotZoomOut();
}

void KontourViewIface::slotOutline()
{
  mView->slotOutline();
}

void KontourViewIface::slotNormal()
{
  mView->slotNormal();
}

void KontourViewIface::slotShowRuler(bool b)
{
  mView->slotCopy();
}

void KontourViewIface::slotShowGrid(bool b)
{
  mView->slotShowGrid(b);
}

void KontourViewIface::slotShowHelplines(bool b)
{
  mView->slotShowHelplines(b);
}

void KontourViewIface::slotShowPaintPanel(bool b)
{
  mView->slotShowPaintPanel(b);
}

void KontourViewIface::slotShowOutlinePanel(bool b)
{
  mView->slotShowOutlinePanel(b);
}

void KontourViewIface::slotAlignToGrid(bool b)
{
  mView->slotAlignToGrid(b);
}

void KontourViewIface::slotAlignToHelplines(bool b)
{
  mView->slotAlignToHelplines(b);
}

void KontourViewIface::slotToFront()
{
  mView->slotToFront();
}

void KontourViewIface::slotToBack()
{
  mView->slotToBack();
}

void KontourViewIface::slotForwardOne()
{
  mView->slotForwardOne();
}

void KontourViewIface::slotBackOne()
{
  mView->slotBackOne();
}

void KontourViewIface::slotGroup()
{
  mView->slotGroup();
}

void KontourViewIface::slotUngroup()
{
  mView->slotUngroup();
}

void KontourViewIface::slotConvertToPath()
{
  mView->slotConvertToPath();
}

void KontourViewIface::slotAddStyle()
{
  mView->slotAddStyle();
}

void KontourViewIface::slotDeleteStyle()
{
  mView->slotDeleteStyle();
}

void KontourViewIface::slotOptions()
{
  mView->slotOptions();
}
