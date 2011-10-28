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
#include "simple_frame_layer.h"

class FramedAnimatedLayer : public AnimatedLayer
{
    Q_OBJECT
    
public:
    FramedAnimatedLayer(KisImageWSP image, const QString& name, quint8 opacity);
    FramedAnimatedLayer(const KisGroupLayer& source);

public:
    virtual QString aname() const;
    virtual void rename(const QString& name);
    
public:
    virtual FrameLayer* frameAt(int num) const;
//     virtual void setFrameAt(int num, FrameLayer* frame);

public:
    virtual FrameLayer* getCachedFrame(int num) const;
    virtual FrameLayer* getKeyFrame(int num) const;
    
public:
    virtual bool isKeyFrame(int num) const;

    virtual int getNextKey(int num) const;
    virtual int getPreviousKey(int num) const;

public:
    virtual int dataStart() const;
    virtual int dataEnd() const;
    
public:
    virtual void init();
    
public:
    virtual const QString& getNameForFrame(int num, bool iskey) const;
    
// protected:
    virtual int getFrameFromName(const QString& name, bool& iskey) const;
    
private:
    int m_first_frame;

    QList< SimpleFrameLayer* > m_frames;
};

#endif // SIMPLE_ANIMATED_LAYER_H
