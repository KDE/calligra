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

#ifndef Tool_h_
#define Tool_h_

#include <GObject.h>
#include <qnamespace.h>

class GDocument;
class Canvas;
class CommandHistory;
class QEvent;
class ToolController;

class Tool
{
   friend class ToolController;
protected:
  Tool (CommandHistory* chist);

public:

  virtual ~Tool () {}
  enum ToolID {ToolDummy, ToolSelect, ToolEditPoint, ToolFreeHand, ToolLine,\
  ToolBezier, ToolRectangle, ToolPolygon, ToolEllipse, ToolText, ToolZoom, ToolPathText, ToolInsertPart};

  virtual void processEvent (QEvent* e, GDocument* doc, Canvas* canvas) = 0;
  virtual void activate (GDocument* doc, Canvas* canvas);
  virtual void deactivate (GDocument*, Canvas*) {}

  virtual void configure () {}

  ToolID id() const {return m_id;};

protected:
  CommandHistory* history;
  QString msgbuf;
  ToolID m_id;
  bool m_configRead;
  ToolController *m_toolController;
};

#endif
