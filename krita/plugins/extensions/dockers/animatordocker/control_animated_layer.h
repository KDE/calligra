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

#ifndef CONTROL_ANIMATED_LAYER_H
#define CONTROL_ANIMATED_LAYER_H

#include "framed_animated_layer.h"


class ControlAnimatedLayer : public FramedAnimatedLayer
{
    Q_OBJECT
    
public:
    ControlAnimatedLayer(const KisGroupLayer& source);
    ControlAnimatedLayer(KisImageWSP image, const QString& name, quint8 opacity);
    virtual ~ControlAnimatedLayer();

public:
    virtual bool displayable() const;
    virtual QString aName() const;
    virtual void setAName(const QString& name);
    virtual bool isKeyFrame(int num) const;
    virtual FrameLayer* emptyFrame();
    
public:
    // Player interface
    virtual void reset();
    virtual int nextFrame(int fnum);
    
    // Control interface
    virtual void setLoop(int from, int to, int number);
};

#endif // CONTROL_ANIMATED_LAYER_H
