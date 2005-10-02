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

#include "kptnumberscale.h"

#include <kdebug.h>

#include <qpopupmenu.h>
#include <qpainter.h>
#include <qdrawutil.h>
#include <qsize.h>
#include <qstringlist.h>

namespace KPlato
{

KPTNumberScale::KPTNumberScale(QWidget *parent, bool left, int offsetTop, int offsetBottom)
    : QCanvasView(parent),
      m_rangeMin(0.0),
      m_rangeMax(100.0),
      m_prec(0),
      m_min(0),
      m_max(0),
      m_left(left),
      m_offsetTop(offsetTop),
      m_offsetBottom(offsetBottom),
      m_canvasWidth(0)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    setHScrollBarMode(QScrollView::AlwaysOff);
    setVScrollBarMode(QScrollView::AlwaysOff);

    KPTNumberScaleCanvas *can = new KPTNumberScaleCanvas(this);
    setCanvas(can);
    
    connect(can, SIGNAL(resize(QSize)), SLOT(canvasResized(QSize)));

}
KPTNumberScale::~KPTNumberScale()
{
    delete canvas();
}

KPTNumberScaleCanvas *KPTNumberScale::theCanvas() {
    return static_cast<KPTNumberScaleCanvas *>(canvas());
}

void KPTNumberScale::setUnit(const QString &unit) {
    theCanvas()->setUnit(unit);
}

void KPTNumberScale::canvasResized(QSize size) {
    //kdDebug()<<k_funcinfo<<endl;
    resize(size.width(), size.height());
}

double KPTNumberScale::posY(double value) {
    KPTNumberScaleCanvas *can = theCanvas();
    if (value <= m_min)
        return can->rangeMin();
    if (value >= m_max)
        return can->rangeMax();
        
    double r = can->rangeMin() - can->rangeMax();
    //kdDebug()<<k_funcinfo<<"value="<<value<<" r="<<r<<" max, min="<<m_max<<", "<<m_min<<endl;
    return can->rangeMin() - (r/((m_max-m_min)/value));
}

double KPTNumberScale::power10(int p) {
    int r = 1;
    if (p >= 0) {
        for (int i=0; i < p; i++) {
            r *= 10;
        }
    } else {
        int r = 1;
        for (int i=0; i < p; i++) {
            r /= 10;
        }
    }
    return r;
}

void KPTNumberScale::draw() {
    //kdDebug()<<k_funcinfo<<"min="<<m_rangeMin<<" max="<<m_rangeMax<<"  prec="<<m_prec<<endl;
    KPTNumberScaleCanvas *can = theCanvas();
    int numInter = can->scaleIntervals(20);
    if (numInter == 0) {
        kdDebug()<<k_funcinfo<<"No room to make a y-scale"<<endl;
        return;
    }
    m_intervals.clear();
    double range = m_rangeMax - m_rangeMin;
    QStringList l;
    if (m_step == 0.0) {
        int ri = (int)(range*power10(m_prec));
        double rest = (ri%(numInter-1))/power10(m_prec);
        double unitsPrInterval = (range-rest)/(numInter-1);
        //kdDebug()<<k_funcinfo<<"range="<<range<<" rest="<<rest<<" numInter="<<numInter<<" unitsPrInterval="<<unitsPrInterval<<endl;
        double v = m_rangeMin;
        for (int i = 0; i <= numInter; i++) {
            //kdDebug()<<k_funcinfo<<v<<endl;
            l.append(QString().setNum(v, 'f', m_prec));
            m_intervals.append(v);
            m_max = v;
            v += unitsPrInterval;
        }
    } else {
        double step = m_step;
        while ((range/step) > numInter) {
            step *= 2;
        }
        for (double v = m_rangeMin; v <= m_rangeMax; v += step) {
            //kdDebug()<<k_funcinfo<<v<<endl;
            l.append(QString().setNum(v, 'f', m_prec));
            m_intervals.append(v);
            m_max = v;
        }
    }
    m_min = m_rangeMin;
    can->draw(l);
}

void KPTNumberScale::resize(int w, int h) {
    //kdDebug()<<k_funcinfo<<"size: "<<w<<"x"<<h<<endl;
    int hei = height();
    QCanvasView::resize(w, h);
    if (h != hei)
        draw();
}

void KPTNumberScale::resizeEvent(QResizeEvent *) {
    //kdDebug()<<k_funcinfo<<contentsWidth()<<"x"<<contentsHeight()<<endl;
    canvas()->resize(width(), height());
}


/********************************************/
KPTNumberScaleCanvas::KPTNumberScaleCanvas(QObject *parent, const char *name)
    : QCanvas(parent, name),
      m_rangeMin(0),
      m_rangeMax(0),
      m_offsetTop(10),
      m_offsetBottom(10)
{
}

KPTNumberScaleCanvas::~KPTNumberScaleCanvas() {
    clear();
}

int KPTNumberScaleCanvas::scaleHeight() {
    return QMAX(0, height() - m_offsetTop - m_offsetBottom);
}

int KPTNumberScaleCanvas::scaleIntervals(int minPixels) {
    if (minPixels <= 0)
        return 0;
    return scaleHeight()/minPixels;
}

void KPTNumberScaleCanvas::repaintEvent(QPaintEvent *) {
    //kdDebug()<<k_funcinfo<<endl;
    //draw();
}

void KPTNumberScaleCanvas::clear() {
    QCanvasItemList list = allItems();
    QCanvasItemList::iterator it;
    for ( it = list.begin(); it != list.end(); ++it ) {
        delete (*it);
    }
}

void KPTNumberScaleCanvas::setValues(QStringList l) {
    m_values.clear();
    m_values = l;
}

void KPTNumberScaleCanvas::draw() {
    //kdDebug()<<k_funcinfo<<endl;
    clear();
    QCanvasLine *l;
    int x = 3;
    int y = 0;
    QCanvasText *unit = 0;
    if (!m_unit.isEmpty()) {
        unit = new QCanvasText(m_unit, this);
        unit->move(x, 2);
        unit->show();
        int rx = unit->boundingRect().width();
        y = unit->boundingRect().height();
        if (x < rx)
            x += rx;
        //kdDebug()<<k_funcinfo<<"Unit "<<m_unit<<" at: "<<x<<",2"<<"("<<rx<<"x"<<y<<")"<<endl;
    }
    m_rangeMax = m_offsetTop + y;
    m_rangeMin = height() - m_offsetBottom;
    if (m_values.count() > 0) {
        QCanvasText *t;
        QPtrList<QCanvasText> list;
        double step = (m_rangeMin - m_rangeMax)/(m_values.count()-1);
        double pos = m_rangeMin;
        //kdDebug()<<k_funcinfo<<"max="<<m_rangeMax<<" min="<<m_rangeMin<<" step="<<step<<endl;
        for (QStringList::Iterator it = m_values.begin(); it != m_values.end(); ++it ) {
            //kdDebug()<<k_funcinfo<<(*it)<<": pos="<<pos<<endl;
            t = new QCanvasText((*it), this);
            list.append(t);
            int rx = t->boundingRect().width();
            t->move(3, pos-(t->boundingRect().height()/2));
            t->show();
            if (x < rx)
                x = rx;
            pos -= step;
        }
        x += 3;
        pos = m_rangeMin;
        for (int i = 0; i < m_values.count(); i++) {
            m_rangeMax = pos;
            l = new QCanvasLine(this);
            l->setPoints(x, (int)pos, x+3, (int)pos);
            l->show();
            t = list.at(i);
            t->move(x-t->boundingRect().width(),t->boundingRect().top());
            pos -= step;
        }
    } else {
        l = new QCanvasLine(this);
        l->setPoints(x, (int)m_rangeMax, x+3, (int)m_rangeMax);
        l->show();
        l = new QCanvasLine(this);
        l->setPoints(x, (int)m_rangeMin, x+3, (int)m_rangeMin);
        l->show();
    }
    l = new QCanvasLine(this);
    l->setPoints(x, (int)m_rangeMin, x, (int)m_rangeMax);
    l->show();
    if (unit) {
        unit->move(x-unit->boundingRect().width(), 2); // right adjusted
    }
    emit resize(QSize(x+5, height()));
    update();
    //kdDebug()<<k_funcinfo<<"range="<<m_rangeMax<<","<<m_rangeMin<<endl;
}

void KPTNumberScaleCanvas::draw(QStringList l) {
    setValues(l);
    draw();
}

} // KPlato namespace

#include "kptnumberscale.moc"
