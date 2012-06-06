/*
 * Copyright (c) 2012 Cyrille Berger <cberger@cberger.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <kis_simple_stroke_strategy.h>

#include <GTLCore/Color.h>
#include <OpenRijn/DrawingPoint.h>
#include <OpenRijn/Source.h>
#include <OpenRijn/Sketch.h>

#include <kis_image.h>

class KisTransaction;

class KisRijnStroke : public KisSimpleStrokeStrategy
{
public:
    KisRijnStroke(KisImageWSP _image, KisNodeSP _node, const OpenRijn::Source& _source);
    virtual void initStrokeCallback();
    virtual void finishStrokeCallback();
    void doStrokeCallback(KisStrokeJobData *data);
private:
    KisImageWSP      m_image;
    KisNodeSP        m_node;
    OpenRijn::Source m_source;
    OpenRijn::Sketch m_sketch;
    KisTransaction*  m_transaction;
};
