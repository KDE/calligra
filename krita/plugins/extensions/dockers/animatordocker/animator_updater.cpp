/*
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

#include "animator_updater.h"

#include <kis_debug.h>

AnimatorUpdater::AnimatorUpdater(AnimatorManager* manager) : QObject(manager)
{
    m_manager = manager;
}

AnimatorUpdater::~AnimatorUpdater()
{
}


void AnimatorUpdater::update(int oldFrame, int newFrame)
{
    QList<AnimatedLayer*> layers = m_manager->layers();
    AnimatedLayer* layer;
    foreach (layer, layers)
    {
        updateLayer(layer, oldFrame, newFrame);
    }
}

void AnimatorUpdater::updateLayer(AnimatedLayer* layer, int oldFrame, int newFrame)
{
    FrameLayer* oldf = layer->getCachedFrame(oldFrame);
    if (oldf)
    {
        oldf->setVisible(0);
        oldf->setDirty(oldf->exactBounds());
    }
    FrameLayer* newf = layer->getUpdatedFrame(newFrame);
    if (newf)
    {
        newf->setVisible(1);
        newf->setOpacity(255);
        newf->setDirty(newf->exactBounds());
    }
}

void AnimatorUpdater::playerModeOn()
{
    m_playerMode = true;
}

void AnimatorUpdater::playerModeOff()
{
    m_playerMode = false;
}
