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

#include <ToolController.h>
#include <GDocument.h>
#include <GLayer.h>
#include <MainView.h>
#include <kdebug.h>


ToolController::ToolController (MainView* view) {
  activeTool = 0;
  mainView = view;
}

ToolController::~ToolController () {
}

void ToolController::emitModeSelected (Tool::ToolID id, const QString &msg)
{
   emit modeSelected(id, msg);
};

void ToolController::emitOperationDone (Tool::ToolID id)
{
   emit operationDone(id);
};

void ToolController::emitActivated (Tool::ToolID id, bool b)
{
   emit activated(id,b);
};

void ToolController::emitPartSelected (Tool::ToolID id, GObject *o)
{
   emit partSelected(id,o);
};

void ToolController::registerTool (Tool* tool)
{
   if (tool==0)
      return;
  tools.insert ((long) tool->id(), tool);
  tool->m_toolController=this;
}

Tool* ToolController::getActiveTool ()
{
  return activeTool;
}

void ToolController::delegateEvent (QEvent *e, GDocument *doc,
                                    Canvas *canvas) {
  if (doc->activeLayer ()->isEditable () && activeTool)
    activeTool->processEvent (e, doc, canvas);
}

/*
void ToolController::reset () {
  toolSelected (0);
}
*/

void ToolController::toolSelected (Tool::ToolID id)
{
   if (activeTool)
   {
      if (activeTool->id()==id) return;
      activeTool->deactivate (mainView->activeDocument (),
                              mainView->getCanvas ());
   };

   activeTool = tools.find ((long) id);
   if (activeTool)
      activeTool->activate(mainView->activeDocument (),mainView->getCanvas());
}

void ToolController::configureTool (Tool::ToolID id)
{
   Tool *t = tools.find ((long) id);
   if (t)
      t->configure ();
}

#include <ToolController.moc>
