/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2002 Igor Janssen (rm@kde.org)

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

#ifndef __PolygonTool_h__
#define __PolygonTool_h__

#include "Tool.h"
#include <koPoint.h>

namespace Kontour {
class GPolygon;

class PolygonTool : public Tool
{
  Q_OBJECT
public:
  PolygonTool(QString aId, ToolController *tc);

  void activate();
  void deactivate();
  void processEvent(QEvent *e);

private:
  void drawPolygon(double r, double a);
  void drawStar(double r1, double r2, double a);

private slots:
  void enableFill();
  void disableFill();
  void setPolygon();
  void setStar();

private:
  enum State{S_Init, S_Resize};
  State state;
  int n;
  KoPoint mCenter;
  double radius;
  bool mFill:1;
  enum Type{Polygon, Star};
  Type type;
};
};
using namespace Kontour;

#endif
