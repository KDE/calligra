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

#include <kis_group_layer.h>

#include "kis_node_manager.h"

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
public:
    virtual KisNode* getFrameLayer(int num);
    virtual KisNode* getOldFrameLayer(int num);
    virtual KisNode* getKeyFrameLayer(int num) = 0;
    
    virtual KisNode* getPreviousKeyFrame(int num);
    virtual KisNode* getNextKeyFrame(int num);
    
    virtual int getPreviousKey(int num) = 0;
    virtual int getNextKey(int num) = 0;
    virtual int getCurrentKey(int num) = 0;
    
    virtual bool isFrameChanged() = 0;
    
    virtual KisNode* getCachedFrame();
    
    virtual void clearJunk() = 0;
    
    /**
     * @return number of first frame with some info
     */
    virtual int firstFrame() = 0;
    
    /**
     * @return number of last frame with some info+1 [firstFrame; lastFrame)
     */
    virtual int lastFrame() = 0;
    
    KisNode* setFrameNumber(int num);
    int getFrameNumber();
    
    int getOldFrame();
    
public slots:
    virtual void update() = 0;
    virtual KisNode* frameUpdate() = 0;
    
public:
    bool isValid();
    
public:
    void setNodeManager(KisNodeManager* nodeman);
    KisNodeManager* getNodeManager();
//     KisNode* source();          // This is temporary function; should be deprecated after full moving to AnimatedLayer
    
protected:
    
    virtual KisNode* getFrameAt(int num) = 0;
    
    void setValid(bool valid);
//     void frameChange(bool ch);
    
private:
    int m_old_frame;
    int m_frame;
    
    bool m_valid;
    
    KisNodeManager* m_nodeman;
    
//     bool m_frame_changed;
    
//     KisGroupLayer* m_source;
};

#endif // ANIMATED_LAYER_H
