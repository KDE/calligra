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

#include "animator_player.h"
#include "animator_updater.h"
#include "control_animated_layer.h"

AnimatorPlayer::AnimatorPlayer(AnimatorManager* manager): QObject(manager)
{
    m_manager = manager;
    
    m_timer = new QTimer(this);
    setFps(12);
    connect(m_timer, SIGNAL(timeout()), SLOT(tick()));
}

AnimatorPlayer::~AnimatorPlayer()
{
}


void AnimatorPlayer::setFps(int nfps)
{
    m_fps = nfps;
    m_timer->setInterval(1000/nfps);
}

int AnimatorPlayer::fps() const
{
    return m_fps;
}

void AnimatorPlayer::setLooped(bool loop)
{
    m_looped = loop;
}

bool AnimatorPlayer::looped() const
{
    return m_looped;
}


void AnimatorPlayer::play()
{
    m_timer->start();
    m_manager->getUpdater()->playerModeOn();
    m_playing = true;
    m_paused = false;
}

void AnimatorPlayer::pause()
{
    m_timer->stop();
    m_manager->getUpdater()->playerModeOff();
    m_paused = true;
}

void AnimatorPlayer::stop()
{
    pause();
    // TODO: go to first frame
    m_paused = false;
}


void AnimatorPlayer::tick()
{
    AnimatorSwitcher* sw = m_manager->getSwitcher();
    int nf = nextFrame(sw->currentFrame());
    if (nf < 0)
        pause();
    else
        sw->goFrame(nf);
}

int AnimatorPlayer::nextFrame(int frame)
{
    int nxt = frame + 1;
    
    QList<AnimatedLayer*> layers = m_manager->layers();
    AnimatedLayer* al;
    foreach (al, layers)
    {
        ControlAnimatedLayer* clayer = qobject_cast<ControlAnimatedLayer*>(al);
        if (clayer)
        {
            nxt = clayer->nextFrame(frame);
        }
    }
    
    if (nxt < 0 || nxt >= m_manager->framesNumber())
    {
        if (looped())
            return 0;
        else
            return -1;
    }
    
    return nxt;
}
