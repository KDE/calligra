/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

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

#ifndef SelectionTool_h_
#define SelectionTool_h_

#include <qobject.h>
#include <qevent.h>

#include "GObject.h"
#include "Tool.h"

class GDocument;
class Canvas;

class SelectionTool : public Tool {
  Q_OBJECT
public:
  SelectionTool (CommandHistory* history);
  
  virtual void processEvent (QEvent* e, GDocument* doc, Canvas* canvas);
  virtual void activate (GDocument* doc, Canvas*);
  virtual void deactivate (GDocument*, Canvas*);
  
  void rotate (GDocument* doc, int dx, int dy, int xp, int yp,
	       bool permanent = false);
  void scale (GDocument* doc, int mask, int dx, int dy, 
	      bool permanent = false);
  void translate (GDocument* doc, int dx, int dy, bool permanent = false);
  
private:
  void processKeyPressEvent (QKeyEvent* e, GDocument* doc, Canvas* canvas);
  void processButtonPressEvent (QMouseEvent* e, GDocument* doc, 
				Canvas* canvas);
  void processMouseMoveEvent (QMouseEvent* e, GDocument* doc, Canvas* canvas);
  void processButtonReleaseEvent (QMouseEvent* e, GDocument* doc, 
				  Canvas* canvas);

  enum State { S_Init, S_Pick, S_Intermediate1, S_Intermediate2, 
	       S_Translate, S_Scale, S_Rubberband, S_RotateSelect,
	       S_Rotate, S_Shear, S_MoveRotCenter };
  enum CursorType { C_Arrow, C_Size, C_Move, C_Rotate };
  State state;
  CursorType ctype;
  QPoint firstpos;
  Rect origbox;
  int oldmask;
  Coord selPoint[2];
  Coord rotCenter;
};

#endif
