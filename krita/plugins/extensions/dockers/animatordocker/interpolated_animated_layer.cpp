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
    m_keys.clear();
    m_non_keys.clear();
}

void InterpolatedAnimatedLayer::update()
{
    SimpleAnimatedLayer::update();
//     clearJunk();
    
    m_cached_frame = 0;
    m_to_cache = 0;
}

// void InterpolatedAnimatedLayer::frameUpdate()
// {
//     SimpleAnimatedLayer::frameUpdate();
// }

KisNode* InterpolatedAnimatedLayer::getKeyFrameLayer(int num)
{
    if (!m_non_keys.contains(num)/* || result && !result->inherits("KisCloneLayer")*/)
        return SimpleAnimatedLayer::getKeyFrameLayer(num);
    return 0;
}

int InterpolatedAnimatedLayer::getSCurrentKey(int num)
{
    // this is SimpleAnimatedLayer::getCurrentKey actually
    if (getFrameAt(num))
        return num;
    return SimpleAnimatedLayer::getPreviousKey(num);
}

KisNode* InterpolatedAnimatedLayer::getFrameLayer(int num)
{
    if (m_gfl_works)
    {
        std::cout << "This is probably a bug... (ial~63)" << std::endl;
        return SimpleAnimatedLayer::getFrameLayer(num);
    }
    
    m_gfl_works = true;
    
    KisNode* current;
    if (current = getKeyFrameLayer(num))
    {
        m_gfl_works = false;
        return current;
    }
    
    current = SimpleAnimatedLayer::getFrameLayer(num);
    if (!current)
    {
        m_gfl_works = false;
        return 0;
    }
    
    int inxt = getNextKey(num);
    KisNode* next = getFrameAt(inxt);
    while (next && next->inherits("KisCloneLayer") && m_non_keys.contains(inxt))
    {
        inxt = getNextKey(inxt);
        next = getFrameAt(inxt);
    }
    
    int ipre = getSCurrentKey(num);
    current = getFrameAt(ipre);
    while (current && current->inherits("KisCloneLayer") && m_non_keys.contains(ipre))
    {
        ipre = getPreviousKey(ipre);
        current = getFrameAt(ipre);
    }

    if (current && next && next->inherits("KisCloneLayer"))
    {
        // interpolation here!
        double cur = num;
        double pre = ipre;
        double nxt = inxt;
        double p = (cur-pre) / (nxt-pre);
        
        KisNode* to_delete = getFrameAt(num);
        
        KisCloneLayer* result = interpolate(current, dynamic_cast<KisCloneLayer*>(next), p);
        
        result->setName("_frame_"+QString::number(num));
        m_non_keys.insert(num);
        
        current = dynamic_cast<KisNode*>(result);
        getNodeManager()->insertNode(current, this, 0);
        
        if (to_delete)
            getNodeManager()->removeNode(to_delete);
        
        update();
        
        if (m_to_cache == num)
        {
            m_cached_frame = dynamic_cast<KisCloneLayer*>( getFrameAt(num) );
        }
    }
    
    m_gfl_works = false;
    
    return current;
}

KisCloneLayer* InterpolatedAnimatedLayer::interpolate(KisNode* from, KisCloneLayer* to, double percent)
{
    // Position
    double x = from->x()*(1.0-percent)+to->x()*percent;
    double y = from->y()*(1.0-percent)+to->y()*percent;
    qint32 ix = x;
    qint32 iy = y;
    
    KisCloneLayer* result = new KisCloneLayer(*to);
    result->setX(ix);
    result->setY(iy);
    result->setVisible(false);
    
    return result;
}

KisNode* InterpolatedAnimatedLayer::setFrameNumber(int num)
{
    m_to_cache = num;
    return AnimatedLayer::setFrameNumber(num);
}

KisNode* InterpolatedAnimatedLayer::getCachedFrame()
{
    return m_cached_frame;
}
