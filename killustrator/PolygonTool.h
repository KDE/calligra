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

#ifndef PolygonTool_h_
#define PolygonTool_h_

#include <Tool.h>

class GPolygon;

class PolygonTool : public Tool {
public:
   PolygonTool (CommandHistory* history);

  virtual void processEvent (QEvent* e, GDocument* doc, Canvas* canvas);
  virtual void activate (GDocument* doc, Canvas* canvas);
  virtual void configure ();

  unsigned int numCorners () const;
  unsigned int sharpness () const;
  bool concavePolygon () const;

  void setNumCorners (unsigned int num);
  void setSharpness (unsigned int value);
  void setConcavePolygon (bool flag);
  void writeOutConfig();

private:
  GPolygon* obj;
  unsigned int nCorners;
  unsigned int sharpValue;
  bool createConcavePolygon;
  Coord sPoint;
};

#endif
