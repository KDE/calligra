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

#ifndef Handle_h_
#define Handle_h_

#include <Coord.h>

class QPainter;
class GDocument;

#define Handle_Top     1
#define Handle_Right   2
#define Handle_Bottom  4
#define Handle_Left    8
#define Handle_Center  16

class Handle
{
public:
  enum Mode { HMode_Default, HMode_Rotate };
  enum Position { HPos_Top = 1, HPos_Right = 2, HPos_Bottom = 4,
                  HPos_Left = 8, HPos_Center = 16 };

  Handle (GDocument* parent);
  ~Handle () {};

  void setBox (const Rect& r);
  void setRotCenter (const Coord& p);
  Coord rotCenter () const { return rcenter; }

  void draw (QPainter& p);
  int contains (const Coord& p);
  void setMode (Mode m, bool propagate = false);
  void show (bool flag = true);

private:
  enum ArrowDirection { Arrow_Left, Arrow_Right, Arrow_Up, Arrow_Down };

  void drawArrow (QPainter& p, int x, int y, ArrowDirection d);

  GDocument* m_parentDoc;
  Coord pos[8];
  Rect box;
  Coord rcenter;
  Mode mode;
  bool showIt;
};

#endif
