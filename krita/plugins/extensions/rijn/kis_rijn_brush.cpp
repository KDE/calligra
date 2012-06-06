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

#include "kis_rijn_brush.h"
#include "kis_rijn_canvas.h"

#include <OpenRijn/DrawingPoint.h>
#include <OpenRijn/DistanceInformation.h>
#include <GTLCore/Color.h>

#include <kis_image.h>
#include <kis_painter.h>
#include <kis_paint_information.h>
#include <kis_paintop_preset.h>
#include <KoColor.h>
#include <KoColorSpaceRegistry.h>

KisRijnBrush::KisRijnBrush(KisPaintOpPresetSP _paintOp) : m_paintOp(_paintOp)
{

}

namespace {
    inline KisPaintInformation odp2kpi(const OpenRijn::DrawingPoint& _point)
    {
        return KisPaintInformation(QPointF(_point.x, _point.y), _point.t, 0.0, 0.0, nullKisVector2D(), _point.angle);
    }
    inline KisDistanceInformation odi2kdi(const OpenRijn::DistanceInformation& _information)
    {
        return KisDistanceInformation(_information.distance, _information.spacing);
    }
    inline OpenRijn::DistanceInformation kdi2odi(const KisDistanceInformation& _information)
    {
        return OpenRijn::DistanceInformation(_information.distance, _information.spacing);
    }
}

float KisRijnBrush::drawPoint(OpenRijn::AbstractCanvas* _canvas, const OpenRijn::DrawingPoint& _point)
{
    return setupPainter(_canvas)->paintAt(odp2kpi(_point));
}

OpenRijn::DistanceInformation KisRijnBrush::drawLine(OpenRijn::AbstractCanvas* _canvas, const OpenRijn::DrawingPoint& _pt1, const OpenRijn::DrawingPoint& _pt2, const OpenRijn::DistanceInformation& _information)
{
    return kdi2odi(setupPainter(_canvas)->paintLine(odp2kpi(_pt1), odp2kpi(_pt2), odi2kdi(_information)));
}

OpenRijn::DistanceInformation KisRijnBrush::drawCurve(OpenRijn::AbstractCanvas* _canvas, const OpenRijn::DrawingPoint& _pt1, float _x1, float _y1, float _x2, float _y2, const OpenRijn::DrawingPoint& _pt2, const OpenRijn::DistanceInformation& _information)
{
    return kdi2odi(setupPainter(_canvas)->paintBezierCurve(odp2kpi(_pt1), QPointF(_x1, _y1), QPointF(_x2, _y2), odp2kpi(_pt2), odi2kdi(_information)));
}

KisPainter* KisRijnBrush::setupPainter(OpenRijn::AbstractCanvas* _canvas)
{
    KisRijnCanvas* canvas  = static_cast<KisRijnCanvas*>(_canvas);
    KisPainter*    painter = canvas->painter();
    painter->setPaintOpPreset(m_paintOp, canvas->image());
    painter->setPaintColor(KoColor(QColor(paintColor().red() * 255, paintColor().green() * 255, paintColor().blue() * 255, paintColor().alpha() * 255), KoColorSpaceRegistry::instance()->rgb8()));
    return painter;
}
