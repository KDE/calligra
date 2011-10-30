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

#include "view_animated_layer.h"

#include <sstream>

ViewAnimatedLayer::ViewAnimatedLayer(const KisGroupLayer& source): AnimatedLayer(source)
{
    m_content = 0;
    m_cached = 0;
}

ViewAnimatedLayer::ViewAnimatedLayer(KisImageWSP image, const QString& name, quint8 opacity): AnimatedLayer(image, name, opacity)
{
    m_content = 0;
    m_cached = 0;
}

ViewAnimatedLayer::~ViewAnimatedLayer()
{
}


bool ViewAnimatedLayer::displayable() const
{
    return false;
}

bool ViewAnimatedLayer::hasPreview() const
{
    return true;
}

QString ViewAnimatedLayer::aName() const
{
    return name().mid(9);
}

void ViewAnimatedLayer::setAName(const QString& name)
{
    setName("_aniview_"+name);
}


void ViewAnimatedLayer::load()
{
    m_content = at(0).data();
    if (!m_content || !m_content->name().startsWith("_"))
        return;
    
    std::stringstream ss;
    ss << m_content->name().mid(1).toAscii().data();
    char t;
    ss >> m_start >> t >> m_end;
    
    warnKrita << m_start << m_end;
}

void ViewAnimatedLayer::save()
{
    if (!m_content)
        return;
    m_content->setName("_"+QString::number(m_start)+"_"+QString::number(m_end));
}

int ViewAnimatedLayer::dataStart() const
{
    return m_start;
}

int ViewAnimatedLayer::dataEnd() const
{
    return m_end;
}


QImage ViewAnimatedLayer::getThumbnail(int fnum, int size)
{
    if (fnum < dataStart() || fnum >= dataEnd())
    {
        return QImage();
    }
    if (m_fullImage.isNull() || m_cached != size)
    {
        m_fullImage = createThumbnail(size*(dataEnd()-dataStart()), size);
        if (m_fullImage.isNull())
        {
            return QImage();
        }
        m_cached = size;
    }
    QImage thumb = m_fullImage.copy((fnum-dataStart())*size, 0, size, size);
    return thumb;
}


// Not supported yet
FrameLayer* ViewAnimatedLayer::getCachedFrame(int num) const
{
    return 0;
}
