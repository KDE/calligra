/*
 *  Frame manager control all actions related to frames
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

#include "animator_frame_manager.h"
#include "animator_switcher.h"
#include <cstdlib>


AnimatorFrameManager::AnimatorFrameManager(AnimatorManager* amanager): QObject(amanager)
{
    m_manager = amanager;
}

AnimatorFrameManager::~AnimatorFrameManager()
{
}

void AnimatorFrameManager::removeRange(int n)
{
    removeRange(m_manager->getSwitcher()->currentFrame(), n);
}

void AnimatorFrameManager::removeRange(int from, int n)
{
    clearRange(from, n);
    moveRange(from+n, -1, -n);
}

void AnimatorFrameManager::insertRange(int n)
{
    insertRange(m_manager->getSwitcher()->currentFrame(), n);
}

void AnimatorFrameManager::insertRange(int from, int n)
{
    moveRange(from, -1, n);
}


void AnimatorFrameManager::clearRange(int n)
{
    clearRange(m_manager->getSwitcher()->currentFrame(), n);
}

void AnimatorFrameManager::clearRange(int from, int n)
{
    if (n == 0)
        return;
    
    QList<AnimatedLayer*> layers = m_manager->layers();
    AnimatedLayer* layer;
    foreach (layer, layers)
    {
        FramedAnimatedLayer* flayer = qobject_cast<FramedAnimatedLayer*>(layer);
        if (flayer)
        {
            clearRange(flayer, from, n);
        }
    }
}

void AnimatorFrameManager::clearRangeActive(int n)
{
    clearRange(qobject_cast<FramedAnimatedLayer*>(m_manager->activeLayer()),
               m_manager->getSwitcher()->currentFrame(),
               n
    );
}

void AnimatorFrameManager::clearRange(FramedAnimatedLayer* layer, int from, int n)
{
    int end;
    if (n < 0)
        end = layer->dataEnd();
    else
        end = from+n;
    for (int i = from; i < end; ++i)
    {
        layer->clearFrame(i);
    }
}


void AnimatorFrameManager::moveRange(int n, int dist)
{
    moveRange(m_manager->getSwitcher()->currentFrame(), n, dist);
}

void AnimatorFrameManager::moveRange(int from, int n, int dist)
{
    if (dist == 0 || n == 0)
        return;
    
    QList<AnimatedLayer*> layers = m_manager->layers();
    AnimatedLayer* layer;
    foreach (layer, layers)
    {
        FramedAnimatedLayer* flayer = qobject_cast<FramedAnimatedLayer*>(layer);
        if (flayer)
        {
            moveRange(flayer, from, n, dist);
        }
    }
}

void AnimatorFrameManager::moveRangeActive(int n, int dist)
{
    int cur = m_manager->getSwitcher()->currentFrame();
    moveRange(qobject_cast<FramedAnimatedLayer*>(m_manager->activeLayer()), cur, n, dist);
    m_manager->getSwitcher()->goFrame(cur+dist);          // for easy moving many times
}

void AnimatorFrameManager::moveRange(FramedAnimatedLayer* layer, int from, int n, int dist)
{
    if (layer == 0 || dist == 0)
        return;
    
    if (n < 0)
        n = layer->dataEnd()-from;
    
    if (n <= 0)
        return;
    
    warnKrita << layer << from << n << dist;
    
    if (dist > 0)
    {
        for (int i = from+n-1; i >= from; --i)
        {
            layer->swapFrames(i, i+dist);
        }
    } else
    {
        for (int i = from; i < from+n; ++i)
        {
            layer->swapFrames(i, i+dist);
        }
    }
}


void AnimatorFrameManager::moveTo(FramedAnimatedLayer* source, int sourceFrame, int targetFrame)
{
    source->moveFrame(sourceFrame, targetFrame);
}

void AnimatorFrameManager::moveTo(FramedAnimatedLayer* source, int sourceFrame, FramedAnimatedLayer* target, int targetFrame)
{
    if (source == target)
    {
        moveTo(source, sourceFrame, targetFrame);
    } else
    {
        warnKrita << "moving frames across layers is not supported yet";
    }
}


