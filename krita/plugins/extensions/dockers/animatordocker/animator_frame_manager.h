/*
 *  Frame manager control all actions related to frames
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

#ifndef ANIMATOR_FRAME_MANAGER_H
#define ANIMATOR_FRAME_MANAGER_H

#include <QObject>
#include "animator_manager.h"


class AnimatorFrameManager : public QObject
{
    Q_OBJECT
    
public:
    AnimatorFrameManager(AnimatorManager* amanager);
    virtual ~AnimatorFrameManager();
    
public:
    // These four functions work on all layers
    virtual void removeRange(int n);
    virtual void removeRange(int from, int n);
    
    virtual void insertRange(int n);
    virtual void insertRange(int from, int n);
    
    // These can work on all or on selected layer
    virtual void moveRange(int n, int dist);
    virtual void moveRange(int from, int n, int dist);
    virtual void moveRange(FramedAnimatedLayer* layer, int from, int n, int dist);
    virtual void moveRangeActive(int n, int dist);
    
    virtual void clearRange(int n);
    virtual void clearRange(int from, int n);
    virtual void clearRange(FramedAnimatedLayer* layer, int from, int n);
    virtual void clearRangeActive(int n);
    
    //
    virtual void copyPreviousKey();
    virtual void copyNextKey();
    virtual void copyFrame(FramedAnimatedLayer *layer, int from, int to);
    
    /**
     * This function just move source frame to target position;
     * no checking for frame at target position.
     */
    virtual void moveTo(FramedAnimatedLayer* source, int sourceFrame, int targetFrame);
    
    /**
     * Like previous function, but between different layers.
     * Not implemented yer
     */
    virtual void moveTo(FramedAnimatedLayer* source, int sourceFrame,
                        FramedAnimatedLayer* target, int targetFrame);
    
private:
    AnimatorManager* m_manager;
};

#endif // ANIMATOR_FRAME_MANAGER_H
