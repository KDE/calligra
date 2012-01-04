/*
 *  Copyright (c) 2007 Sven Langkamp <sven.langkamp@gmail.com>
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
#ifndef KIS_SELECTION_COMPONENT_H
#define KIS_SELECTION_COMPONENT_H

#include <krita_export.h>

class QRect;
class QTransform;
class KUndo2Command;
class KisSelection;
class KisPixelSelection;

#include "kis_types.h"

class KRITAIMAGE_EXPORT KisSelectionComponent
{
public:
    KisSelectionComponent() {}
    virtual ~KisSelectionComponent() {}

    virtual KisSelectionComponent* clone(KisSelection* selection) = 0;

    virtual void renderToProjection(KisPaintDeviceSP projection) = 0;
    virtual void renderToProjection(KisPaintDeviceSP projection, const QRect& r) = 0;

    virtual void moveX(qint32 x) { Q_UNUSED(x); }
    virtual void moveY(qint32 y) { Q_UNUSED(y); }

    virtual KUndo2Command* transform(const QTransform &transform) {
        Q_UNUSED(transform);
        return 0;
    }

    /// api used for now mailin in KisPixelSelection

    /**
     * Fill the specified rect with the specified selectedness.
     */
    virtual void select(const QRect & r, quint8 selectedness = MAX_SELECTED) { Q_UNUSED(r); Q_UNUSED(selectedness); }


    /**
     * Invert the total selection. This will also invert the default value
     * of the selection paint device, from MIN_SELECTED to MAX_SELECTED or
     * back.
     */
    virtual void invert() {}

    /**
     * Apply a selection to the selection using the specified selection mode
     */
    virtual void applySelection(KisPixelSelectionSP selection, SelectionAction action) { Q_UNUSED(selection); Q_UNUSED(action); }

    /// Tests if the the rect is totally outside the selection
    virtual bool isTotallyUnselected(const QRect & r) const { Q_UNUSED(r); }

    /**
     * Rough, but fastish way of determining the area
     * of the tiles used by the selection.
     */
    virtual QRect selectedRect() const {}

    /**
     * Slow, but exact way of determining the rectangle
     * that encloses the selection.
     */
    virtual QRect selectedExactRect() const {}

    /**
     * @brief outline returns the outline of the current selection
     * @return a vector of polygons that can be used to draw the outline
     */
    virtual QVector<QPolygon> outline() const {}


};

#endif
