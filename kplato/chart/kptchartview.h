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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KPTCHARTVIEW_H
#define KPTCHARTVIEW_H

#include <qwidget.h>

#include "kptchartdataset.h"

#include <qdatetime.h>
#include <qcanvas.h>

class QanvasView;
class QBoxLayout;

namespace KPlato
{

class KPTNumberScale;
class KPTTimeScale;
class KPTChartCanvasView;

class KPTChartView : public QWidget
{
    Q_OBJECT
public:
    enum Mode { Mode_Bar };
    
    KPTChartView(QWidget* parent = 0, const char* name = 0);
    ~KPTChartView();

    void setDrawMode(int mode);
    void clear();
    void clearData();
    void addData(KPTChartDataSet *data);
    int canvasWidth();    
    
    void addYMarkerLine(KPTChartDataSetItem *item);
    void setYZero(double zero) { m_yZero = zero; }
    void enableYZeroLine(bool on) { m_yZeroEnabled = on; }
    void enableYGrid(bool on) { m_yGridEnabled = on; }
    
    void enableXMajorGrid(bool on) { m_xMajorGrid = on; }
    void enableXMinorGrid(bool on) { m_xMinorGrid = on; }
    
    void setDescription(const QString &desc) { m_description = desc; }
    void setYScaleUnit(const QString &unit) { m_yScaleUnit = unit; }

public slots:
    void draw();
    void activateLayout();

    void setTimeScaleRange(const QDateTime &start, const QDateTime &end);
    void setYScaleRange(double min, double max, double step=0.0);
    
protected:    
    virtual void resizeEvent(QResizeEvent *);
    void drawChart();
    void drawData();
    void clearYLines();
    void addYLineAt(double value);

private:
    Mode m_drawMode;
    double m_yZero;
    bool m_yZeroEnabled;
    bool m_yGridEnabled;
    QString m_yScaleUnit;
    bool m_xMajorGrid;
    bool m_xMinorGrid;
    
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


