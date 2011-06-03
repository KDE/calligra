/*
 *  Light table model
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


#include "animator_light_table.h"

#include <cstdlib>
#include <iostream>

AnimatorLightTable::AnimatorLightTable()
{
    setNear(0);
}

AnimatorLightTable::~AnimatorLightTable()
{

}

double AnimatorLightTable::getOpacity(int rel_frame)
{
    update();
    if (std::abs(rel_frame) > m_near)
        return 0.0;
    return m_opacity[std::abs(rel_frame)*2+(rel_frame < 0)];
}

bool AnimatorLightTable::getVisibility(int rel_frame)
{
    update();
    if (std::abs(rel_frame) > m_near)
        return false;
    return m_vis[std::abs(rel_frame)*2+(rel_frame < 0)];
}

void AnimatorLightTable::setOpacity(int rel_frame, double op)
{
//     std::cout << op << std::endl;
    m_opacity[std::abs(rel_frame)*2+(rel_frame < 0)] = op;
    emit opacityChanged();
}

void AnimatorLightTable::setOpacity(int rel_frame, int op)
{
    double dop = op / 1000.0;
    setOpacity(rel_frame, dop);
}

void AnimatorLightTable::setVisibility(int rel_frame, bool ch)
{
    m_vis[std::abs(rel_frame)*2+(rel_frame < 0)] = ch;
    emit visibilityChanged();
}

void AnimatorLightTable::setNear(int near)
{
//     std::cout << "HHHHHHH" << std::endl;
    if (near < 0)
        return;
    if (near == m_near)
        return;
    for (int i = 0; i <= near - m_near; ++i)
    {
        m_opacity.append(0.0);
        m_opacity.append(0.0);
        m_vis.append(false);
        m_vis.append(false);
    }
    m_near = near;
    emit nearChanged(near);
}

int AnimatorLightTable::getNear()
{
    return m_near;
}

void AnimatorLightTable::update()
{
    if (!m_manually)
    {
        // 
    }
}
