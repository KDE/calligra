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

SimpleAnimatedLayer::SimpleAnimatedLayer(const KisGroupLayer& source) : AnimatedLayer(source)
{
    m_frames.clear();
}

void SimpleAnimatedLayer::loadFrames()
{
    m_frames.clear();
    m_first_frame = 0;
    for ( qint32 i = 0; i < childCount(); ++i )
    {
        const KisNode* chsource = childNodes(QStringList(),  KoProperties())[i];
        bool iskey;
        int fnum = getFrameFromName(chsource->name(), iskey);
        if (fnum >= 0)
//         if (chsource->name().startsWith("_frame_"))
        {
            
            KisNode* frame = const_cast<KisNode*>(chsource);
//             QString ts = chsource->name();
//             
//             qint32 fnum = ts.mid(7).toLong();
//             
//             KisNode* frame = mchsource;
//             getNodeManager()->insertNode(frame, this, fnum);
//             getNodeManager()->removeNode(mchsource);
            
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
                m_frames[fnum] = dynamic_cast<KisNode*>(frame);
            }
            if (fnum < m_first_frame)
                m_first_frame = fnum;
        }
    }
}

KisNode* SimpleAnimatedLayer::getFrameAt(int num) const
{
    if (num >= dataStart() && num < dataEnd())
        return m_frames[num];
    return 0;
}

KisNode* SimpleAnimatedLayer::getKeyFrame(int num) const
{
    if (isKeyFrame(num))
    {
        return m_frames[num];
    }

    return 0;
}

KisNode* SimpleAnimatedLayer::getCachedFrame(int num) const
{
    KisNode* frame = getFrameAt(num);
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
    return num >= dataStart() && num < dataEnd() && m_frames[num]; // && m_frames[num]->isKeyFrame();
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

void SimpleAnimatedLayer::insertFrame(int num, KisNode* frame, bool iskey)
{
    frame->setName(getNameForFrame(num, iskey));
    getNodeManager()->insertNode(frame, this, num);
}
