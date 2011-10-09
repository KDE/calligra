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

#include "simple_animated_layer.h"

#include <sstream>

#include <iostream>
#include <iomanip>
#include "frame_layer.h"

SimpleAnimatedLayer::SimpleAnimatedLayer(const KisGroupLayer& source) : AnimatedLayer(source)
{
    m_frames.clear();
    m_loaded = false;
    m_updating = false;
}

bool SimpleAnimatedLayer::loaded()
{
    return m_loaded;
}

void SimpleAnimatedLayer::loadFrames()
{
    if (m_updating)
        return;
    
    m_updating = true;
    m_loaded = true;
    
    m_frames.clear();
    m_first_frame = 0;
    for ( qint32 i = 0; i < childCount(); ++i )
    {
        KisNode* chsource = at(i).data(); //childNodes(QStringList(),  KoProperties())[i];
        bool iskey;
        int fnum = getFrameFromName(chsource->name(), iskey);
        if (fnum >= 0)
        {
            FrameLayer* frame = 0;
            
            if (chsource->inherits("FrameLayer"))
            {
                frame = dynamic_cast<FrameLayer*>(chsource);
            } else
            {
                QString rname = chsource->name();
                
                frame = new FrameLayer(*dynamic_cast<KisGroupLayer*>( chsource ));
                
                frame->setName(getNameForFrame(fnum, iskey));
                frame->setNodeManager(getNodeManager());
                frame->at(0)->setName("_");
                
                getNodeManager()->insertNode(frame, this, i);
                
                frame->setContent(chsource->at(0).data());
                
                // To be sure active node is present
                getNodeManager()->activateNode(this);
                
                getNodeManager()->removeNode(chsource);
            }
            
            if (fnum == m_frames.size())
            {
                m_frames.append(frame);
            } else {
                if (fnum > m_frames.size())
                {
                    for (qint32 fc = fnum-m_frames.size()+1; fc; --fc)
                    {
                        m_frames.append(0);
                    }
                }
                m_frames[fnum] = frame;
            }
            if (fnum < m_first_frame)
                m_first_frame = fnum;
        }
    }
    
    m_updating = false;
}

FrameLayer* SimpleAnimatedLayer::getFrameAt(int num) const
{
    if (num >= dataStart() && num < dataEnd())
        return m_frames[num];
    return 0;
}

FrameLayer* SimpleAnimatedLayer::getKeyFrame(int num) const
{
    if (isKeyFrame(num))
    {
        return m_frames[num];
    }

    return 0;
}

FrameLayer* SimpleAnimatedLayer::getCachedFrame(int num) const
{
    FrameLayer* frame = getFrameAt(num);
    if (frame)
    {
        return frame;
    }

    return getPreviousKeyFrame(num);
}

int SimpleAnimatedLayer::getNextKey(int num) const
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

int SimpleAnimatedLayer::getPreviousKey(int num) const
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

bool SimpleAnimatedLayer::isKeyFrame(int num) const
{
    return num >= dataStart() && num < dataEnd() && m_frames[num] && m_frames[num]->isKeyFrame();
}

int SimpleAnimatedLayer::dataStart() const
{
    return m_first_frame;
}

int SimpleAnimatedLayer::dataEnd() const
{
    return m_frames.size();
}

const QString& SimpleAnimatedLayer::getNameForFrame(int num, bool iskey) const
{
    Q_UNUSED(iskey);
    
    std::stringstream ns;
    ns << "_frame_";
    ns << std::setfill('0') << std::setw(4) << num;
    std::string s;
    ns >> s;
    
    QString* t = new QString(s.c_str());
    return *t;
}

int SimpleAnimatedLayer::getFrameFromName(const QString& name, bool& iskey) const
{
    if (name.startsWith("_frame_"))
    {
        std::stringstream ns;
        ns << (name.mid(7).toAscii().data());
        int fnum;
        ns >> fnum;
        return fnum;
    }
    return -1;
}

QVariant SimpleAnimatedLayer::getVision(int role, int num, bool isCurrent)
{
    if (getFrameAt(num))
        return getFrameAt(num)->getVision(role, isCurrent);
    return AnimatedLayer::getVision(role, num, isCurrent);
}

void SimpleAnimatedLayer::insertFrame(int num, FrameLayer* frame, bool iskey)
{
    frame->setName(getNameForFrame(num, iskey));
    getNodeManager()->insertNode(frame, this, num);
}
