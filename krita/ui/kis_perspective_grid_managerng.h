/*
 * This file is part of Krita
 *
 *  Copyright (c) 2014 Shivaraman Aiyer<sra392@gmail.com>
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
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef KIS_PERSPECTIVE_GRIDNG_MANAGER_H
#define KIS_PERSPECTIVE_GRIDNG_MANAGER_H

#include <QPointF>

#include "canvas/kis_canvas_decoration.h"
#include "kis_perspective_gridng.h"

#include <krita_export.h>

class KActionCollection;

/**
 * This class hold a list of painting assistants.
 */
class KRITAUI_EXPORT KisPerspectiveGridNgManager: public KisCanvasDecoration
{
public:
    KisPerspectiveGridNgManager(KisView2* parent);
    ~KisPerspectiveGridNgManager();
    void addAssistant(KisPerspectiveGridNg* assistant);
    void removeAssistant(KisPerspectiveGridNg* assistant);
    void removeAll();
    QPointF adjustPosition(const QPointF& point, const QPointF& strokeBegin);
    void endStroke();
    void setup(KActionCollection * collection);
    QList<KisPerspectiveGridNgHandleSP> handles();
    QList<KisPerspectiveGridNg*> assistants();
protected:
    void drawDecoration(QPainter& gc, const QRectF& updateRect, const KisCoordinatesConverter *converter,KisCanvas2* canvas);

private:
    struct Private;
    Private* const d;
};

#endif
