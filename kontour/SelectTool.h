/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2001-2002 Igor Janssen (rm@kde.org)

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

#ifndef __SelectTool_h__
#define __SelectTool_h__

#include "Tool.h"
#include <koPoint.h>

namespace Kontour {
class GPage;
class Canvas;

class SelectTool : public Tool
{
  Q_OBJECT
public:
  SelectTool(QString aId, ToolController *tc);
  
  void activate();
  void deactivate();
  void processEvent(QEvent *e);

  void translate(GPage *page, double dx, double dy, bool snap, bool permanent = false);
  void scale(GPage *page, double dx, double dy, bool type = 0, bool permanent = false);
  void shear(GPage *page, double dx, double dy, bool permanent = false);
  void rotate(GPage *page, double xf, double yf, double xp, double yp, bool permanent = false);

private:
  void processButtonPressEvent(QMouseEvent *e, GPage *page, Canvas *canvas);
  void processMouseMoveEvent(QMouseEvent *e, GPage *page, Canvas *canvas);
  void processButtonReleaseEvent(QMouseEvent *e, GPage *page, Canvas *canvas);
  void processKeyPressEvent(QKeyEvent *e, GPage *page, Canvas *canvas);

private:
  enum State{ S_Init, S_Rubberband, S_Pick, S_Press, S_Translate, S_Scale, S_Shear, S_Rotate, S_DragHorizHelpline, S_DragVertHelpline, S_MoveRotCenter};
  State state;
  enum CursorType{ C_Arrow, C_Size, C_Move, C_Shear, C_Rotate, C_MoveRotCenter, C_Horiz, C_Vert};
  CursorType ctype;
  KoPoint fp;
  QPoint p1;
  QRect r;
  int mHL;
  int prevcoord;
  int mask;
};
};
using namespace Kontour;

#endif
