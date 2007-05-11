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

#ifndef KDCHARTABSTRACTCARTESIANDIAGRAM_H
#define KDCHARTABSTRACTCARTESIANDIAGRAM_H

#include "KDChartCartesianCoordinatePlane.h"
#include "KDChartAbstractDiagram.h"
#include "KDChartCartesianAxis.h"

namespace KDChart {

    class GridAttributes;
//    class PaintContext;

    /**
     * @brief Base class for diagrams based on a cartesian coordianate system.
     *
     * The AbstractCartesianDiagram interface adds those elements that are
     * specific to diagrams based on a cartesian coordinate system to the
     * basic AbstractDiagram interface.
     */
    class KDCHART_EXPORT AbstractCartesianDiagram : public AbstractDiagram
    {
        Q_OBJECT
        Q_DISABLE_COPY( AbstractCartesianDiagram )
//        KDCHART_DECLARE_PRIVATE_DERIVED( AbstractCartesianDiagram )
        KDCHART_DECLARE_DERIVED_DIAGRAM( AbstractCartesianDiagram, CartesianCoordinatePlane )

    public:
        explicit AbstractCartesianDiagram ( QWidget* parent = 0, CartesianCoordinatePlane* plane = 0 );
        virtual ~AbstractCartesianDiagram();

        virtual const int numberOfAbscissaSegments () const = 0;
        virtual const int numberOfOrdinateSegments () const = 0;
        /**
         * Add the axis to the diagram. The diagram takes ownership of the axis
         * and will delete it.
         *
         * To gain back ownership (e.g. for assigning the axis to another diagram)
         * use the takeAxis method, before calling addAxis on the other diagram.
         *
         * \sa takeAxis
        */
        virtual void addAxis( CartesianAxis * axis );
        /**
         * Removes the axis from the diagram, without deleting it.
         *
         * The diagram no longer owns the axis, so it is
         * the caller's responsibility to delete the axis.
         *
         * \sa addAxis
        */
        virtual void takeAxis( CartesianAxis * axis );
        virtual KDChart::CartesianAxisList axes () const;

        virtual void layoutPlanes();
        virtual void setCoordinatePlane( AbstractCoordinatePlane* plane );

        virtual void setReferenceDiagram( AbstractCartesianDiagram* diagram, const QPointF& offset = QPointF() );
        virtual AbstractCartesianDiagram* referenceDiagram() const;
        virtual QPointF referenceDiagramOffset() const;

    protected:
        virtual double threeDItemDepth( const QModelIndex & index ) const = 0;
        virtual double threeDItemDepth( int column ) const = 0;
    };

}

#endif
