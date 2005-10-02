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


#ifndef KPTNUMBERSCALE_H
#define KPTNUMBERSCALE_H

#include <qcanvas.h>
#include <qstring.h>

#include <math.h>

class QCanvasRectangle;
class QStringList;

namespace KPlato
{

class KPTNumberScaleCanvas;

class KPTNumberScale : public QCanvasView
{
   Q_OBJECT
public:
    KPTNumberScale(QWidget *parent, bool left=true, int offsetTop=0, int offsetBottom=0);
    ~KPTNumberScale();
    
    virtual void resize(int w, int h);
    KPTNumberScaleCanvas *theCanvas();
    double posY(double value);
    void setRange(double min, double max, double step=0.0)
        { m_rangeMin = min; m_rangeMax = max; m_step = step; }
    double rangeMin() const { return m_rangeMin; }
    double rangeMax() const { return m_rangeMax; }
    void setUnit(const QString &unit);
    void setPrecission(unsigned int prec=0) { m_prec = prec; }
    const QValueList<double> &intervals() const { return m_intervals; }
    
    void draw();
    
public slots:
    void canvasResized(QSize);
    
protected:
    virtual void resizeEvent(QResizeEvent *);
    
private:
    double power10(int p);
    
private:
    double m_rangeMin;
    double m_rangeMax;
    int m_prec;
    
    double m_min;
    double m_max;
    double m_step;
    bool m_left;
    int m_offsetTop;
    int m_offsetBottom;
    int m_canvasWidth;

    QValueList<double> m_intervals;
};

class KPTNumberScaleCanvas : public QCanvas
{
    Q_OBJECT
public:
    KPTNumberScaleCanvas(QObject * parent = 0, const char * name = 0);
    ~KPTNumberScaleCanvas();
    
    void clear();
    
    double rangeMin() { return m_rangeMin; }
    double rangeMax() { return m_rangeMax; }
    int offsetTop() { return m_offsetTop; }
    void setUnit(const QString &unit) { m_unit = unit; }
    
    int scaleHeight();
    int scaleIntervals(int minPixels=20);
    void setValues(QStringList l);
    void draw();
    void draw(QStringList l);

signals:
    void resize(QSize);
    
protected:
    virtual void repaintEvent(QPaintEvent *e);
    
private:
    double m_rangeMin;
    double m_rangeMax;
    int m_offsetTop;
    int m_offsetBottom;
    QStringList m_values;
    QString m_unit;
    
};

} // KPlato namespace

#endif //KPTNUMBERSCALE_H
