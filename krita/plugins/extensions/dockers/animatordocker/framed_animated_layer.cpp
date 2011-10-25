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

#include <KoProperties.h>

#include "framed_animated_layer.h"

#include <sstream>

#include <iomanip>
#include "frame_layer.h"
#include "simple_frame_layer.h"
#include "animator_manager.h"
#include "animator_manager_factory.h"

FramedAnimatedLayer::FramedAnimatedLayer(KisImageWSP image, const QString& name, quint8 opacity): AnimatedLayer(image, name, opacity)
{
}

FramedAnimatedLayer::FramedAnimatedLayer(const KisGroupLayer& source) : AnimatedLayer(source)
{
}


void FramedAnimatedLayer::rename(const QString& name)
{
    setName("_ani_"+name);
}


void FramedAnimatedLayer::init()
{
    m_frames.clear();
    m_first_frame = 0;
    
    KisNodeSP child = firstChild();
    while (child)
    {
        SimpleFrameLayer* frame = dynamic_cast<SimpleFrameLayer*>(child.data());
        if (frame)
        {
            bool isKey;
            int fnum = getFrameFromName(frame->name(), isKey);
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
        }
        
        child = child->nextSibling();
    }
    
    AnimatorManager* manager = AnimatorManagerFactory::instance()->getManager(image().data());
    manager->layerFramesNumberChange(this, dataEnd());
}


FrameLayer* FramedAnimatedLayer::frameAt(int num) const
{
    if (num >= dataStart() && num < dataEnd())
        return m_frames[num];
    return 0;
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
    return num >= dataStart() && num < dataEnd() && m_frames[num] && m_frames[num]->isKeyFrame();
}

int FramedAnimatedLayer::dataStart() const
{
    return m_first_frame;
}

int FramedAnimatedLayer::dataEnd() const
{
    return m_frames.size();
}

const QString& FramedAnimatedLayer::getNameForFrame(int num, bool iskey) const
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
        iskey = name.endsWith("_");
        
        std::stringstream ns;
        ns << (name.mid(7).toAscii().data());
        int fnum;
        ns >> fnum;
        return fnum;
    }
    return -1;
}

// FrameLayer* FramedAnimatedLayer::insertFrame(int num, FrameLayer* frame, bool iskey)
// {
//     frame->setName(getNameForFrame(num, iskey));
//     getNodeManager()->insertNode(frame, this, num);
//     return frame;
// }
// 
// FrameLayer* FramedAnimatedLayer::createFrameLayer(KisGroupLayer& src)
// {
//     return new FrameLayer(src);
// }
// 
// FrameLayer* FramedAnimatedLayer::createFrame(int num, KisGroupLayer* source, bool iskey)
// {
//     FrameLayer* nf = createFrameLayer(*source);
//     nf->setName(getNameForFrame(num, iskey));
//     nf->setNodeManager(getNodeManager());
//     getNodeManager()->activateNode(this);
//     getNodeManager()->insertNode(nf, this, num);
//     nf->setContent(source->at(0).data());
//     getNodeManager()->removeNode(source);
//     return nf;
// }
