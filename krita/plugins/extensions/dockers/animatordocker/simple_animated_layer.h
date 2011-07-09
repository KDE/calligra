/*
 *  Simple animated layer -- just group layer with frames as it was in old
 *  AnimatorModel before splitting.
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


#ifndef SIMPLE_ANIMATED_LAYER_H
#define SIMPLE_ANIMATED_LAYER_H

#include "animated_layer.h"

class SimpleAnimatedLayer : public AnimatedLayer
{
    Q_OBJECT
    
public:
    SimpleAnimatedLayer(const KisGroupLayer& source);
    
public slots:
    virtual void update();
    virtual void frameUpdate();
    virtual void frameUpdate(bool do_all);
    
public:
    virtual KisNode* getFrameLayer(int num);
    virtual KisNode* getKeyFrameLayer(int num);
    virtual KisNode* getPreviousKeyFrame(int num);
    virtual KisNode* getNextKeyFrame(int num);
    
    virtual bool isFrameChanged();
    
public:
    virtual int firstFrame();
    virtual int lastFrame();

protected:
    virtual void visibleFrame(int f, bool v);
    virtual void visibleAll(bool v);
    
private:
    QList< KisNode* > m_frames;
};

#endif // SIMPLE_ANIMATED_LAYER_H
