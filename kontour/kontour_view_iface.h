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

#ifndef KONTOUR_VIEW_IFACE_H
#define KONTOUR_VIEW_IFACE_H

#include <KoViewIface.h>

#include <qstring.h>

class KontourView;

class KOntourViewIface : public KoViewIface
{
    K_DCOP
public:
    KOntourViewIface( KontourView *view_ );
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
  void slotShowRuler(bool);
  void slotShowGrid(bool);
  void slotShowHelplines(bool);
  void slotShowPaintPanel(bool);
  void slotShowOutlinePanel(bool);
  void slotAlignToGrid(bool);
  void slotAlignToHelplines(bool);
  void slotToFront();
  void slotToBack();
  void slotForwardOne();
  void slotBackOne();
  void slotGroup();
  void slotUngroup();
  void slotAddStyle();
  void slotDeleteStyle();
  void slotDistribute();
  void slotConvertToPath();
  void slotBlend();
  void slotOptions();

private:
    KontourView *m_view;
};

#endif
