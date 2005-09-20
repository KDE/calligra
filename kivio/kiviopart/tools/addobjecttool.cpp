/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2005 Peter Simonsson
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "addobjecttool.h"

namespace Kivio
{

AddObjectTool::AddObjectTool(KivioView* parent)
  : MouseTool(parent, "Add Object Mouse Tool")
{
}

AddObjectTool::~AddObjectTool()
{
}

bool AddObjectTool::processEvent(QEvent* e)
{
  switch (e->type())
  {
    case QEvent::MouseButtonPress:
      mousePress(static_cast<QMouseEvent*>(e));
      return true;
      break;

    case QEvent::MouseButtonRelease:
      mouseRelease(static_cast<QMouseEvent*>(e));
      return true;
      break;

    case QEvent::MouseMove:
      mouseMove(static_cast<QMouseEvent*>(e));
      return true;
      break;

    default:
      break;
  }

  return false;
}

void AddObjectTool::setActivated(bool a)
{
  if(a) {
    emit activated(this);
  }
}

void AddObjectTool::mousePress(QMouseEvent* e)
{
}

void AddObjectTool::mouseRelease(QMouseEvent* e)
{
}

void AddObjectTool::mouseMove(QMouseEvent* e)
{
}

}

#include "addobjecttool.moc"
