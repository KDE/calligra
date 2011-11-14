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
#include "kis_image.h"

AnimatedLayer::AnimatedLayer(KisImageWSP image, const QString& name, quint8 opacity): KisGroupLayer(image, name, opacity)
{
    setEnabled(true);
}

AnimatedLayer::AnimatedLayer(const KisGroupLayer& source): KisGroupLayer(source.image(), source.name(), source.opacity())
{
    setEnabled(true);
}

FrameLayer* AnimatedLayer::getUpdatedFrame(int num)
{
    if (enabled())
        updateFrame(num);
    return getCachedFrame(num);
}

void AnimatedLayer::updateAllFrames()
{
    for (int i = dataStart(); i < dataEnd(); ++i) {
        updateFrame(i);
    }
}

void AnimatedLayer::updateFrame(int num)
{
    Q_UNUSED(num);
}

bool AnimatedLayer::enabled()
{
    return m_enabled;
}

void AnimatedLayer::setEnabled(bool val)
{
    m_enabled = val;
}
