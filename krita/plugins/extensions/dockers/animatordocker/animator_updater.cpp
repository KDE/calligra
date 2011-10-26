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
    playerModeOff();
}

AnimatorUpdater::~AnimatorUpdater()
{
}

void AnimatorUpdater::fullUpdate()
{
    QList<AnimatedLayer*> layers = m_manager->layers();
    AnimatedLayer* layer;
    foreach (layer, layers)
    {
        fullUpdateLayer(layer);
    }
}

void AnimatorUpdater::fullUpdateLayer(AnimatedLayer* layer)
{
    for (int i = layer->dataStart(); i < layer->dataEnd(); ++i)
    {
        FrameLayer* f = layer->getCachedFrame(i);
        if (f && f->visible())
        {
            f->setVisible(0);
            f->setDirty(f->exactBounds());
        }
    }
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
    FrameLayer* newf;
    if (m_playerMode)
        newf = layer->getCachedFrame(newFrame);
    else
        newf = layer->getUpdatedFrame(newFrame);
    
    if (oldf == newf && (!oldf || oldf->visible() && oldf->opacity() == 255))
        return;
    
    if (oldf && oldf->visible())
    {
        oldf->setVisible(false);
        oldf->setDirty(oldf->exactBounds());
    }
    if (newf && (!newf->visible() || newf->opacity() != 255))
    {
        newf->setVisible(true);
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
