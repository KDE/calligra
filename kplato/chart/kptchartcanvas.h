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


#ifndef KPTCHARTCANVAS_H
#define KPTCHARTCANVAS_H

#include <qcanvas.h>
#include <qstring.h>
#include <qvaluelist.h>

#include <math.h>

class QCanvasRectangle;
class QCanvasText;
class QStringList;

namespace KPlato
{

class KPTChartDataSetItem;
class KPTTimeScale;
class KPTNumberScale;

class KPTChartCanvasView : public QCanvasView
{
   Q_OBJECT
public:
    KPTChartCanvasView(QWidget *parent, const char * name = 0);
    ~KPTChartCanvasView();

    void canvasUpdate();
    void clear();
    void drawYZeroLine(KPTNumberScale *scale, double zero);
    void drawYGrid(KPTNumberScale *scale, double step=0.0);
    void drawXMajorGrid(KPTTimeScale *scale);
    void drawXMinorGrid(KPTTimeScale *scale);
    void drawDescription(const QString &desc);
    
    void drawBar(KPTChartDataSetItem *data, KPTTimeScale *x, KPTNumberScale *y);
    void drawLine(KPTChartDataSetItem *data, KPTTimeScale *x, KPTNumberScale *y);
    
    class Canvas : public QCanvas
    {
    public:
        Canvas(QObject * parent = 0, const char * name = 0);
        ~Canvas();
        
        void clear();
        
    };

protected slots:
    void drawDescription();
    void canvasMoved();
    
protected:
    KPTChartCanvasView::Canvas *chartCanvas();
    void drawYGrid(KPTNumberScale *scale, const QValueList<double> &list);

private:
    QCanvasText *m_description;
};

} // KPlato namespace

#endif //KPTCHARTCANVAS_H
