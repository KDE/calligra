/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998-99 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

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

#include <GObject.h>
#include <Tool.h>
#include <qcursor.h>

class GDocument;
class Canvas;

class SelectionTool : public Tool {
public:
  SelectionTool (CommandHistory* history);
  ~SelectionTool() { delete cursor; }

  enum ScaleType { None, Proporcional};

  virtual void processEvent (QEvent* e, GDocument* doc, Canvas* canvas);
  virtual void activate (GDocument* doc, Canvas*);
  virtual void deactivate (GDocument*, Canvas*);

  void processTabKeyEvent (GDocument*, Canvas*);

  void rotate (GDocument* doc, float dx, float dy, float xp, float yp,
               bool permanent = false);
  void scale (GDocument* doc, Canvas* canvas, int mask, float dx, float dy, int type = 0,
              bool permanent = false);
  void shear (GDocument* doc, int mask, float dx, float dy,
              bool permanent = false);
  void translate (GDocument* doc, Canvas* canvas,
                  float dx, float dy, bool snap, bool permanent = false);

  //virtual bool consumesRMBEvents () { return false; }

  void setPickMode () { state = S_Pick; }

//signals:
//  void partSelected (GObject *);

private:
  void processKeyPressEvent (QKeyEvent* e, GDocument* doc, Canvas* canvas);

  void processButtonPressForHelpline (QMouseEvent* e, GDocument* doc,
                                      Canvas* canvas);
  void processMouseMoveForHelpline (QMouseEvent* e, GDocument* doc,
                                    Canvas* canvas);
  void processButtonReleaseForHelpline (QMouseEvent* e, GDocument* doc,
                                        Canvas* canvas);

  void processButtonPressEvent (QMouseEvent* e, GDocument* doc,
                                Canvas* canvas);
  void processMouseMoveEvent (QMouseEvent* e, GDocument* doc, Canvas* canvas);
  void processButtonReleaseEvent (QMouseEvent* e, GDocument* doc,
                                  Canvas* canvas);

  enum State { S_Init, S_Pick, S_Intermediate1, S_Intermediate2,
               S_Translate, S_Scale, S_Rubberband, S_RotateSelect,
               S_Rotate, S_Shear, S_MoveRotCenter, S_Inactive };
  enum CursorType { C_Arrow, C_Size, C_Move, C_Rotate, C_Horiz, C_Vert };
  State state;
  CursorType ctype;
  Coord firstpos;
  Rect origbox;
  int oldmask;
  Coord selPoint[2];
  Coord rotCenter;
  int dragHorizHelpline, dragVertHelpline;
  QCursor *cursor;
};

#endif
