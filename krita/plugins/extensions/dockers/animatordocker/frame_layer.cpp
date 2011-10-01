/*
 *  Frame layer class
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


#include "frame_layer.h"
#include <kis_node_manager.h>
#include "kis_image.h"

FrameLayer::FrameLayer(KisImageWSP image, const QString& name, quint8 opacity): KisGroupLayer(image, name, opacity)
{
}

FrameLayer::FrameLayer(const KisGroupLayer& source): KisGroupLayer(source)
{
    for (int i = 0; i < childCount(); ++i)
    {
        at(i)->setName(at(i)->name().mid(14, at(i)->name().size()-15));
    }
}

QVariant FrameLayer::getVision(int role, bool isCurrent)
{
    if (role == Qt::BackgroundRole)
    {
        int r, g, b;
        r = 127;
        g = 127;
        b = 127;
        if (isCurrent)
        {
            r -= 64;
            g -= 64;
            b -= 64;
        }
        if (!getContent())
        {
            // it is strange..
            return QVariant();
        }
        if (getContent()->inherits("KisPaintLayer"))
        {
            g += 64;
        } else if (getContent()->inherits("KisCloneLayer"))
        {
            b += 64;
        } else {
            r += 64;
        }
        return QVariant(QBrush(QColor(r, g, b)));
    }
    return QVariant();
}

KisNode* FrameLayer::getContent()
{
    if (!at(0)->name().compare("_"))
    {
        return at(0).data();
    }
    return 0;
}

void FrameLayer::setContent(KisNode* c)
{
    if (getNodeManager())
    {
        if (getContent())
            getNodeManager()->removeNode(getContent());
        getNodeManager()->insertNode(c, this, 0);
        c->setName("_");
    }
}

void FrameLayer::setNodeManager(KisNodeManager* nm)
{
    m_node_manager = nm;
}

KisNodeManager* FrameLayer::getNodeManager()
{
    return m_node_manager;
}
