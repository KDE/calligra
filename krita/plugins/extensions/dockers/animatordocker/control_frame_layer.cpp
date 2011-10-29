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

#include "control_frame_layer.h"

#include <sstream>

#include <kis_debug.h>

ControlFrameLayer::ControlFrameLayer(const KisGroupLayer& source): SimpleFrameLayer(source)
{
}

ControlFrameLayer::ControlFrameLayer(KisImageWSP image, const QString& name, quint8 opacity): SimpleFrameLayer(image, name, opacity)
{
}

ControlFrameLayer::~ControlFrameLayer()
{
}

void ControlFrameLayer::setContent(KisNode* c)
{
    SimpleFrameLayer::setContent(c);
    load();
}

bool ControlFrameLayer::isKeyFrame()
{
    return true;
}


void ControlFrameLayer::load()
{
    if (!getContent())
        return;
    if (getContent()->name().startsWith("_loop_"))
    {
        std::stringstream ss;
        ss << getContent()->name().mid(6).toAscii().data();
        int fnum;
        int rnum;
        char t;
        ss >> fnum >> t >> rnum;
        m_target = fnum;
        m_repeatNumber = rnum;
        reset();
    } else
    {
        warnKrita << "only looping is currently supported by control layers";
        warnKrita << "may be you need to update Animator";
    }
}

void ControlFrameLayer::save()
{
    if (!getContent())
        return;
    getContent()->setName("_loop_"+QString::number(target())+"_"+QString::number(repeat()));
}


void ControlFrameLayer::setTarget(int fnum)
{
    m_target = fnum;
    save();
}

int ControlFrameLayer::target()
{
    return m_target;
}


void ControlFrameLayer::setRepeat(int num)
{
    m_repeatNumber = num;
    save();
}

int ControlFrameLayer::repeat()
{
    return m_repeatNumber;
}

int ControlFrameLayer::remaining()
{
    return m_remaining;
}


bool ControlFrameLayer::pass()
{
    if (!enabled())
        return true;
    
    if (m_remaining < 0)
        return false;
    --m_remaining;
    if (!m_remaining)
    {
        reset();
        return true;
    }
    return false;
}

void ControlFrameLayer::reset()
{
    m_remaining = repeat();
}


void ControlFrameLayer::setEnabled(bool val)
{
    m_enabled = val;
}

bool ControlFrameLayer::enabled()
{
    return m_enabled;
}
