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

#include "kis_rijn_canvas.h"

#include <kis_image.h>
#include <kis_painter.h>
#include <kis_paint_device.h>

KisRijnCanvas::KisRijnCanvas(KisImageWSP _image, KisPaintDeviceSP _paintDevice) : m_image(_image), m_paintDevice(_paintDevice), m_painter(new KisPainter(m_paintDevice))
{

}

KisRijnCanvas::~KisRijnCanvas()
{
    delete m_painter;
}

KisImageWSP KisRijnCanvas::image()
{
    return m_image;
}

KisPainter* KisRijnCanvas::painter()
{
    return m_painter;
}
