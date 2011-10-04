/*
 *  Interpolated layer: allows several ways of interpolating
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

#include <iostream>

#include "interpolated_animated_layer.h"

InterpolatedAnimatedLayer::InterpolatedAnimatedLayer(const KisGroupLayer& source): SimpleAnimatedLayer(source)
{
    m_non_keys.clear();
    m_updating = false;
}

// void InterpolatedAnimatedLayer::loadFrames()
// {
//     SimpleAnimatedLayer::loadFrames();
//     
// }

void InterpolatedAnimatedLayer::updateFrame(int num)
{
    if (m_updating)
        return;
    
    if (isKeyFrame(num))
    {
        return;
    }
    m_updating = true;
    
    int inxt = getNextKey(num);
    KisCloneLayer* next = 0;
    if (isKeyFrame(inxt))
        next = dynamic_cast<KisCloneLayer*>( getKeyFrame(inxt)->getContent() );
    
    int ipre = getPreviousKey(num);
    KisNode* prev = 0;
    if (isKeyFrame(ipre))
        prev = getKeyFrame(ipre)->getContent();

    if (prev && next && next->inherits("KisCloneLayer"))
    {
        // interpolation here!
        double cur = num;
        double pre = ipre;
        double nxt = inxt;
        double p = (cur-pre) / (nxt-pre);
        
        getNodeManager()->activateNode(this);
        getNodeManager()->createNode("KisGroupLayer");
        
        KisNode* target = getNodeManager()->activeNode().data();
        getNodeManager()->insertNode(interpolate(prev, next, p), target, 0);
        
        target->setName(getNameForFrame(num, false));
        target->at(0)->setName("_");
        
        m_updating = false;
//         loadFrames();
        // This is a hack to call loadFrames() without crashing
        // TODO: find the reason for crashing & fix it
        getNodeManager()->activateNode(this);
        getNodeManager()->createNode("KisPaintLayer");
        getNodeManager()->removeNode(getNodeManager()->activeNode());
        m_updating = true;      // not required, but for ethtetic reasons..
        
//         FrameLayer* old = getFrameAt(num);
        
//         if (result)
//         {
//             insertFrame(num, result, false);
//             
//             // Clear previous
//             if (old)
//             {
// //                 std::cout << "removing" << std::endl;
//                 getNodeManager()->removeNode(old);
//             }
//             loadFrames();
//         }
    }
    m_updating = false;
}

// KisCloneLayer* InterpolatedAnimatedLayer::interpolate(FrameLayer* from, KisCloneLayer* to, double percent)
// {
//     // Position
//     double x = from->x()*(1.0-percent)+to->x()*percent;
//     double y = from->y()*(1.0-percent)+to->y()*percent;
//     qint32 ix = x;
//     qint32 iy = y;
//     
//     KisCloneLayer* result = new KisCloneLayer(*to);
//     result->setX(ix);
//     result->setY(iy);
//     result->setVisible(false);
//     
//     return result;
// }

const QString& InterpolatedAnimatedLayer::getNameForFrame(int num, bool iskey) const
{
    QString *t = const_cast<QString*>( &SimpleAnimatedLayer::getNameForFrame(num, iskey) );
    if (iskey)
        return *t;
    else
        return * ( new QString (*t+QString("_")) );
}

int InterpolatedAnimatedLayer::getFrameFromName(const QString& name, bool& iskey) const
{
    int result = SimpleAnimatedLayer::getFrameFromName(name, iskey);
    iskey = !name.endsWith("_");
    return result;
}

bool InterpolatedAnimatedLayer::isKeyFrame(int num) const
{
    return SimpleAnimatedLayer::isKeyFrame(num) && !m_non_keys.contains(num);
}

void InterpolatedAnimatedLayer::insertFrame(int num, FrameLayer* frame, bool iskey)
{
    SimpleAnimatedLayer::insertFrame(num, frame, iskey);
    if (!iskey)
        m_non_keys.insert(num);
}
