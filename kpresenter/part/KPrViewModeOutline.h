/* This file is part of the KDE project
   Copyright (C) 2011 Benjamin Port <port.benjamin@gmail.com>
   Copyright (C) 2011 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/


#ifndef KPRVIEWMODEOUTLINE_H
#define KPRVIEWMODEOUTLINE_H

#include <QTextBlockUserData>

#include "KoPAViewMode.h"
#include "pagelayout/KPrPlaceholders.h"

class KoPAView;
class KoPACanvas;
class KPrOutlineEditor;

class KPrViewModeOutline : public KoPAViewMode
{
    Q_OBJECT
public:
    KPrViewModeOutline(KoPAView *view, KoPACanvas *canvas);

    virtual void paint(KoPACanvasBase* canvas, QPainter& painter, const QRectF &paintRect);
    void tabletEvent(QTabletEvent *event, const QPointF &point);
    void mousePressEvent(QMouseEvent *event, const QPointF &point);
    void mouseDoubleClickEvent(QMouseEvent *event, const QPointF &point);
    void mouseMoveEvent(QMouseEvent *event, const QPointF &point);
    void mouseReleaseEvent(QMouseEvent *event, const QPointF &point);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent * event, const QPointF &point);

    void activate(KoPAViewMode *previousViewMode);
    void deactivate();
    void populate();

protected:
    /**
      * Class to manage block data
      */
    class SlideUserBlockData : public QTextBlockUserData
    {
    public:
        SlideUserBlockData(int pageNumber, OutlinePair outlinePair)
            : m_pageNumber(pageNumber), m_outlinePair(outlinePair)
        {}

        ~SlideUserBlockData() {}
        int pageNumber() { return m_pageNumber; }
        OutlinePair outlinePair() { return m_outlinePair; }
    private:
        int m_pageNumber;
        OutlinePair m_outlinePair;
    };

protected slots:
    void synchronize(int position, int charsRemoved, int charsAdded);
    void slotSelectionChanged();
private:
    KPrOutlineEditor *m_outlineEditor;
};

#endif // KPRVIEWMODEOUTLINE_H
