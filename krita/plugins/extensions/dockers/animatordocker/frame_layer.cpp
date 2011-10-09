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
        r = 96;
        g = 96;
        b = 96;
        if (isCurrent)
        {
            r -= 64;
            g -= 64;
            b -= 64;
        }
        if (! isKeyFrame())
        {
            r += 16;
            g += 16;
            b += 16;
        }
        
        if (! getContent())
            // it is strange..
            return QVariant();
        
        int addToCh = 64 + (isKeyFrame()?0:-48);
        
        if (getContent()->inherits("KisPaintLayer"))
        {
            g += addToCh;
        } else if (getContent()->inherits("KisCloneLayer"))
        {
            b += addToCh;
        } else {
            r += addToCh;
        }
        
        return QVariant(QBrush(QColor(r, g, b)));
    }
    return QVariant();
}

KisNode* FrameLayer::getContent()
{
    if (at(0) && !at(0)->name().compare("_"))
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
        c->setName("_");
        if (c->parent())
            getNodeManager()->moveNodeAt(c, this, 0);
        else
            getNodeManager()->insertNode(c, this, 0);
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

bool FrameLayer::isKeyFrame()
{
    return ! name().endsWith("_");
}
