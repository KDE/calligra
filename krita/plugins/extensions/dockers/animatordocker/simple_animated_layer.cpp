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

SimpleAnimatedLayer::SimpleAnimatedLayer(const KisGroupLayer& source) : AnimatedLayer(source)
{
//     if (!source)
//     {
//         setValid(false);
//         return;
//     }
    
    m_frames.clear();
    
    update();
    
    setValid(true);
}

void SimpleAnimatedLayer::update()
{
    for ( qint32 i = 0; i < childCount(); ++i )
    {
        const KisNode* chsource = childNodes(QStringList(),  KoProperties())[i];
        if (chsource->name().startsWith("_frame_"))
        {
            KisNode* mchsource = const_cast<KisNode*>(chsource);
            QString ts = chsource->name();
            
            qint32 fnum = ts.mid(7).toLong();
            
//                 if (fnum+1 > columnCount()) {
//                     setFramesNumber(fnum+1, false);
//                 }
            
            if (fnum == m_frames.size())
            {
                m_frames.append(mchsource);
            } else {
                if (fnum > m_frames.size())
                {
                    for (qint32 fc = fnum-m_frames.size()+1; fc; --fc)
                    {
                        m_frames.append(0);
                    }
                }
                m_frames[fnum] = mchsource;
            }
        }
    }
}

void SimpleAnimatedLayer::frameUpdate()
{
    visibleAll(false);
    KisNode* node = getFrameLayer(getFrameNumber());
    if (node)
    {
        node->setVisible(true);
        node->setOpacity(255);
//         node->
    }
//     AnimatedLayer::frameUpdate();
}

KisNode* SimpleAnimatedLayer::getFrameLayer(int num)
{
    KisNode* r = getKeyFrameLayer(num);
    if (!r)
        r = getPreviousKeyFrame(num);
    return r;
}

KisNode* SimpleAnimatedLayer::getKeyFrameLayer(int num)
{
    if (num >= 0 && num < m_frames.size())
        return m_frames[num];
    return 0;
}

KisNode* SimpleAnimatedLayer::getNextKeyFrame(int num)
{
    return 0;
}

KisNode* SimpleAnimatedLayer::getPreviousKeyFrame(int num)
{
    if (num < 0)
        num = 0;
    if (num >= m_frames.size())
        num = m_frames.size();
    while (--num >= 0)
    {
        if (m_frames[num])
            return m_frames[num];
    }
    return 0;
}

void SimpleAnimatedLayer::visibleAll(bool v)
{
    foreach (const KisNode* cn, m_frames)
    {
        KisNode* node = const_cast<KisNode*>( cn );
        if (node)
            node->setVisible(v);
    }
}
