/*
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


#ifndef ANIMATOR_LT_DOCK_H
#define ANIMATOR_LT_DOCK_H

#include <QDockWidget>

#include <KoCanvasObserverBase.h>

#include "animator_lt.h"
#include "animator_lt_view.h"
#include "animator_manager.h"

/**
 *  Light table docker
 */
class AnimatorLTDock : public QDockWidget, public KoCanvasObserverBase
{
    Q_OBJECT

public:
    AnimatorLTDock();
    virtual ~AnimatorLTDock();
    
public:
    virtual void setCanvas(KoCanvasBase* canvas);
    virtual void unsetCanvas();
    
protected:
    virtual void setupUI();
    
private:
    AnimatorLTView* m_view;
};

#endif // ANIMATOR_LT_DOCK_H
