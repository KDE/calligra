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

#ifndef EditPointTool_h_
#define EditPointTool_h_

#include <Tool.h>

class QCursor;
class GObject;

class EditPointTool : public Tool {
public:
  enum Mode {
    MovePoint, InsertPoint, RemovePoint, Join, Split
  };

  EditPointTool (CommandHistory *history);
  ~EditPointTool ();

  virtual void processEvent (QEvent* e, GDocument* doc, Canvas* canvas);

  void setMode (Mode m);

  void activate (GDocument* doc, Canvas* canvas);
  void deactivate (GDocument* doc, Canvas* canvas);

//signals:
//    void activated(bool);

private:
  Mode mode;
  GObject* obj;
  int pointIdx;
  Coord startPos, lastPos;
  QCursor *cursor;
};

#endif
