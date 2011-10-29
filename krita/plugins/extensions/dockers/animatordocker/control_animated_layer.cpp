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

#include "control_animated_layer.h"
#include "control_frame_layer.h"

ControlAnimatedLayer::ControlAnimatedLayer(const KisGroupLayer& source): FramedAnimatedLayer(source)
{
}

ControlAnimatedLayer::ControlAnimatedLayer(KisImageWSP image, const QString& name, quint8 opacity): FramedAnimatedLayer(image, name, opacity)
{
}

ControlAnimatedLayer::~ControlAnimatedLayer()
{
}


QString ControlAnimatedLayer::aName() const
{
    return name().mid(12);
}

void ControlAnimatedLayer::setAName(const QString& name)
{
    setName("_anicontrol_"+name);
}

bool ControlAnimatedLayer::isKeyFrame(int num) const
{
    return frameAt(num);
}

FrameLayer* ControlAnimatedLayer::emptyFrame()
{
    return new ControlFrameLayer(image(), "", 255);
}


int ControlAnimatedLayer::nextFrame(int fnum)
{
    if (isKeyFrame(fnum))
    {
        ControlFrameLayer* cframe = qobject_cast<ControlFrameLayer*>(frameAt(fnum));
        if (!cframe->pass())
            return cframe->target();
    }
    
    return fnum+1;
}

void ControlAnimatedLayer::reset()
{
    FrameLayer* frame;
    foreach (frame, frames())
    {
        ControlFrameLayer* cframe = qobject_cast<ControlFrameLayer*>(frame);
        if (cframe)
            cframe->reset();
    }
}

void ControlAnimatedLayer::setLoop(int from, int to, int number)
{
    if (!isKeyFrame(to))
    {
        createFrame(to, true);
    }
    ControlFrameLayer* frame = qobject_cast<ControlFrameLayer*>(frameAt(to));
    if (!frame)
        return;
    
    frame->setTarget(from);
    frame->setRepeat(number);
    frame->reset();
}
