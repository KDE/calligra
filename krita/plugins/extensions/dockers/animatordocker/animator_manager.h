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

class AnimatorSwitcher;
class AnimatorUpdater;
class AnimatorLoader;
class AnimatorPlayer;

#include "simple_frame_layer.h"
#include "framed_animated_layer.h"
#include "animator_meta_info.h"

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
    virtual AnimatorUpdater* getUpdater();
    virtual AnimatorLoader* getLoader();
    virtual AnimatorPlayer* getPlayer();
    
    virtual KisImage* image();
    
public:
    virtual void layerFramesNumberChange(AnimatedLayer* layer, int number);
    
signals:
    void layerFramesNumberChanged(AnimatedLayer* layer, int number);
    
protected slots:
    virtual void framesNumberCheck(AnimatedLayer* layer, int number);
    
public:
    virtual AnimatorMetaInfo* kraMetaInfo();
    virtual AnimatorMetaInfo* metaInfo();
    
public:
    virtual void setFrameContent(SimpleFrameLayer* frame, KisNode* content);
    virtual void insertFrame(SimpleFrameLayer* frame, FramedAnimatedLayer* layer);
    virtual void removeFrame(KisNode* frame);
    
    virtual void moveFrames(KisGroupLayerSP to, KisGroupLayerSP from);
    
    virtual void insertLayer(AnimatedLayer* layer, KisNodeSP parent, int index);
    virtual void removeLayer(KisNode* layer);
    
public:
    virtual int framesNumber() const;
    
public:
    virtual void activate(int frameNumber, KisNode* node);
    
public:
    virtual QList<AnimatedLayer*> layers();
    
protected:
    virtual void layerAdded(AnimatedLayer* layer);
    virtual void layerRemoved(AnimatedLayer* layer);
    
private:
    KisImage* m_image;
    KisNodeManager* m_nodeManager;
    
    AnimatorSwitcher* m_switcher;
    AnimatorUpdater* m_updater;
    AnimatorLoader* m_loader;
    AnimatorPlayer* m_player;
    AnimatorMetaInfo* m_info;
    
//     AnimatorExporter* m_exporter;
    
    QList<AnimatedLayer*> m_layers;
    
    int m_framesNumber;
};

#endif

