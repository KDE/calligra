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

#include "kptchartcanvas.h"
#include "kptchartdataset.h"
#include "kpttimescale.h"
#include "kptnumberscale.h"

#include <kdebug.h>

#include <qpopupmenu.h>
#include <qpainter.h>
#include <qdrawutil.h>
#include <qsize.h>
#include <qstringlist.h>
#include <qevent.h>

namespace KPlato
{

KPTChartCanvasView::KPTChartCanvasView(QWidget *parent, const char * name)
    : QCanvasView(parent, name),
      m_description(0)
{
    setHScrollBarMode(QScrollView::AlwaysOff);
    setCanvas(new KPTChartCanvasView::Canvas(this));
    
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
    m_xGridLines.clear();
    m_yGridLines.clear();
    if (m_description) {
        delete m_description;
        m_description = 0;
    }
    QCanvasItemList list = canvas()->allItems();
    QCanvasItemList::iterator it;
    for ( it = list.begin(); it != list.end(); ++it ) {
        delete (*it);
    }
    //canvas()->update();
}

KPTChartCanvasView::Canvas *KPTChartCanvasView::chartCanvas() {
    return static_cast<KPTChartCanvasView::Canvas *>(canvas());
}

void KPTChartCanvasView::drawYZeroLine(KPTNumberScale *scale, double zero) {
    //kdDebug()<<k_funcinfo<<"zero="<<zero<<" at "<<scale->posY(zero)<<endl;
    QCanvasLine *l = new QCanvasLine(canvas());
    l->setPoints(0, (int)scale->posY(zero), canvas()->width(), (int)scale->posY(zero));
    l->setZ(100.0);
    l->show();
}

void KPTChartCanvasView::drawYGrid(KPTNumberScale *scale, const QValueList<double> &list) {
    m_yGridLines.setAutoDelete(true);
    m_yGridLines.clear();
    m_yGridLines.setAutoDelete(false);
    QValueList<double>::const_iterator it;
    for (it = list.begin(); it != list.end(); ++it) {
        QCanvasLine *l = new QCanvasLine(canvas());
        l->setPen(QPen(gray, 1, DotLine));
        l->setPoints(0, (int)scale->posY(*it), canvas()->width(), (int)scale->posY(*it));
        l->setZ(-100.0);
        l->show();
        m_yGridLines.append(l);
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
    m_xGridLines.setAutoDelete(true);
    m_xGridLines.clear();
    m_xGridLines.setAutoDelete(false);
    int top = (int)y->posY(y->rangeMax());
    int bottom = (int)y->posY(y->rangeMin());
    QValueList<int> list = x->majorGridValues();
    QValueList<int>::iterator it;
    for (it = list.begin(); it != list.end(); ++it) {
        QCanvasLine *l = new QCanvasLine(canvas());
        l->setPen(QPen(gray, 1, DotLine));
        l->setPoints((int)(*it), top, (int)(*it), bottom);
        l->show();
        m_xGridLines.append(l);
    }
}

void KPTChartCanvasView::drawXMinorGrid(KPTTimeScale *scale) {
/*    QCanvasLine *l = new QCanvasLine(canvas());
    l->setPoints(0, 0, x, canvas()->height());
    l->show();
    canvas()->update();*/
}

void KPTChartCanvasView::drawBar(KPTChartDataSetItem *data, KPTTimeScale *x, KPTNumberScale *y) {
    int x1 = (int)x->posX(data->x1());
    int y1 = (int)y->posY(data->y1());
    int x2 = (int)x->posX(data->x2());
    int y2 = (int)y->posY(data->y2());
    QPen *pen = data->pen();
    QBrush *brush = data->brush();
    //kdDebug()<<k_funcinfo<<"x1="<<x1<<" y1="<<y1<<" x2="<<x2<<" y2="<<y2<<endl;
    
    QCanvasRectangle *r = new QCanvasRectangle(x1, y1, x2-x1, y2-y1, canvas());
    r->setZ(data->prio());
    r->setPen(pen ? *pen : QPen(QColor(black)));
    r->setBrush(brush ? QBrush(*brush) : QBrush(QColor(green)));
    r->show();
}

void KPTChartCanvasView::drawHorisontalLine(KPTChartDataSetItem *data, KPTTimeScale *x, KPTNumberScale *y) {
    int y1 = (int)y->posY(data->y1());
    QPen *pen = data->pen();
    QCanvasLine *l = new QCanvasLine(canvas());
    l->setZ(data->prio());
    l->setPen(pen ? QPen(*pen) : QPen(QColor(black)));
    l->setPoints(0, y1, canvas()->width(), y1);
    l->show();
}

void KPTChartCanvasView::drawLine(KPTChartDataSetItem *data, KPTTimeScale *x, KPTNumberScale *y) {
    int x1 = (int)x->posX(data->x1());
    int y1 = (int)y->posY(data->y1());
    int x2 = (int)x->posX(data->x2());
    int y2 = (int)y->posY(data->y2());
    QPen *pen = data->pen();
    QCanvasLine *l = new QCanvasLine(canvas());
    l->setZ(data->prio());
    l->setPen(pen ? QPen(*pen) : QPen(QColor(black)));
    l->setPoints(x1, y1, x2, y2);
    l->show();
    
    //kdDebug()<<k_funcinfo<<" at "<<data->x1().toString()<<","<<data->y1()<<" to "<<data->x2().toString()<<","<<data->y2()<<endl;
}

void KPTChartCanvasView::canvasMoved() {
    drawDescription();
    canvas()->update();
}

void KPTChartCanvasView::drawDescription() {
    if (!m_description)
        return;
    
    int w = QMAX(30, QMIN(visibleWidth(), canvas()->size().width()));
    m_description->move(contentsX()+(w/10), contentsY()+2);
    m_description->show();
}

void KPTChartCanvasView::drawDescription(const QString &desc) {
    if (!m_description) {
        m_description = new QCanvasText(canvas());
        m_description->setZ(1000.0);
    }
    m_description->setText(desc);
    drawDescription();
}

void KPTChartCanvasView::contentsContextMenuEvent(QContextMenuEvent *e) {
    kdDebug()<<k_funcinfo<<endl;
    emit contextMenuRequest(e->pos());
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
