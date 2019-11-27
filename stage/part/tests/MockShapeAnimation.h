/* This file is part of the KDE project
 * Copyright ( C ) 2012 Paul Mendez <paulestebanms@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef MOCKSHAPEANIMATION_H
#define MOCKSHAPEANIMATION_H

#include <animations/KPrShapeAnimation.h>

class MockShapeAnimation : public KPrShapeAnimation
{
public:
     MockShapeAnimation(KoShape *shape, QTextBlockUserData *textBlockUserData)
        : KPrShapeAnimation(shape, textBlockUserData)
        , m_beginTime(0)
        , m_duration(1)
    {}

    /**
      * Return the begin and end time of the animation as a QPair
      */
    QPair<int, int> timeRange() const override {return QPair<int, int>(m_beginTime, m_beginTime + m_duration);}
    /**
      * Return global duration of the shape animation
      */
    int globalDuration() const override {return m_duration;}

    /**
     * @brief Set the begin time for the animation
     *
     * @param timeMS time in milliseconds
     */
    void setBeginTime(int timeMS) override {m_beginTime = timeMS;}

    /**
     * @brief Set duration time for the animation
     *
     * @param timeMS time in milliseconds
     */
    void setGlobalDuration(int timeMS) override {m_duration = timeMS;}

private:
    int m_beginTime;
    int m_duration;

};

#endif // MOCKSHAPEANIMATION_H
