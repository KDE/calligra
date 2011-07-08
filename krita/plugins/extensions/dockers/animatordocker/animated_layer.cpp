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


#include "animated_layer.h"

AnimatedLayer::AnimatedLayer(const KisGroupLayer& source): KisGroupLayer(source)
{
//     m_source = const_cast<KisGroupLayer*>( &source );
    
    connect(this, SIGNAL(requireRedraw()), this, SLOT(frameUpdate()));
}

// bool AnimatedLayer::needProjection() const
// {
//     bool t = m_frame_changed;
//     const_cast<AnimatedLayer*>(this)->frameChange(false);
//     return KisLayer::needProjection() || t;
// }
// 
// void AnimatedLayer::frameUpdate()
// {
//     frameChange(true);
// }

int AnimatedLayer::getFrameNumber()
{
    return m_frame;
}

void AnimatedLayer::setFrameNumber(int num)
{
    m_frame = num;
    emit requireRedraw();
}

bool AnimatedLayer::isValid()
{
    return m_valid;
}

void AnimatedLayer::setValid(bool valid)
{
    m_valid = valid;
}

// void AnimatedLayer::frameChange(bool ch)
// {
//     m_frame_changed = ch;
// }

// KisNode* AnimatedLayer::source()
// {
//     return m_source;
// }
