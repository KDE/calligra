/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
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

#ifndef __PathTool_h__
#define __PathTool_h__

#include "Tool.h"

namespace Kontour {
class GPath;

class PathTool : public Tool
{
  Q_OBJECT
public:
  PathTool(QString aId, ToolController *tc);

  void activate();
  void deactivate();
  void processEvent(QEvent *e);

private:
  enum Mode{Curve, Line, Polyline, Bezier, Spiral};
  enum State{S_Init, S_Resize};
  Mode           mode;
  State          state;
};
};
using namespace Kontour;

#endif
