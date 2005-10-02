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


#ifndef KPTCHARTCANVAS_H
#define KPTCHARTCANVAS_H

#include <qcanvas.h>
#include <qstring.h>
#include <qvaluelist.h>
#include <qptrlist.h>

#include <math.h>

class QCanvasRectangle;
class QCanvasText;
class QStringList;
class QPoint;

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
    void drawXMajorGrid(KPTTimeScale *x, KPTNumberScale *y);
    void drawXMinorGrid(KPTTimeScale *scale);
    void drawDescription(const QString &desc);
    
    void drawBar(KPTChartDataSetItem *data, KPTTimeScale *x, KPTNumberScale *y);
    void drawHorisontalLine(KPTChartDataSetItem *data, KPTTimeScale *x, KPTNumberScale *y);
    void drawLine(KPTChartDataSetItem *data, KPTTimeScale *x, KPTNumberScale *y);
    
    class Canvas : public QCanvas
    {
    public:
        Canvas(QObject * parent = 0, const char * name = 0);
        ~Canvas();
        
        void clear();
        
    };

signals:
    void contextMenuRequest(const QPoint &pos);
    
protected slots:
    void drawDescription();
    void canvasMoved();
    
protected:
    KPTChartCanvasView::Canvas *chartCanvas();
    void drawYGrid(KPTNumberScale *scale, const QValueList<double> &list);
    virtual void contentsContextMenuEvent(QContextMenuEvent *e);

private:
    template<class type>
    class CanvasPtrList : public QPtrList<type> {
        public:
            CanvasPtrList() {}
            void hide() {
                while (this->count() > 0)
                    hide(this->getFirst());
            }
            void hide(type *item) {
                if (item->isVisible()) {
                    item->hide();
                    m_hidden.append(item);
                    removeRef(item);
                }
            }
            void show(type *item) {
                if (!item->isVisible()) {
                    item->show();
                    append(item);
                    m_hidden.removeRef(item);
                }
            }
            type *getItem(QCanvas *canvas=0) {
                type *item = m_hidden.getFirst();
                if (item == 0) {
                    item = new type(canvas);
                    m_hidden.append(item);
                }
                return item;  
            }
        private:
            QPtrList<type> m_hidden;    
    };

    QCanvasText *m_description;
    CanvasPtrList<QCanvasLine> m_xGridLines;
    CanvasPtrList<QCanvasLine> m_yGridLines;
    CanvasPtrList<QCanvasLine> m_horisontalLines;
    QCanvasLine *m_yZeroLine;

    CanvasPtrList<QCanvasLine> m_lines;
    CanvasPtrList<QCanvasRectangle> m_bars;
};

} // KPlato namespace

#endif //KPTCHARTCANVAS_H
