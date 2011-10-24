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

#ifndef SIMPLE_FRAME_LAYER_H
#define SIMPLE_FRAME_LAYER_H

#include "frame_layer.h"

class SimpleFrameLayer : public FrameLayer
{
    Q_OBJECT
    
public:
//     SimpleFrameLayer(KisImageWSP image, const QString& name, quint8 opacity);
    SimpleFrameLayer(const KisGroupLayer& source);
    
public:
    virtual KisNode* getContent();
//     virtual void setContent(KisNode* c);
    
public:
    virtual bool isKeyFrame();
};

#endif // SIMPLE_FRAME_LAYER_H
