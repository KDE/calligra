/*
 *  Copyright (c) 2015 Boudewijn Rempt <boud@valdyas.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#ifndef STACKED_BRUSH_H_
#define STACKED_BRUSH_H_

#include <QVector>

#include <KoColor.h>

#include "kis_paint_device.h"

class StackedBrush
{

public:
    StackedBrush();
    ~StackedBrush();
    void paint(KisPaintDeviceSP dev, qreal x, qreal y, const KoColor &color);

};

#endif
