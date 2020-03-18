/* This file is part of the KDE project

   Copyright 2007-2008 Johannes Simon <johannes.simon@gmail.com>
   Copyright 2008-2010 Inge Wallin    <inge@lysator.liu.se>
   Copyright 2018 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KCHART_PLOTAREA_H
#define KCHART_PLOTAREA_H

// Qt
#include <QObject>
#include <QList>

// ChartShape
#include "ChartShape.h"
#include "DataSet.h"

namespace KChart {
    class CartesianCoordinatePlane;
    class PolarCoordinatePlane;
    class RadarCoordinatePlane;
}


class Ko3dScene;


namespace KoChart {

/**
 * @brief The PlotArea class is the central chart element. It plots the data and draws the axes.
 *
 * There always exists exactly one instance of this class, owned by
 * the chart shape. The plot area itself takes ownership of the axes
 * and the chart wall.
 *
 * 3D support is not yet fully implemented, a chart floor is not
 * supported at all yet.
 *
 * This class also plays a central role when loading from or saving to
 * ODF. Though it does not handle anything in particular itself, it
 * utilizes the DataSet, ChartTableModel, Axis, Surface, and
 * ChartProxyModel classes to handle ODF data and properties embedded
 * in the <chart:plotarea> element.
 */

class PlotArea : public QObject, public KoShape
{
    friend class Surface;
    friend class Axis;
    Q_OBJECT

public:
    explicit PlotArea(ChartShape *parent);
    ~PlotArea();

    void plotAreaInit();

    ChartProxyModel *proxyModel() const;

    ChartType    chartType() const;
    ChartSubtype chartSubType() const;
    void         setChartType(ChartType type);
    void         setChartSubType(ChartSubtype subType);

    QList<Axis*>    axes() const;
    QList<DataSet*> dataSets() const;
    int             dataSetCount() const;
    bool            addAxis(Axis *axis);
    /// Remove and delete the @p axis.
    /// Also deletes the title
    bool removeAxis(Axis *axis);
    /// Remove the @p axis, but do not delete it.
    /// The title is not touched.
    bool takeAxis(Axis *axis);

    // TODO: Rename this into primaryXAxis()
    Axis *xAxis() const;
    // TODO: Rename this into primaryYAxis()
    Axis *yAxis() const;
    Axis *secondaryXAxis() const;
    Axis *secondaryYAxis() const;

    bool isThreeD() const;
    Ko3dScene *threeDScene() const;

    /**
     * Determines from what range of cells the data in this chart
     * comes from. This region also contains the name of the sheet.
     * See table:cell-range-address, ODF v1.2, $18.595
     */
    CellRegion cellRangeAddress() const;

    /**
     * Determines whether x and y axis are swapped. Default is 'false'.
     * See chart:vertical attribute in ODF v1.2, $19.63
     *
     * NOTE: Only bar charts can be vertical so only returns true if chartType() is BarChartType
     */
    bool isVertical() const;

    /**
     * Defines at what angle, relative to the right-most point
     * of a pie or ring chart, the first slice is going to be drawn,
     * going counter-clockwise.
     * See chart:angle-offset property, as defined in ODF v1.2.
     */
    qreal angleOffset() const;

    /**
     * @see angleOffset
     */
    void setAngleOffset(qreal angle);

    /// The chart:hole-size attribute specifies the diameter of the inner hole of a ring chart
    /// as percentage of the outer diameter of the outermost ring.
    qreal holeSize() const;
    /// @see holeSize()
    void setHoleSize(qreal value);

    bool loadOdf(const KoXmlElement &plotAreaElement, KoShapeLoadingContext &context) override;
    bool loadOdfSeries(const KoXmlElement &seriesElement, KoShapeLoadingContext &context);

    void saveOdf(KoShapeSavingContext &context) const override;
    void saveOdfSubType(KoXmlWriter &bodyWriter, KoGenStyle &plotAreaStyle) const;


    void setThreeD(bool threeD);

    /**
     * @see cellRangeAddress
     */
    void setCellRangeAddress(const CellRegion &region);

    /**
     * @see isVertical
     */
    void setVertical(bool vertical);

    ChartShape *parent() const;

    void paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintcontext) override;

    bool registerKdDiagram(KChart::AbstractDiagram *diagram);
    bool deregisterKdDiagram(KChart::AbstractDiagram *diagram);

    void relayout() const;

    void registerKdPlane(KChart::AbstractCoordinatePlane *plane);

    void addTitleToLayout();

    void setStockRangeLinePen(const QPen &pen);
    QPen stockRangeLinePen() const;
    void setStockGainBrush(const QBrush &brush);
    QBrush stockGainBrush() const;
    void setStockLossBrush(const QBrush &brush);
    QBrush stockLossBrush() const;

    QString symbolType() const;
    void setSymbolType(const QString &type);
    QString symbolName() const;
    void setSymbolName(const QString &name);
    DataSet::ValueLabelType valueLabelType() const;
    void setValueLabelType(const DataSet::ValueLabelType &type);

public Q_SLOTS:
    void requestRepaint() const;
    void proxyModelStructureChanged();
    void plotAreaUpdate();

    void updateKChartStockAttributes();

Q_SIGNALS:
    void angleOffsetChanged(qreal);
    void holeSizeChanged(qreal);

private:
    void addAxesTitlesToLayout();
    void paintPixmap(QPainter &painter, const KoViewConverter &converter);

    // For class Axis
    KChart::CartesianCoordinatePlane *kdCartesianPlane(Axis *axis = 0) const;
    KChart::PolarCoordinatePlane *kdPolarPlane() const;
    KChart::RadarCoordinatePlane *kdRadarPlane() const;
    KChart::Chart *kdChart() const;

    class Private;
    Private *const d;
};

} // Namespace KoChart

#endif // KCHART_PLOTAREA_H

