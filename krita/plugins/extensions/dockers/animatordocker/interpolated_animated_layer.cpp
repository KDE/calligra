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
}

// void InterpolatedAnimatedLayer::loadFrames()
// {
//     SimpleAnimatedLayer::loadFrames();
//     
// }

void InterpolatedAnimatedLayer::updateFrame(int num)
{
    if (isKeyFrame(num))
    {
        return;
    }
    
    int inxt = getNextKey(num);
    KisNode* next = getKeyFrame(inxt);
    
    int ipre = getPreviousKey(num);
    KisNode* prev = getKeyFrame(ipre);

    if (prev && next && next->inherits("KisCloneLayer"))
    {
        // interpolation here!
        double cur = num;
        double pre = ipre;
        double nxt = inxt;
        double p = (cur-pre) / (nxt-pre);
        
        KisNode* result = dynamic_cast<KisNode*>(interpolate(prev, dynamic_cast<KisCloneLayer*>(next), p));
        
        KisNode* old = getFrameAt(num);
        
        if (result)
        {
            insertFrame(num, result, false);
            
            // Clear previous
            if (old)
            {
//                 std::cout << "removing" << std::endl;
                getNodeManager()->removeNode(old);
            }
            loadFrames();
        }
    }
}

// KisCloneLayer* InterpolatedAnimatedLayer::interpolate(KisNode* from, KisCloneLayer* to, double percent)
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

void InterpolatedAnimatedLayer::insertFrame(int num, KisNode* frame, bool iskey)
{
    SimpleAnimatedLayer::insertFrame(num, frame, iskey);
    if (!iskey)
        m_non_keys.insert(num);
}
