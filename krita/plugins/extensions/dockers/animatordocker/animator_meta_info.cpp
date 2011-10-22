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

#include "animator_meta_info.h"

#include <strstream>

AnimatorMetaInfo::AnimatorMetaInfo(KisGroupLayerSP meta_node)
{
    if (! meta_node)
    {
        m_major = -1;
        return;
    }
    
    QString t = meta_node->name();
    
    if (! t.startsWith("_animator_"))
    {
        m_major = -1;
        return;
    }
    
    std::strstream ns;
    ns << t.mid(10).toAscii().data();
    ns >> m_major;
    
    ns << std::endl << t.mid(10).toAscii().data();
    int m;
    char tmp;
    ns >> m >> tmp >> m_minor;             // major_minor (e.x. 2_5)
}

AnimatorMetaInfo::AnimatorMetaInfo(int vmajor, int vminor)
{
    m_major = vmajor;
    m_minor = vminor;
}

int AnimatorMetaInfo::getMajor()
{
    return m_major;
}

int AnimatorMetaInfo::getMinor()
{
    return m_minor;
}
