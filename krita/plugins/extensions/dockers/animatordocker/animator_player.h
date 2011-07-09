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


#ifndef ANIMATOR_PLAYER_H
#define ANIMATOR_PLAYER_H

#include <QObject>
#include <QTimer>

#include "animator_model.h"

class AnimatorPlayer : public QObject
{
    Q_OBJECT

public:
    AnimatorPlayer(AnimatorModel* model = 0);
    
    void setModel(AnimatorModel* model);
    
public slots:
    void play(bool ch);
    void setFps(int fps);
    void nextFrame();
    void setLoop(bool loop);
    
public:
    bool getLoop();

protected:
    int getTime();

private:
    AnimatorModel* m_model;
    QTimer* m_timer;
    
    bool m_loop;
    
    int m_fps;
};

#endif // ANIMATOR_PLAYER_H
