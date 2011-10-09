/*
 *  Player
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

#include <iostream>

AnimatorPlayer::AnimatorPlayer(AnimatorModel* model)
{
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(nextFrame()));
    setFps(1);
    setLoop(false);
    setModel(model);
}

void AnimatorPlayer::setModel(AnimatorModel* model)
{
    m_model = model;
}

void AnimatorPlayer::setFps(int fps)
{
    if (fps > 0)
        m_fps = fps;
}

int AnimatorPlayer::getTime()
{
    return 1000/m_fps;
}

void AnimatorPlayer::play(bool ch)
{
    if (ch)
    {
        m_model->enableAllLayers(false);
        m_model->disableOnion();
        m_timer->start(getTime());
//         m_model->blockSignals(true);
    } else
    {
        m_timer->stop();
        m_model->enableOnion();
        m_model->enableAllLayers(true);
//         m_model->blockSignals(false);
    }
}

void AnimatorPlayer::nextFrame()
{
//     std::cout << "next frame" << std::endl;
    if (m_loop && m_model->isLast())
        m_model->goFirst();
    else
        m_model->goNext();
//     m_model->updateCanvas();
}

bool AnimatorPlayer::getLoop()
{
    return m_loop;
}

void AnimatorPlayer::setLoop(bool loop)
{
    m_loop = loop;
}
