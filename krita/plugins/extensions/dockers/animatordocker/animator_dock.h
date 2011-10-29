/*
 *  Docker -- almost all gui is here
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


#ifndef _ANIMATOR_DOCK_H_
#define _ANIMATOR_DOCK_H_

#include <QDockWidget>

#include "kis_view2.h"
#include "KoCanvasObserverBase.h"
#include "kis_node_model.h"
#include "KoMainWindow.h"

#include "animator_model.h"
#include "animator_view.h"
#include "animator_manager.h"

class AnimatorDock : public QDockWidget, public KoCanvasObserverBase
{
    Q_OBJECT
    
public:
    AnimatorDock();

public slots:
    virtual void setCanvas(KoCanvasBase* canvas);
    virtual void unsetCanvas();
    
protected slots:
    virtual void setFrameWidth(int width);
    virtual void setShowThumbs(bool val);
    
private:
    virtual void setupUI();
    
private:
    AnimatorManager* m_manager;
    
private:
    AnimatorModel* m_mainModel;
    AnimatorView* m_view;
};


#endif
