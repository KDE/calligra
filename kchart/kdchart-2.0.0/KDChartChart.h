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

#ifndef KDCHARTCHART_H
#define KDCHARTCHART_H

#include <QWidget>

#include "kdchart_export.h"
#include "KDChartGlobal.h"

namespace KDChart {

    class AbstractDiagram;
    class AbstractCoordinatePlane;
    class HeaderFooter;
    class Legend;

    typedef QList<AbstractCoordinatePlane*> CoordinatePlaneList;
    typedef QList<HeaderFooter*> HeaderFooterList;
    typedef QList<Legend*> LegendList;

    /**
     * @brief A chart with one or more diagrams.
     *
     * The Chart class represents a drawing consisting of one or more diagrams
     * and various optional elements such as legends, axes, text boxes, headers
     * or footers. It takes ownership of all these elements when they are assigned
     * to it. Each diagram is associated with a coordinate plane, of which the chart
     * can have more than one. The coordinate planes (and thus the associated diagrams)
     * can be layed out in various ways.
     *
     * The Chart class makes heavy use of the Qt Interview framework for model/view
     * programming, and thus requires data to be presented to it in a QAbstractItemModel
     * compatible way. For many simple charts, especially if the visualized data is
     * static, KDChart::Widget provides an abstracted interface, that hides the complexity
     * of Interview to a large extent.
     */
    class KDCHART_EXPORT Chart : public QWidget
    {
        Q_OBJECT
        Q_PROPERTY( int globalLeadingTop READ globalLeadingTop WRITE setGlobalLeadingTop )
        Q_PROPERTY( int globalLeadingBottom READ globalLeadingBottom WRITE setGlobalLeadingBottom )
        Q_PROPERTY( int globalLeadingLeft READ globalLeadingLeft WRITE setGlobalLeadingLeft )
        Q_PROPERTY( int globalLeadingRight READ globalLeadingRight WRITE setGlobalLeadingRight )

        KDCHART_DECLARE_PRIVATE_BASE_POLYMORPHIC_QWIDGET( Chart )

    public:
        explicit Chart ( QWidget* parent = 0 );
        ~Chart();

        /**
         * Each chart must have at least one coordinate plane.
         * Initially a default CartesianCoordinatePlane is created.
         * Use replaceCoordinatePlane() to replace it with a different
         * one, such as a PolarCoordinatePlane.
         * @return The first coordinate plane of the chart.
         */
        AbstractCoordinatePlane* coordinatePlane();

        /**
         * The list of coordinate planes.
         * @return The list of coordinate planes.
         */
        CoordinatePlaneList coordinatePlanes();

        /**
         * Adds a coordinate plane to the chart. The chart takes ownership.
         * @param plane The coordinate plane to add.
         *
         * \sa replaceCoordinatePlane, takeCoordinatePlane
         */
        void addCoordinatePlane( AbstractCoordinatePlane* plane );

        /**
         * Replaces the old coordinate plane, or appends the
         * plane, it there is none yet.
         *
         * @param plane The coordinate plane to be used instead of the old plane.
         * This parameter must not be zero, or the method will do nothing.
         *
         * @param oldPlane The coordinate plane to be removed by the new plane. This
         * plane will be deleted automatically. If the parameter is omitted,
         * the very first coordinate plane will be replaced. In case, there was no
         * plane yet, the new plane will just be added.
         *
         * \note If you want to re-use the old coordinate plane, call takeCoordinatePlane and
         * addCoordinatePlane, instead of using replaceCoordinatePlane.
         *
         * \sa addCoordinatePlane, takeCoordinatePlane
         */
        void replaceCoordinatePlane( AbstractCoordinatePlane* plane,
                                     AbstractCoordinatePlane* oldPlane = 0 );

        /**
         * Removes the coordinate plane from the chart, without deleting it.
         *
         * The chart no longer owns the plane, so it is
         * the caller's responsibility to delete the plane.
         *
         * \sa addCoordinatePlane, takeCoordinatePlane
         */
        void takeCoordinatePlane( AbstractCoordinatePlane* plane );


        void setCoordinatePlaneLayout( QLayout * layout );

        /**
         * The first header or footer of the chart. By default there is none.
         * @return The first header or footer of the chart or 0 if there was none
         * added to the chart.
         */
        HeaderFooter* headerFooter();

        /**
         * The list of headers and footers associated with the chart.
         * @return The list of headers and footers associated with the chart.
         */
        HeaderFooterList headerFooters();

        /**
         * Adds a header or a footer to the chart. The chart takes ownership.
         * @param headerFooter The header (or footer, resp.) to add.
         *
         * \sa replaceHeaderFooter, takeHeaderFooter
         */
        void addHeaderFooter( HeaderFooter* headerFooter );

        /**
         * Replaces the old header (or footer, resp.), or appends the
         * new header or footer, it there is none yet.
         *
         * @param headerFooter The header or footer to be used instead of the old one.
         * This parameter must not be zero, or the method will do nothing.
         *
         * @param oldHeaderFooter The header or footer to be removed by the new one. This
         * header or footer will be deleted automatically. If the parameter is omitted,
         * the very first header or footer will be replaced. In case, there was no
         * header and no footer yet, the new header or footer will just be added.
         *
         * \note If you want to re-use the old header or footer, call takeHeaderFooter and
         * addHeaderFooter, instead of using replaceHeaderFooter.
         *
         * \sa addHeaderFooter, takeHeaderFooter
         */
        void replaceHeaderFooter ( HeaderFooter* headerFooter,
                                   HeaderFooter* oldHeaderFooter = 0 );

        /**
         * Removes the header (or footer, resp.) from the chart, without deleting it.
         *
         * The chart no longer owns the header or footer, so it is
         * the caller's responsibility to delete the header or footer.
         *
         * \sa addHeaderFooter, replaceHeaderFooter
         */
        void takeHeaderFooter( HeaderFooter* headerFooter );

        /**
         * The first legend of the chart or 0 if there was none added to the chart.
         * @return The first legend of the chart or 0 if none exists.
         */
        Legend* legend();

        /**
         * The list of all legends associated with the chart.
         * @return The list of all legends associated with the chart.
         */
        LegendList legends();

        /**
         * Add the given legend to the chart. The chart takes ownership.
         * @param legend The legend to add.
         *
         * \sa replaceLegend, takeLegend
         */
        void addLegend( Legend* legend );

        /**
         * Replaces the old legend, or appends the
         * new legend, it there is none yet.
         *
         * @param legend The legend to be used instead of the old one.
         * This parameter must not be zero, or the method will do nothing.
         *
         * @param oldLegend The legend to be removed by the new one. This
         * legend will be deleted automatically. If the parameter is omitted,
         * the very first legend will be replaced. In case, there was no
         * legend yet, the new legend will just be added.
         *
         * \note If you want to re-use the old legend, call takeLegend and
         * addLegend, instead of using replaceLegend.
         *
         * \sa addLegend, takeLegend
         */
        void replaceLegend ( Legend* legend, Legend* oldLegend = 0 );

        /**
         * Removes the legend from the chart, without deleting it.
         *
         * The chart no longer owns the legend, so it is
         * the caller's responsibility to delete the legend.
         *
         * \sa addLegend, takeLegend
         */
        void takeLegend( Legend* legend );

        /**
         * Set the padding between the margin of the widget and the area that
         * the contents are drawn into.
         * @param left The padding on the left side.
         * @param top The padding at the top.
         * @param right The padding on the left hand side.
         * @param bottom The padding on the bottom.
         *
         * \sa setGlobalLeadingTop, setGlobalLeadingBottom, setGlobalLeadingLeft, setGlobalLeadingRight
         * \sa globalLeadingTop, globalLeadingBottom, globalLeadingLeft, globalLeadingRight
         */
        void setGlobalLeading( int left, int top, int right, int bottom );

        /**
         * Set the padding between the start of the widget and the start
         * of the area that is used for drawing on the left.
         * @param leading The padding value.
         *
         * \sa setGlobalLeading
         */
        void setGlobalLeadingLeft( int leading );

        /**
         * The padding between the start of the widget and the start
         * of the area that is used for drawing on the left.
         * @return The padding between the start of the widget and the start
         * of the area that is used for drawing on the left.
         *
         * \sa setGlobalLeading
         */
        int globalLeadingLeft() const;

        /**
         * Set the padding between the start of the widget and the start
         * of the area that is used for drawing at the top.
         * @param leading The padding value.
         *
         * \sa setGlobalLeading
         */
        void setGlobalLeadingTop( int leading );

        /**
         * The padding between the start of the widget and the start
         * of the area that is used for drawing at the top.
         * @return The padding between the start of the widget and the start
         * of the area that is used for drawing at the top.
         *
         * \sa setGlobalLeading
         */
        int globalLeadingTop() const;

        /**
         * Set the padding between the start of the widget and the start
         * of the area that is used for drawing on the right.
         * @param leading The padding value.
         *
         * \sa setGlobalLeading
         */
        void setGlobalLeadingRight( int leading );

        /**
         * The padding between the start of the widget and the start
         * of the area that is used for drawing on the right.
         * @return The padding between the start of the widget and the start
         * of the area that is used for drawing on the right.
         *
         * \sa setGlobalLeading
         */
        int globalLeadingRight() const;

        /**
         * Set the padding between the start of the widget and the start
         * of the area that is used for drawing on the bottom.
         * @param leading The padding value.
         *
         * \sa setGlobalLeading
         */
        void setGlobalLeadingBottom( int leading );

        /**
         * The padding between the start of the widget and the start
         * of the area that is used for drawing at the bottom.
         * @return The padding between the start of the widget and the start
         * of the area that is used for drawing at the bottom.
         *
         * \sa setGlobalLeading
         */
        int globalLeadingBottom() const;

        /**
          * Paints all the contents of the chart. Use this method, to make KD Chart
          * draw into your QPainter.
          *
          * \note Any global leading settings will be used by the paint method too,
          * so make sure to set them to zero, if you want the drawing to have the exact
          * size of the target rectangle.
          *
          * \param painter The painter to be drawn into.
          * \param target The rectangle to be filled by the Chart's drawing.
          *
          * \sa setGlobalLeading
          */
        void paint( QPainter* painter, const QRect& target );

    Q_SIGNALS:
        /** Emitted upon change of a property of the Chart or any of its components. */
        void propertiesChanged();

    protected:
        /**
          * Adjusts the internal layout when the chart is resized.
          */
        /* reimp */ void resizeEvent ( QResizeEvent * event );

        /**
          * @brief Draws the background and frame, then calls paint().
          *
          * In most cases there is no need to override this method in a derived
          * class, but if you do, do not forget to call paint().
          * @sa paint
          */
        /* reimp */ void paintEvent( QPaintEvent* event );
    };

}

#endif
