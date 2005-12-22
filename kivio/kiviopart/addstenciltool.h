/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2005 Peter Simonsson <psn@linux.se>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef KIVIOADDSTENCILTOOL_H
#define KIVIOADDSTENCILTOOL_H

#include <KoPoint.h>
#include <KoSize.h>

#include "kivio_mousetool.h"

class KivioStencilSpawner;

namespace Kivio {

/**
 * Tool used to add new stencils to the canvas
 */
class AddStencilTool : public MouseTool
{
  Q_OBJECT
  public:
    AddStencilTool(KivioView* parent);
    ~AddStencilTool();

    virtual bool processEvent(QEvent* e);

  public slots:
    void setActivated(bool a);
    void activateNewStencil(KivioStencilSpawner* spawner);

  protected:
    void createStencil(const QPoint& position);
    void endOperation(const QPoint& position);
    void resize(const QPoint& position);

  private:
    bool m_leftMouseButtonPressed;
    KivioStencilSpawner* m_spawner;
    KoPoint m_startPoint;
    KivioStencil* m_stencil;
    KoSize m_originalSize;
};

}

#endif
