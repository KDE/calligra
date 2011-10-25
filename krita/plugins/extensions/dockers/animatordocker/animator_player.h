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

#ifndef ANIMATOR_PLAYER_H
#define ANIMATOR_PLAYER_H

#include <QObject>
#include <QTimer>

#include "animator_switcher.h"

class AnimatorPlayer : public QObject
{
    Q_OBJECT

public:
    AnimatorPlayer(AnimatorManager* manager);
    virtual ~AnimatorPlayer();
    
public slots:
    void setFps(int fps);
    void setLooped(bool loop);
    
    void play();
    void pause();
    void stop();
    
public:
    int fps() const;
    bool looped() const;
    
protected slots:
    void tick();
    
protected:
    int nextFrame(int frame);
    
private:
    AnimatorManager* m_manager;
    
    QTimer* m_timer;
    int m_fps;
    
    bool m_playing;
    bool m_paused;
    
    bool m_looped;
};

#endif // ANIMATOR_PLAYER_H
