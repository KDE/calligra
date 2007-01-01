/****************************************************************************
 ** Copyright (C) 2006 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KD Chart library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KD Chart licenses may use this file in
 ** accordance with the KD Chart Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.kdab.net/kdchart for
 **   information about KDChart Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/

#ifndef KDCHARTABSTRACTGRID_H
#define KDCHARTABSTRACTGRID_H

#include <QPair>

#include "KDChartAbstractCoordinatePlane.h"
#include "KDChartGridAttributes.h"
#include "KDChartAbstractDiagram.h"
#include "KDChartCartesianAxis.h"

namespace KDChart {

    class PaintContext;


    /**
     * \internal
     *
     * \brief Abstract base class for grid classes: cartesian, polar, ...
     *
     * The AbstractGrid interface is the base class used by
     * AbstractCoordinatePlane, for calculating and for drawing
     * the grid lines of the plane.
     */
    class AbstractGrid
    {
    public:
        virtual ~AbstractGrid();
    protected:
        AbstractGrid ();


    public:
        /** \brief Returns the cached result of data calculation.
          *
          * For this, all derived classes need to implement the
          * pure-virtual calculateGrid() method.
          */
        DataDimensionsList updateData( AbstractCoordinatePlane* plane );

        /**
          * Doing the actual drawing.
          *
          * Every derived class must implement this.
          *
          * \note When implementing drawGrid():  Before you start drawing,
          * make sure to call updateData(), to get the data boundaries
          * recalculated.
          * For an example, see the implementation of CartesianGrid:drawGrid().
          */
        virtual void drawGrid( PaintContext* context ) = 0;

        static bool isBoundariesValid(const QRectF& r );
        static bool isBoundariesValid(const QPair<QPointF,QPointF>& b );
        static bool isBoundariesValid(const DataDimensionsList& l );
        static bool isValueValid(const qreal& r );

        GridAttributes gridAttributes;

    protected:
        DataDimensionsList mData;

    private:
        /**
          * \brief Calculates the grid start/end/step width values.
          *
          * Gets the raw data dimensions - e.g. the data model's boundaries,
          * together with their isCalculated flags.
          *
          * Returns the calculated start/end values for the grid, and their
          * respective step widths.
          * If at least one of the step widths is Zero, all dimensions of
          * the returned list are considered invalid!
          *
          * \note This function needs to be implemented by all derived classes,
          * like CartesianGrid, PolarGrid, ...
          */
        virtual DataDimensionsList calculateGrid( const DataDimensionsList& rawDataDimensions )const = 0;
        DataDimensionsList mCachedRawDataDimensions;
    };

}

#endif
