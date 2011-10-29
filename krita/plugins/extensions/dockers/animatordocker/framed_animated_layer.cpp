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

#include "framed_animated_layer.h"

#include <sstream>
#include <iomanip>

#include "frame_layer.h"
#include "simple_frame_layer.h"
#include "animator_manager.h"
#include "animator_manager_factory.h"

FramedAnimatedLayer::FramedAnimatedLayer(KisImageWSP image, const QString& name, quint8 opacity): AnimatedLayer(image, name, opacity)
{
    m_firstFrame = 0;
}

FramedAnimatedLayer::FramedAnimatedLayer(const KisGroupLayer& source) : AnimatedLayer(source)
{
    m_firstFrame = 0;
}


QString FramedAnimatedLayer::aName() const
{
    return name().mid(5);
}

void FramedAnimatedLayer::setAName(const QString& name)
{
    setName("_ani_"+name);
}

bool FramedAnimatedLayer::displayable() const
{
    return true;
}


FrameLayer* FramedAnimatedLayer::frameAt(int num) const
{
    if (num >= dataStart() && num < dataEnd())
        return m_frames[num];
    return 0;
}

void FramedAnimatedLayer::insertFrame(FrameLayer* frame)
{
    AnimatorManager* manager = AnimatorManagerFactory::instance()->getManager(image().data());
    manager->putNodeAt(frame, this, 0);
    
    bool isKey;
    int fnum = getFrameFromName(frame->name(), isKey);
    if (fnum < 0)
    {
        return;
    }
    if (fnum >= m_frames.size())
    {
        while (m_frames.size() < fnum)
            m_frames.append(0);
        m_frames.append(frame);
    } else
    {
        Q_ASSERT(m_frames[fnum] == 0);
        m_frames[fnum] = frame;
    }
    
    manager->layerFramesNumberChange(this, dataEnd());
}

void FramedAnimatedLayer::createFrame(int num, bool isKey)
{
    FrameLayer* frame = emptyFrame();
    frame->setName(getNameForFrame(num, isKey));
    insertFrame(frame);
}

FrameLayer* FramedAnimatedLayer::emptyFrame()
{
    return new SimpleFrameLayer(image(), "", 255);
}

void FramedAnimatedLayer::removeFrameAt(int num)
{
    AnimatorManager* manager = AnimatorManagerFactory::instance()->getManager(image().data());
    manager->removeFrame(frameAt(num));
    m_frames[num] = 0;
}


FrameLayer* FramedAnimatedLayer::getKeyFrame(int num) const
{
    if (isKeyFrame(num))
    {
        return m_frames[num];
    }

    return 0;
}

FrameLayer* FramedAnimatedLayer::getCachedFrame(int num) const
{
    FrameLayer* frame = frameAt(num);
    if (frame)
    {
        return frame;
    }

    return frameAt(getPreviousKey(num));
}

int FramedAnimatedLayer::getNextKey(int num) const
{
    if (num < dataStart())
        num = -1;
    if (num >= dataEnd())
        return 0;
    while (++num < dataEnd())
    {
        if (isKeyFrame(num))
            return num;
    }
    return -1;
}

int FramedAnimatedLayer::getPreviousKey(int num) const
{
    if (num < dataStart())
        return 0;
    if (num >= dataEnd())
        num = dataEnd();
    while (--num >= dataStart())
    {
        if (isKeyFrame(num))
            return num;
    }
    return -1;
}

bool FramedAnimatedLayer::isKeyFrame(int num) const
{
    return frameAt(num) && qobject_cast<SimpleFrameLayer*>(frameAt(num))->isKeyFrame();
}

int FramedAnimatedLayer::dataStart() const
{
    return m_firstFrame;
}

int FramedAnimatedLayer::dataEnd() const
{
    return m_frames.size();
}

QString FramedAnimatedLayer::getNameForFrame(int num, bool iskey) const
{
    std::stringstream ns;
    ns << "_frame_";
    ns << std::setfill('0') << std::setw(4) << num;
    if (!iskey)
        ns << "_";
    std::string s;
    ns >> s;
    
    QString* t = new QString(s.c_str());
    return *t;
}

int FramedAnimatedLayer::getFrameFromName(const QString& name, bool& iskey) const
{
    if (name.startsWith("_frame_"))
    {
        iskey = !name.endsWith("_");
        
        std::stringstream ns;
        ns << (name.mid(7).toAscii().data());
        int fnum;
        ns >> fnum;
        return fnum;
    }
    return -1;
}

QList< FrameLayer* > FramedAnimatedLayer::frames()
{
    return m_frames;
}
