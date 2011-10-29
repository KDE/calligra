/*
 *  Control docker
 *  Copyright (C) 2011 Torio Mlshi <mlshi@lavabit.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef ANIMATOR_CONTROL_DOCK_H
#define ANIMATOR_CONTROL_DOCK_H

#include <QDockWidget>
#include <QToolBar>

#include "kis_view2.h"
#include "KoCanvasObserverBase.h"
#include "kis_node_model.h"
#include "KoMainWindow.h"

#include "animator_manager.h"
#include "animator_actions.h"

class AnimatorControlDock : public QDockWidget, public KoCanvasObserverBase
{
    Q_OBJECT
    
public:
    AnimatorControlDock();

public slots:
    virtual void setCanvas(KoCanvasBase* canvas);
    virtual void unsetCanvas();

protected slots:
    virtual void setupFramesToolbar();
    
protected:
    virtual void setupUI();
    
private:
    AnimatorManager* m_manager;
    
    AnimatorActions* m_actions;
    
private:
    QToolBar* m_tbFrames;
};


#endif // ANIMATOR_CONTROL_DOCK_H
