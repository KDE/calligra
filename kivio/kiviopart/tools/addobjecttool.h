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
#ifndef ADDOBJECTTOOL_H
#define ADDOBJECTTOOL_H

#include "kivio_mousetool.h"

namespace Kivio
{

class AddObjectTool : public MouseTool
{
  Q_OBJECT
  public:
    AddObjectTool(KivioView* parent);
    ~AddObjectTool();

    virtual bool processEvent(QEvent* e);

  public slots:
    void setActivated(bool a);

  protected:
    void mousePress(QMouseEvent* e);
    void mouseRelease(QMouseEvent* e);
    void mouseMove(QMouseEvent* e);
};

}
#endif