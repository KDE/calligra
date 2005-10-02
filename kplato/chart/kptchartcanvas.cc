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

#include "kptchartcanvas.h"
#include "kptchartdataset.h"
#include "kpttimescale.h"
#include "kptnumberscale.h"

#include <kdebug.h>

#include <qpopupmenu.h>
#include <qdrawutil.h>
#include <qsize.h>
#include <qstringlist.h>
#include <qevent.h>

namespace KPlato
{

KPTChartCanvasView::KPTChartCanvasView(QWidget *parent, const char * name)
    : QCanvasView(parent, name)
{
    setHScrollBarMode(QScrollView::AlwaysOff);
    setVScrollBarMode(QScrollView::AlwaysOff);
    setCanvas(new KPTChartCanvasView::Canvas(this));
    
    m_yZeroLine = new QCanvasLine(canvas());
    m_yZeroLine->setZ(100.0);
    m_description = new QCanvasText(canvas());
    m_description->setZ(1000.0);
    
    connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), SLOT(canvasMoved()));

    //kdDebug()<<k_funcinfo<<"size: "<<width()<<","<<height()<<" frame: "<<frameSize().height()<<endl;

}

KPTChartCanvasView::~KPTChartCanvasView() {
    delete canvas();
}

void KPTChartCanvasView::canvasUpdate() {
    canvas()->update();
}

void KPTChartCanvasView::clear() {
    //kdDebug()<<k_funcinfo<<endl;
    m_xGridLines.hide();
    m_yGridLines.hide();
    m_description->hide();
    m_yZeroLine->hide();
    m_horisontalLines.hide();
    m_lines.hide();
    m_bars.hide();
    
    //canvas()->update();
}

KPTChartCanvasView::Canvas *KPTChartCanvasView::chartCanvas() {
    return static_cast<KPTChartCanvasView::Canvas *>(canvas());
}

void KPTChartCanvasView::drawYZeroLine(KPTNumberScale *scale, double zero) {
    //kdDebug()<<k_funcinfo<<"zero="<<zero<<" at "<<scale->posY(zero)<<endl;
    m_yZeroLine->hide();
    int y = (int)scale->posY(zero);
    m_yZeroLine->setPoints(0, y, canvas()->width(), y);
    m_yZeroLine->show();
}

void KPTChartCanvasView::drawYGrid(KPTNumberScale *scale, const QValueList<double> &list) {
    m_yGridLines.hide();
    QCanvasLine *l = 0;
    QValueList<double>::const_iterator it;
    for (it = list.begin(); it != list.end(); ++it) {
        l = m_yGridLines.getItem(canvas());
        l->setPen(QPen(gray, 1, DotLine));
        l->setZ(-100.0);
        int y = (int)scale->posY(*it);
        l->setPoints(0, y, canvas()->width(), y);
        m_yGridLines.show(l);
    }
}

void KPTChartCanvasView::drawYGrid(KPTNumberScale *scale, double step) {
    if (step <= 0.0) {
        drawYGrid(scale, scale->intervals()); // use intervals from y-scale
    } else {
        QValueList<double> list;
        for (double v = scale->rangeMin(); v <= scale->rangeMax(); v += step) {
            list.append(v);
        }
        drawYGrid(scale, list);
    }
}

void KPTChartCanvasView::drawXMajorGrid(KPTTimeScale *x, KPTNumberScale *y) {
    m_xGridLines.hide();
    int top = (int)y->posY(y->rangeMax());
    int bottom = (int)y->posY(y->rangeMin());
    QCanvasLine *l = 0;
    QValueList<int> list = x->majorGridValues();
    QValueList<int>::iterator it;
    for (it = list.begin(); it != list.end(); ++it) {
        l = m_xGridLines.getItem(canvas());
        l->setPen(QPen(gray, 1, DotLine));
        l->setZ(-100.0);
        l->setPoints((int)(*it), top, (int)(*it), bottom);
        m_xGridLines.show(l);
    }
}

void KPTChartCanvasView::drawXMinorGrid(KPTTimeScale *scale) {
/*    QCanvasLine *l = new QCanvasLine(canvas());
    l->setPoints(0, 0, x, canvas()->height());
    l->show();
    canvas()->update();*/
}

void KPTChartCanvasView::drawBar(KPTChartDataSetItem *data, KPTTimeScale *x, KPTNumberScale *y) {
    //kdDebug()<<k_funcinfo<<contentsX()<<","<<contentsY()<<" "<<visibleWidth()<<"x"<<visibleHeight()<<endl;
    int cx1 = contentsX();
    int cx2 = cx1 + visibleWidth();
    int x1 = (int)x->posX(data->x1());
    int x2 = (int)x->posX(data->x2());
    if ((x1 < cx1 || x1 > cx2) && (x2 < cx1 || x2 > cx2)) {
        return; // don't paint outside view
    }
    int y1 = (int)y->posY(data->y1());
    int y2 = (int)y->posY(data->y2());
    QPen *pen = data->pen();
    QBrush *brush = data->brush();
    //kdDebug()<<k_funcinfo<<"x1="<<x1<<" y1="<<y1<<" x2="<<x2<<" y2="<<y2<<endl;
    
    QCanvasRectangle *r = m_bars.getItem(canvas());
    r->setSize(x2-x1, y2-y1);
    r->move(x1, y1);
    r->setZ(data->prio());
    r->setPen(pen ? *pen : QPen(QColor(black)));
    r->setBrush(brush ? QBrush(*brush) : QBrush(QColor(green)));
    m_bars.show(r);
}

void KPTChartCanvasView::drawHorisontalLine(KPTChartDataSetItem *data, KPTTimeScale *x, KPTNumberScale *y) {
    int y1 = (int)y->posY(data->y1());
    QPen *pen = data->pen();
    QCanvasLine *l = m_horisontalLines.getItem(canvas());
    l->setZ(data->prio());
    l->setPen(pen ? QPen(*pen) : QPen(QColor(black)));
    l->setPoints(0, y1, canvas()->width(), y1);
    m_horisontalLines.show(l);
}

void KPTChartCanvasView::drawLine(KPTChartDataSetItem *data, KPTTimeScale *x, KPTNumberScale *y) {
    int x1 = (int)x->posX(data->x1());
    int y1 = (int)y->posY(data->y1());
    int x2 = (int)x->posX(data->x2());
    int y2 = (int)y->posY(data->y2());
    QPen *pen = data->pen();
    QCanvasLine *l = m_lines.getItem(canvas());
    l->setZ(data->prio());
    l->setPen(pen ? QPen(*pen) : QPen(QColor(black)));
    l->setPoints(x1, y1, x2, y2);
    m_lines.show(l);
    
    //kdDebug()<<k_funcinfo<<" at "<<data->x1().toString()<<","<<data->y1()<<" to "<<data->x2().toString()<<","<<data->y2()<<endl;
}

void KPTChartCanvasView::canvasMoved() {
    drawDescription();
    canvas()->update();
}

void KPTChartCanvasView::drawDescription() {
    int w = QMAX(30, QMIN(visibleWidth(), canvas()->size().width()));
    m_description->move(contentsX()+(w/10), contentsY()+2);
    m_description->show();
}

void KPTChartCanvasView::drawDescription(const QString &desc) {
    m_description->hide();
    m_description->setText(desc);
    drawDescription();
}

void KPTChartCanvasView::contentsContextMenuEvent(QContextMenuEvent *e) {
    kdDebug()<<k_funcinfo<<endl;
    emit contextMenuRequest(e->globalPos());
}

/********************************************/
KPTChartCanvasView::Canvas::Canvas(QObject *parent, const char *name)
    : QCanvas(parent, name)
{
}

KPTChartCanvasView::Canvas::~Canvas() {
    clear();
}

void KPTChartCanvasView::Canvas::clear() {
    QCanvasItemList list = allItems();
    QCanvasItemList::iterator it;
    for ( it = list.begin(); it != list.end(); ++it ) {
        delete (*it);
    }
}

} // KPlato namespace

#include "kptchartcanvas.moc"
