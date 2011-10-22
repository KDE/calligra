/*
 *  
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

#ifndef ANIMATOR_MANAGER_H
#define ANIMATOR_MANAGER_H

#include <QObject>

#include <KoCanvasObserverBase.h>

#include <kis_image.h>
#include <kis_node_manager.h>

// #include "animator_switcher.h"
// #include "animator_loader.h"
class AnimatorLoader;
class AnimatorSwitcher;

// #include "animator_player.h"
// #include "animator_exporter.h"

#include "simple_frame_layer.h"
#include "framed_animated_layer.h"

class AnimatorManager : public QObject, KoCanvasObserverBase {
    Q_OBJECT
    
public:
    AnimatorManager(KisImage* image);
    virtual ~AnimatorManager();
    
public:
    virtual void setCanvas(KoCanvasBase* canvas);
    virtual void unsetCanvas();
    
    virtual bool ready();
    
public:
    virtual AnimatorSwitcher* getSwitcher();
    virtual AnimatorLoader* getLoader();
    
    virtual KisImage* image();
    
public:
    virtual void setFrameContent(SimpleFrameLayer* frame, KisNode* content);
    virtual void insertFrame(SimpleFrameLayer* frame, FramedAnimatedLayer* layer);
    virtual void removeFrame(KisNode* frame);
    
    virtual void moveFrames(KisGroupLayerSP to, KisGroupLayerSP from);
    
    virtual void insertLayer(AnimatedLayer* layer, KisNodeSP parent, int index);
    virtual void removeLayer(KisNode* layer);
    
private:
    KisImage* m_image;
    KisNodeManager* m_nodeManager;
    
    AnimatorSwitcher* m_switcher;
    AnimatorLoader* m_loader;
//     AnimatorPlayer* m_player;
//     AnimatorExporter* m_exporter;
};

#endif

