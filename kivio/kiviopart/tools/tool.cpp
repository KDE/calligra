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
#include <iostream.h>
#include "tool.h"

#include "kivio_factory.h"
#include "kivio_view.h"
#include "kivio_canvas.h"
#include "kivio_page.h"

Tool::Tool(KivioView* view, const char* name)
: KParts::Plugin(view,name)
{
  setInstance(KivioFactory::global());
  m_pView = view;
  m_pCanvas = 0L;
  m_pOldActiveTool = 0L;

  controller()->registerTool(this);
}

Tool::~Tool()
{
}

ToolController* Tool::controller()
{
  return m_pView->toolsController();
}

ToolSelectAction* Tool::action()
{
  return (ToolSelectAction*)actionCollection()->action("ToolAction");
}

void Tool::setOverride()
{
  m_pOldActiveTool = controller()->getActiveTool();
  if ( m_pOldActiveTool == this ) {
    m_pOldActiveTool = 0L;
    return ;
  }

  controller()->selectTool(this);
}

void Tool::removeOverride()
{
  if (m_pOldActiveTool)
    controller()->selectTool(m_pOldActiveTool);
}
  ToolSelectAction* m_pTool;
#include "tool.moc"
