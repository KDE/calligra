/*
 *  Animated layer class
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


#ifndef ANIMATED_LAYER_H
#define ANIMATED_LAYER_H

#include "kis_group_layer.h"
#include "kis_node_manager.h"

#include "frame_layer.h"

class AnimatedLayer : public KisGroupLayer
{
    Q_OBJECT

// signals:
//     void requireRedraw();

public:
    AnimatedLayer(const KisGroupLayer& source);
    
public:
//     virtual bool needProjection() const;
    
    // Animation interface
    // NEW
public:
    virtual FrameLayer* getUpdatedFrame(int num);
    virtual FrameLayer* getCachedFrame(int num) const = 0;
    
    virtual FrameLayer* getKeyFrame(int num) const = 0;
    virtual FrameLayer* getNextKeyFrame(int num) const;
    virtual FrameLayer* getPreviousKeyFrame(int num) const;
    
    virtual bool isKeyFrame(int num) const = 0;
    virtual int getNextKey(int num) const = 0;
    virtual int getPreviousKey(int num) const = 0;
    
    virtual void loadFrames() = 0;
    virtual bool loaded() = 0;
    
    virtual QVariant getVision(int role, bool isCurrent);
    
public:
//     virtual const QString& getNameForFrame(int num) const;
    virtual const QString& getNameForFrame(int num, bool iskey) const = 0;
    
protected:
    virtual int getFrameFromName(const QString& name, bool &iskey) const = 0;
    
public:
    /**
     * @return number of first frame with some info
     */
    virtual int dataStart() const = 0;
    
    /**
     * @return number of last frame with some info+1 [firstFrame; lastFrame)
     */
    virtual int dataEnd() const = 0;

    // May be this should be removed
    void setNodeManager(KisNodeManager* nodeman);
    KisNodeManager* getNodeManager() const;

protected:
    virtual void updateFrame(int num);
    
    virtual void insertFrame(int num, FrameLayer* frame, bool iskey) = 0;
    
private:
    KisNodeManager* m_nodeman;
    
//     bool m_frame_changed;
    
//     KisGroupLayer* m_source;
};

#endif // ANIMATED_LAYER_H
