/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000 theKompany.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#ifndef TOOL_H
#define TOOL_H

#include <qobject.h>
#include <qevent.h>
#include <qkeycode.h>

#include <klocale.h>

#include "tool_controller.h"

class KivioView;
class KivioPage;
class KivioCanvas;
class ToolSelectAction;

#include <kparts/plugin.h>

class Tool : public KParts::Plugin
{ Q_OBJECT
public:
  Tool(KivioView* view, const char* name);
  virtual ~Tool();

  virtual void processEvent( QEvent* e ) = 0;
  virtual void activate() {};
  virtual void deactivate() {};
  virtual void configure() {};

  virtual void activateGUI( KXMLGUIFactory* ) {};
  virtual void deactivateGUI( KXMLGUIFactory* ) {};

  virtual ToolSelectAction* action();

  void setCanvasWidget( KivioCanvas* c ) { m_pCanvas = c; }
  KivioCanvas* canvasWidget() { return m_pCanvas; }

  int sortNum() { return m_sort; }
  void setSortNum(int i) { m_sort = i; }

protected:
  ToolController* controller();

  /** manualy activate this tool if his not be active
   *  and store old active tool in m_pOldActiveTool
   */
  void setOverride();

  /** Restore tool from m_pOldActiveTool
   */
  void removeOverride();

  Tool* m_pOldActiveTool;

  KivioCanvas* m_pCanvas;
  KivioView* m_pView;

signals:
  void operationDone();

private:
  int m_sort;
};

inline bool operator<(Tool& t1, Tool& t2) { return t1.sortNum()<t2.sortNum(); }
inline bool operator==(Tool&, Tool&) { return false; }

#endif
