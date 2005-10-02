/* This file is part of the KDE project
   Copyright (C) 2005 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KPTCHARTVIEW_H
#define KPTCHARTVIEW_H

#include <qwidget.h>

#include "kpttimescale.h"
#include "kptchartdataset.h"

#include <qdatetime.h>
#include <qcanvas.h>
#include <koffice_export.h>

class QanvasView;
class QBoxLayout;

namespace KPlato
{

class KPTNumberScale;
class KPTTimeScale;
class KPTChartCanvasView;

/**
 * KPTChartView can draw bar- and linecharts.
 * The x-axis is a timescale and the y-axis is a numberscale.
 * The x-axis can be scrolled when the whole x-axis cannot be 
 * displayed in the given window width.
 * The y-axis is scaled to fit the window height.
 */
class KPLATOCHART_EXPORT KPTChartView : public QWidget
{
    Q_OBJECT
public:    
    KPTChartView(QWidget* parent = 0, const char* name = 0);
    ~KPTChartView();

    /// Clears the datasets and the chart (but not the scales)
    void clear();
    /// Clears the datasets
    void clearData();
    /// Adds the dataset to the list of data to be drawn on the chart
    void addData(KPTChartDataSet *data);    
    /// Adds a horisontal line to the chart
    void addHorisontalLine(KPTChartDataSetItem *item);
    /// Sets for which value the zero line shall be drawn
    void setYZero(double zero) { m_yZero = zero; }
    /// Enable/disable horisontal zero line
    void enableYZeroLine(bool on) { m_yZeroEnabled = on; }
    /// Enable/disable horisontal grid lines
    void enableYGrid(bool on) { m_yGridEnabled = on; }
    /// Enable/disable vertical grid lines for major timescale
    void enableXMajorGrid(bool on) { m_xMajorGrid = on; }
    /// Enable/disable vertical grid lines for minor timescale
    void enableXMinorGrid(bool on) { m_xMinorGrid = on; }
    /// Set the chart description
    void setDescription(const QString &desc) { m_description = desc; }
    /// Set the minor timescale unit
    void setTimeScaleUnit(KPTTimeHeaderWidget::Scale unit) { m_timeScaleUnit = unit; }
    /// Set the numberscale unit description
    void setYScaleUnit(const QString &unit) { m_yScaleUnit = unit; }
    /// Set the timescale range
    void setTimeScaleRange(const QDateTime &start, const QDateTime &end);
    /// Set the numberscale range
    void setYScaleRange(double min, double max, double step=0.0);
    
public slots:
    /// Draw the scales and chart
    void draw();

    /// Activate layout'ing of the widgets
    void activateLayout();

signals:
    /// The timescale unit has been changed
    void timeScaleUnitChanged(int unit);
    /// Context menu requested by the chart
    void chartMenuRequest(const QPoint &pos);
    /// Context menu requested by the numberscale
    void contextMenuRequested(KPTNumberScale *yScale, const QPoint & pos);
    
protected slots:
    void slotTimeScaleWidthChanged(int w);
    void slotTimeScaleUnitChanged(int unit);
    void slotTimeFormatChanged(int format);
    void slotChartMenuRequested(const QPoint &pos);
    void hScrollBarValueChanged(int value);
    
protected:    
    virtual void resizeEvent(QResizeEvent *);
    void drawChart();
    void drawData();
    void clearYLines();
    void addYLineAt(double value);

private:
    double m_yZero;
    bool m_yZeroEnabled;
    bool m_yGridEnabled;
    QString m_yScaleUnit;
    bool m_xMajorGrid;
    bool m_xMinorGrid;
    KPTTimeHeaderWidget::Scale m_timeScaleUnit;
    
    KPTNumberScale *m_yScale;
    double m_yRangeMin;
    double m_yRangeMax;
    double m_yStep;
    KPTTimeScale *m_timeScale;
    KPTChartCanvasView *m_chart;
    
    QDateTime m_startTime;
    QDateTime m_endTime;
    KPTChartDataSet m_yLines;
    QPtrList<KPTChartDataSet> m_data;
    
    QBoxLayout *lh;
    QWidget *m_cornerWidget;
    
    QString m_description;
    QString m_unit;
};

}  //KPlato namespace

#endif


