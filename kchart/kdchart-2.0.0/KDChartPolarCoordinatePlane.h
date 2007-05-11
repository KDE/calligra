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

#ifndef KDCHART_POLAR_COORDINATEPLANE_H
#define KDCHART_POLAR_COORDINATEPLANE_H

#include "KDChartAbstractCoordinatePlane.h"

namespace KDChart {

    class Chart;
    class PaintContext;

    class KDCHART_EXPORT PolarCoordinatePlane : public AbstractCoordinatePlane
    {
        Q_OBJECT

        Q_DISABLE_COPY( PolarCoordinatePlane )
        KDCHART_DECLARE_PRIVATE_DERIVED_PARENT( PolarCoordinatePlane, Chart* )

    public:
        struct CoordinateTransformation;
        typedef QList<CoordinateTransformation> CoordinateTransformationList;

        explicit PolarCoordinatePlane ( Chart* parent = 0 );
        ~PolarCoordinatePlane();

        void addDiagram ( AbstractDiagram* diagram );

        const QPointF translate ( const QPointF& diagramPoint ) const;
        const QPointF translatePolar ( const QPointF& diagramPoint ) const;

        virtual double zoomFactorX() const;
        virtual double zoomFactorY() const;

        virtual void setZoomFactorX( double factor );
        virtual void setZoomFactorY( double factor );

        virtual QPointF zoomCenter() const;

        virtual void setZoomCenter( QPointF center );

        /**
         * Set the attributes to be used for grid lines drawn in circular
         * direction (or in sagittal direction, resp.).
         *
         * To disable circular grid painting, for example, your code should like this:
         * \code
         * GridAttributes ga = plane->gridAttributes( bool );
         * ga.setGridVisible( false );
         * plane-setGridAttributes( bool, ga );
         * \endcode
         *
         * \note setGridAttributes overwrites the global attributes that
         * were set by AbstractCoordinatePlane::setGlobalGridAttributes.
         * To re-activate these global attributes you can call
         * resetGridAttributes.
         *
         * \sa resetGridAttributes, gridAttributes
         * \sa AbstractCoordinatePlane::setGlobalGridAttributes
         * \sa hasOwnGridAttributes
         */
        void setGridAttributes( bool circular, const GridAttributes & );

        /**
         * Reset the attributes to be used for grid lines drawn in circular
         * direction (or in sagittal direction, resp.).
         * By calling this method you specify that the global attributes set by
         * AbstractCoordinatePlane::setGlobalGridAttributes be used.
         *
         * \sa setGridAttributes, gridAttributes
         * \sa AbstractCoordinatePlane::globalGridAttributes
         * \sa hasOwnGridAttributes
         */
        void resetGridAttributes( bool circular );

        /**
         * \return The attributes used for grid lines drawn in circular
         * direction (or in sagittal direction, resp.).
         *
         * \note This function always returns a valid set of grid attributes:
         * If no special grid attributes were set for this direction
         * the global attributes are returned, as returned by
         * AbstractCoordinatePlane::globalGridAttributes.
         *
         * \sa setGridAttributes
         * \sa resetGridAttributes
         * \sa AbstractCoordinatePlane::globalGridAttributes
         * \sa hasOwnGridAttributes
         */
        GridAttributes gridAttributes( bool circular ) const;

        /**
         * \return Returns whether the grid attributes have been set for the
         * respective direction via setGridAttributes( bool circular ).
         *
         * If false, the grid will use the global attributes set
         * by AbstractCoordinatePlane::globalGridAttributes (or the default
         * attributes, resp.)
         *
         * \sa setGridAttributes
         * \sa resetGridAttributes
         * \sa AbstractCoordinatePlane::globalGridAttributes
         */
        bool hasOwnGridAttributes( bool circular ) const;

        qreal angleUnit() const;

        /** reimpl */
        virtual void paint( QPainter* );

    protected:
        virtual DataDimensionsList getDataDimensionsList() const;
        void paintEvent ( QPaintEvent* );
        void resizeEvent ( QResizeEvent* );

        void layoutDiagrams();
    protected Q_SLOTS:
        void slotLayoutChanged( AbstractDiagram* diagram );

    private:
        void setHasOwnGridAttributes(
            bool circular, bool on );
    };

}

#endif
