/*
 *  Animated layer class
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

#include <QBrush>

#include "animated_layer.h"

AnimatedLayer::AnimatedLayer(const KisGroupLayer& source): KisGroupLayer(source)
{
}

void AnimatedLayer::init(KisGroupLayer* source)
{
    if (! getNodeManager())
        return;
    
    int i;
    int chcount = source->childCount();
    for (i = 0; i < chcount; ++i)
        getNodeManager()->moveNodeAt(source->at(0), this, i);
}

void AnimatedLayer::setNodeManager(KisNodeManager* nodeman)
{
    m_nodeman = nodeman;
}

KisNodeManager* AnimatedLayer::getNodeManager() const
{
    return m_nodeman;
}

FrameLayer* AnimatedLayer::getNextKeyFrame(int num) const
{
    return getKeyFrame(getNextKey(num));
}

FrameLayer* AnimatedLayer::getPreviousKeyFrame(int num) const
{
    return getKeyFrame(getPreviousKey(num));
}

FrameLayer* AnimatedLayer::getUpdatedFrame(int num)
{
    updateFrame(num);
    return getCachedFrame(num);
}

void AnimatedLayer::updateFrame(int num)
{
    Q_UNUSED(num);
}

QVariant AnimatedLayer::getVision(int role, bool isCurrent)
{
    if (isCurrent && role == Qt::BackgroundRole)
        return QVariant(QBrush(QColor(127, 127, 127)));
    return QVariant();
}
