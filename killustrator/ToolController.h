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

#ifndef ToolController_h_
#define ToolController_h_

#include <qintdict.h>
#include <Tool.h>

class GDocument;
class Canvas;
class MainView;

//this one serves also as a signal proxy for the various tools
//this makes the tool objects smaller
class ToolController : public QObject
{
  Q_OBJECT
public:
  ToolController (MainView* view);
  ~ToolController ();

  //void registerTool (int id, Tool* tool);
  void registerTool (Tool* tool);
  Tool* getActiveTool ();

  void delegateEvent (QEvent *e, GDocument* doc, Canvas* canvas);

  void emitModeSelected (Tool::ToolID id, const QString &msg);
  void emitOperationDone (Tool::ToolID id);
  void emitActivated (Tool::ToolID id, bool b);
  void emitPartSelected (Tool::ToolID id, GObject *);


signals:
  void modeSelected (Tool::ToolID id, const QString &msg);
  void operationDone (Tool::ToolID id );
  void activated (Tool::ToolID id, bool);
  void partSelected (Tool::ToolID id, GObject *);

public slots:
   void toolSelected (Tool::ToolID id);
   void configureTool (Tool::ToolID id);

  //  void reset ();

private:
  QIntDict<Tool> tools;
  Tool* activeTool;
  MainView* mainView;
};

#endif
