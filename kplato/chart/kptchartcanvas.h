/* This file is part of the KDE project
   Copyright (C) 2005 Dag Andersen <danders@get2net.dk>
   Copyright (C) 2005 Raphael Langerhorst <raphael.langerhorst@kdemail.net>

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

template<class type>
class KPTCanvasValueList : public QValueList<type*> {
    public:
        KPTCanvasValueList() {}
        void hide() {
            while (!this->isEmpty())
                hide(this->first());
        }
        void hide(type *item) {
            if (item->isVisible()) {
                item->hide();
                m_hidden.append(item);
                remove(item);
            }
        }
        void show(type *item) {
            if (!item->isVisible()) {
                item->show();
                append(item);
                m_hidden.remove(item);
            }
        }
        type *getItem(QCanvas *canvas=0) {
            type *item = NULL;
            if (m_hidden.isEmpty()) {
                item = new type(canvas);
                m_hidden.append(item);
            }
            else
            {
                item = m_hidden.first();
            }
            Q_ASSERT(item);
            return item;
        }
    private:
        QValueList<type*> m_hidden;
};

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
    QCanvasText *m_description;
    KPTCanvasValueList<QCanvasLine> m_xGridLines;
    KPTCanvasValueList<QCanvasLine> m_yGridLines;
    KPTCanvasValueList<QCanvasLine> m_horisontalLines;
    QCanvasLine *m_yZeroLine;

    KPTCanvasValueList<QCanvasLine> m_lines;
    KPTCanvasValueList<QCanvasRectangle> m_bars;
};

} // KPlato namespace

#endif //KPTCHARTCANVAS_H
