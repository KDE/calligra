/* This file is part of the KDE project

   Copyright 2007 Johannes Simon <johannes.simon@gmail.com>
   Copyright 2009 Inge Wallin    <inge@lysator.liu.se>

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

#ifndef KCHART_AXIS_H
#define KCHART_AXIS_H


// Qt
#include <QObject>

// KoChart
#include "ChartShape.h"

#include <KChartCartesianAxis.h>

namespace KoOdfNumberStyles {
    struct NumericStyleFormat;
}

namespace KoChart {

enum OdfGridClass {
    OdfMajorGrid,
    OdfMinorGrid
};

/**
 * @brief The Axis class handles axis as well as grid settings.
 *
 * Data series can be attached to axes that represent an
 * ordinate. This is done to customize the scaling, i.e., the relation
 * in which the data of a series is visualized.
 */

class Axis : public QObject
{
    Q_OBJECT

public:
    Axis(PlotArea *parent, AxisDimension dimension);
    ~Axis();

    // odf chart:name that identifies the axis
    QString name() const;
    void setName(const QString &name);

    PlotArea *plotArea() const;
    KoShape *title() const;
    QString titleText() const;
    bool showLabels() const;
    bool showOverlappingDataLabels() const;
    QString id() const;
    AxisDimension dimension() const;
    QList<DataSet*> dataSets() const;
    qreal majorInterval() const;
    qreal minorInterval() const;
    int minorIntervalDivisor() const;
    bool useAutomaticMajorInterval() const;
    bool useAutomaticMinorInterval() const;
    bool showInnerMinorTicks() const;
    bool showOuterMinorTicks() const;
    bool showInnerMajorTicks() const;
    bool showOuterMajorTicks() const;
    bool scalingIsLogarithmic() const;
    bool showMajorGrid() const;
    bool showMinorGrid() const;
    Qt::Orientation orientation() const;
    QFont font() const;
    qreal fontSize() const;
    bool isVisible() const;
    bool showRuler() const;

    void setTitleText(const QString &text);
    void setShowLabels(bool show);
    void setShowOverlappingDataLabels(bool show);

    /**
     * Attaches a data set to this axis, adding it to a diagram
     * of its chart type, creating it if necessary
     */
    bool attachDataSet(DataSet *dataSet);

    /**
     * Detaches a data set from this axis, removing it from the diagram of
     * its chart type, and deleting it if it was the last data set in this diagram.
     */
    bool detachDataSet(DataSet *dataSet, bool silent = false);

    /**
     * Detaches all data sets in this axis, deleting any diagram
     * that this axis might have owned.
     */
    void clearDataSets();

    void setMajorInterval(qreal interval);
    void setMinorInterval(qreal interval);
    void setMinorIntervalDivisor(int divisor);
    void setUseAutomaticMajorInterval(bool automatic);
    void setUseAutomaticMinorInterval(bool automatic);
    void setShowInnerMinorTicks(bool showTicks);
    void setShowOuterMinorTicks(bool showTicks);
    void setShowInnerMajorTicks(bool showTicks);
    void setShowOuterMajorTicks(bool showTicks);
    void setScalingLogarithmic(bool logarithmicScaling);
    void setShowMajorGrid(bool showGrid);
    void setShowMinorGrid(bool showGrid);
    void setThreeD(bool threeD);
    void setFont(const QFont &font);
    void setFontSize(qreal size);
    void setVisible(bool visible);
    void setShowRuler(bool show);

    KoOdfNumberStyles::NumericStyleFormat *numericStyleFormat() const;
    void SetNumericStyleFormat(KoOdfNumberStyles::NumericStyleFormat *numericStyleFormat) const;

    bool loadOdf(const KoXmlElement &axisElement, KoShapeLoadingContext &context);
    bool loadOdfChartSubtypeProperties(const KoXmlElement &axisElement,
                                        KoShapeLoadingContext &context);
    void saveOdf(KoShapeSavingContext &context);
    void saveOdfGrid(KoShapeSavingContext &context, OdfGridClass gridClass);

    // KChart stuff
    KChart::CartesianAxis *kdAxis() const;
    KChart::AbstractCoordinatePlane *kdPlane() const;

    void plotAreaChartTypeChanged(ChartType chartType);
    void plotAreaChartSubTypeChanged(ChartSubtype chartSubType);
    void plotAreaIsVerticalChanged();

    void registerAxis(Axis *axis);
    void registerDiagram(KChart::AbstractCartesianDiagram *diagram);
    void removeAxisFromDiagrams(bool clear = false);

    void update() const;
    void requestRepaint() const;
    void layoutPlanes();

    /**
     * Returns the gap between bars as a  percent of the width of one bar
     *
     * Negative numbers signify that bars overlap
     * This is the negative representation of the chart:overlap property.
     *
     * From odf 1.2 spec:
     * The chart:overlap attribute specifies how much bars within the same category in a bar chart overlap.
     * The attribute value is an integer that is interpreted as a percentage relative to the width of a single bar.
     * Negative values specify gaps between bars.
     * This attribute is evaluated for chart styles that are applied to a <chart:axis> element when a chart:dimension attribute set to y.
     * Note: This allows bars to be attached to different axis arranged differently as long as they are grouped per axis (chart:group-bars-per-axis attribute is true).
     */
    int gapBetweenBars() const;
    /**
     * Returns the gap between sets of bars as a  percent of the width of one bar
     * This represents the chart:gap-width property.
     *
     * From the odf 1.2 spec:
     * The chart:gap-width attribute specifies a gap between neighboring groups of bars in a bar chart
     * (that is the distance between the last bar in one category and t*he first bar in the following category).
     * It is specified as an integer percentage relative to the width of a single bar.
     * This attribute is evaluated for chart styles that are applied to a <chart:axis> element with chart:dimension attribute set to y.
     * Note: Bars attached to different axis can be arranged differently as long as they are grouped per axis (chart:group-bars-per-axis attribute has the value true).
     */
    int gapBetweenSets() const;

    /**
     * Set axis position to @p odfpos
     * odfpos can be "start", "end" or a double value
     */
    void setOdfAxisPosition(const QString &odfpos);
    /// @return the axis position in odf format
    /// @see setOdfAxisPosition()
    QString odfAxisPosition() const;
    /**
     * Update the chart axis position from the odf position
     * ODF defines:
     * end: Translates to Top for x-axes and Right for y-axes
     *      If reversed: Translates to Bottom for x-axes and Left for y-axes
     * start: Translates to Bottom for x-axes and Left for y-axes
     *        If reversed: Translates to Top for x-axes and Right for y-axes
     * value: Not supported, defaults to "start"
     *
     * @see odfAxisPosition()
     */
    void updateKChartAxisPosition();
    /// @return the KChart axis position
    KChart::CartesianAxis::Position kchartAxisPosition() const;
    // @return the actual KChart axis position
    // This is not quite kchartAxisPosition() as it also depends on bar diagram orientation
    KChart::CartesianAxis::Position actualAxisPosition() const;

    /// @return true if direction is reversed
    bool axisDirectionReversed() const;

    void setOdfAxisLabelsPosition(const QString &odfpos);
    QString odfAxisLabelsPosition() const;

public Q_SLOTS:
    void setGapBetweenBars(int percent);
    void setGapBetweenSets(int percent);
    void setAngleOffset(qreal angle);
    void setHoleSize(qreal value);

    void updateKChartStockAttributes();

private:
    class Private;
    Private *const d;
};

} // Namespace KoChart

QDebug operator<<(QDebug dbg, KoChart::Axis *a);

#endif
