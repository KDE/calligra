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


#ifndef FRAME_LAYER_H
#define FRAME_LAYER_H

#include <QObject>

#include <kis_group_layer.h>
#include <kis_node_manager.h>
#include <kis_image.h>

class FrameLayer : public KisGroupLayer
{
    Q_OBJECT
    
    
public:
    FrameLayer(KisImageWSP image, const QString& name, quint8 opacity);
    FrameLayer(const KisGroupLayer& source);
    
//     void setNodeManager(KisNodeManager* nm);
//     KisNodeManager* getNodeManager();
    
private:
//     KisNodeManager* m_node_manager;
};

#endif // FRAME_LAYER_H
