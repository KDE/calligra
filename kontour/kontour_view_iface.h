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

#ifndef __kontour_view_iface_h__
#define __kontour_view_iface_h__

#include <KoViewIface.h>

#include <qstring.h>

class KontourView;

class KontourViewIface : public KoViewIface
{
  K_DCOP
public:
  KontourViewIface(KontourView *aView);
k_dcop:
  void slotCopy();
  void slotPaste();
  void slotCut();
  void slotDuplicate();
  void slotDelete();
  void slotSelectAll();
  void slotZoomIn();
  void slotZoomOut();
  void slotOutline();
  void slotNormal();
  void slotShowRuler(bool b);
  void slotShowGrid(bool b);
  void slotShowHelplines(bool b);
  void slotShowPaintPanel(bool b);
  void slotShowOutlinePanel(bool b);
  void slotAlignToGrid(bool b);
  void slotAlignToHelplines(bool b);
  void slotToFront();
  void slotToBack();
  void slotForwardOne();
  void slotBackOne();
  void slotGroup();
  void slotUngroup();
  void slotConvertToPath();
  void slotAddStyle();
  void slotDeleteStyle();
  void slotOptions();

private:
  KontourView *mView;
};

#endif
