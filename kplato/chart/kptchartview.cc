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

#include "kptchartview.h"

#include "kpttimescale.h"
#include "kptnumberscale.h"
#include "kptchartcanvas.h"

#include <qcanvas.h>
#include <qpainter.h>
#include <qevent.h>
#include <qlayout.h>
#include <qhbox.h>
#include <qlabel.h>

#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>

namespace KPlato
{

KPTChartView::KPTChartView(QWidget* parent, const char* name )
    : QWidget(parent, name),
      m_drawMode(KPTChartView::Mode_Bar),
      m_yZero(0.0),
      m_yZeroEnabled(true),
      m_yGridEnabled(false),
      m_yScaleUnit("%"),
      m_xMajorGrid(true),
      m_xMinorGrid(false)
{
    lh = new QHBoxLayout(this, 0, 0);
    
    QBoxLayout *left = new QVBoxLayout(lh);
    m_yScale = new KPTNumberScale(this);
    left->addWidget(m_yScale);
    m_cornerWidget = new QLabel(this);
    left->addWidget(m_cornerWidget);
    
    QBoxLayout *right = new QVBoxLayout(lh);
    m_chart = new KPTChartCanvasView(this);
    right->addWidget(m_chart);
        
    m_timeScale = new KPTTimeScale(this);
    right->addWidget(m_timeScale);

    m_cornerWidget->setMaximumHeight(m_timeScale->fullHeight());
    m_cornerWidget->setMinimumHeight(m_timeScale->fullHeight());
    
    activateLayout();

    m_yLines.setDrawMode(KPTChartDataSet::Mode_Line);
    m_yLines.setPen(new QPen(gray, 1, DashLine));    
    
    m_yRangeMin = 0.0;
    m_yRangeMax = 100.0;
    m_yStep = 10.0;
    
    connect(m_timeScale->horizontalScrollBar(), SIGNAL(valueChanged(int)), m_chart->horizontalScrollBar(), SLOT(setValue(int)));
    
    m_data.setAutoDelete(true);
    
    //kdDebug()<<k_funcinfo<<"Size: "<<width()<<"x"<<height()<<endl;
    //kdDebug()<<k_funcinfo<<"Y scale size: "<<m_yScale->width()<<"x"<<m_yScale->height()<<endl;
    //kdDebug()<<k_funcinfo<<"Time scale size: "<<m_timeScale->width()<<"x"<<m_timeScale->height()<<endl;
    //kdDebug()<<k_funcinfo<<"Chart size: "<<m_chart->width()<<"x"<<m_chart->height()<<endl;

}

KPTChartView::~KPTChartView()
{
}

void KPTChartView::setTimeScaleRange(const QDateTime &start, const QDateTime &end) { 
    m_startTime = start;
    m_endTime = end;
}

void KPTChartView::setYScaleRange(double min, double max, double step) {
    m_yRangeMin = min;
    m_yRangeMax = max;
    m_yStep = step;
}

int KPTChartView::canvasWidth() { 
    return m_chart->width(); 
}

void KPTChartView::addYMarkerLine(KPTChartDataSetItem *item) { 
    m_yLines.append(item); 
}
 
void KPTChartView::resizeEvent(QResizeEvent *) {
    //kdDebug()<<k_funcinfo<<"size="<<width()<<"x"<<height()<<endl;
    
    m_timeScale->resize(width()-m_yScale->canvas()->width(), m_timeScale->height());
    m_chart->canvas()->resize(QMAX(m_timeScale->contentsWidth(),width()), height()- m_timeScale->height() - 2);
    m_yScale->resize(m_yScale->canvas()->width(), height()- m_timeScale->height() - 2);
        
    drawChart();
}

void KPTChartView::activateLayout() {
    lh->activate();
}

void KPTChartView::setDrawMode(int mode) {
    m_drawMode = static_cast<Mode>(mode);
    drawData();
}

void KPTChartView::clear() {
    clearData();
    m_chart->clear();
    //draw();
}

void KPTChartView::clearData() {
    m_data.clear();
}

void KPTChartView::addData(KPTChartDataSet *data) {
    m_data.append(data);
}

void KPTChartView::drawChart() {
    m_chart->clear();
    if (m_yZeroEnabled) {
       m_chart->drawYZeroLine(m_yScale, m_yZero);
    }
    if (m_yGridEnabled) {
        m_chart->drawYGrid(m_yScale);
    }
    if (m_xMajorGrid) {
        m_chart->drawXMajorGrid(m_timeScale);
    }
    if (m_xMinorGrid) {
        m_chart->drawXMinorGrid(m_timeScale);    
    }
    m_chart->drawDescription(m_description);
    drawData();
    m_chart->canvasUpdate();
}

void KPTChartView::drawData() {
    //kdDebug()<<k_funcinfo<<endl;
    QPtrListIterator<KPTChartDataSet> sit = m_data;
    for (; sit.current(); ++sit) {
        QPtrListIterator<KPTChartDataSetItem> it = sit.current()->dataItems();
        for (; it.current(); ++it) {
            switch (sit.current()->drawMode()) {
            case KPTChartDataSet::Mode_Bar:
                m_chart->drawBar(it.current(), m_timeScale, m_yScale);
                break;
            case KPTChartDataSet::Mode_Line:
                m_chart->drawLine(it.current(), m_timeScale, m_yScale);
                break;
            default:
                m_chart->drawBar(it.current(), m_timeScale, m_yScale);
                break;
            }
        }
    }    
}

void KPTChartView::draw() {
    //kdDebug()<<k_funcinfo<<endl;
    m_yScale->setUnit(m_yScaleUnit);
    m_timeScale->setRange(m_startTime, m_endTime);
    m_yScale->setRange(m_yRangeMin, m_yRangeMax, m_yStep);
    m_yScale->draw();
    lh->activate();
    
    drawChart();
    
/*    int startDay = KGlobal::locale()->weekStartDay();
    if (m_startTime.daysTo(m_endTime) < 7) {
        int x = m_startTime.date().dayOfWeek();
        x = (x > startDay ? x - startDay : startDay - x);
        m_startTime = m_startTime.addDays(-x);
    }
    if (m_startTime.daysTo(m_endTime) < 7) {
        m_endTime = m_startTime.addDays(7);
    }
    m_timeScale->setHorizonStart(m_startTime);
    m_timeScale->setHorizonEnd(m_endTime);*/
    //drawYScale();
    //m_yScale->height() - 
/*    repaintContents(true);
    QPainter p(viewport());
    KDChart::paint(&p, m_mainview->params(), m_mainview->data());*/
}

}  //KPlato namespace

#include "kptchartview.moc"
