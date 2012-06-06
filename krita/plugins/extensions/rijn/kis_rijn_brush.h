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

#include <OpenRijn/AbstractBrush.h>
#include <kis_types.h>

class KisPainter;
class KisRijnBrush : public OpenRijn::AbstractBrush
{
public:
    KisRijnBrush(KisPaintOpPresetSP _paintOp);
    virtual float drawPoint(OpenRijn::AbstractCanvas* _canvas, const OpenRijn::DrawingPoint& _point );
    virtual OpenRijn::DistanceInformation drawCurve(OpenRijn::AbstractCanvas* canvas, const OpenRijn::DrawingPoint& pt1, float x1, float y1, float x2, float y2, const OpenRijn::DrawingPoint& pt2, const OpenRijn::DistanceInformation& information);
    virtual OpenRijn::DistanceInformation drawLine(OpenRijn::AbstractCanvas* canvas, const OpenRijn::DrawingPoint& pt1, const OpenRijn::DrawingPoint& pt2, const OpenRijn::DistanceInformation& information);
private:
    KisPainter* setupPainter(OpenRijn::AbstractCanvas* _canvas);
private:
    KisPaintOpPresetSP m_paintOp;
};
