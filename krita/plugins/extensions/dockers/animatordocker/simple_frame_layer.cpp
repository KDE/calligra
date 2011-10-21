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

#include "simple_frame_layer.h"

SimpleFrameLayer::SimpleFrameLayer(KisImageWSP image, const QString& name, quint8 opacity): FrameLayer(image, name, opacity)
{
}

SimpleFrameLayer::SimpleFrameLayer(const KisGroupLayer& source): FrameLayer(source)
{
}

KisNode* SimpleFrameLayer::getContent()
{
    if (at(0) && at(0)->name().startsWith("_"))
    {
        return at(0).data();
    }
    return 0;
}

bool SimpleFrameLayer::isKeyFrame()
{
    return ! name().endsWith("_");
}
