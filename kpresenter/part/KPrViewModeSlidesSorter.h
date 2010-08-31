/* This file is part of the KDE project
*
* Copyright (C) 2010 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Library General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Library General Public License for more details.
*
* You should have received a copy of the GNU Library General Public License
* along with this library; see the file COPYING.LIB.  If not, write to
* the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301, USA.
*/

#ifndef KPRVIEWMODESLIDESSORTER_H
#define KPRVIEWMODESLIDESSORTER_H

#include <QListWidget>
#include <QSize>

#include <KoPageApp.h>
#include <KoPAViewMode.h>

class KoPAView;
class KoPACanvas;
class KoPAPageBase;

class KPrViewModeSlidesSorter : public KoPAViewMode
{
    Q_OBJECT
public:
    KPrViewModeSlidesSorter(KoPAView *view, KoPACanvas *canvas);
    ~KPrViewModeSlidesSorter();

    void paint(KoPACanvasBase* canvas, QPainter& painter, const QRectF &paintRect);
    void paintEvent( KoPACanvas * canvas, QPaintEvent* event );
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

    void updateActivePage( KoPAPageBase *page );

    void addShape( KoShape *shape );
    void removeShape( KoShape *shape );

protected:

    /**
    * Fills the editor with presentation slides and ordored them in the KPrSlidesSorter
    */
    void populate();

    void movePage( int pageNumber, int pageAfterNumber );

    int pageCount();

    QSize iconSize();

    QRect itemSize();

    void setItemSize(QRect size);

    bool isDraging();

    void setDragingFlag(bool flag = true);

    int lastItemNumber();

    void setLastItemNumber(int number);

    class KPrSlidesSorter : public QListWidget {
        public:
            KPrSlidesSorter ( KPrViewModeSlidesSorter * viewModeSlidesSorter, QWidget * parent = 0 )
                : QListWidget(parent)
                , m_viewModeSlidesSorter(viewModeSlidesSorter)
            {
                setViewMode(QListView::IconMode);
                setResizeMode(QListView::Adjust);
                setDragDropMode(QAbstractItemView::DragDrop);
            };
            ~KPrSlidesSorter(){};

            virtual Qt::DropActions supportedDropActions() const
            {
                return Qt::MoveAction;
            }

            virtual void paintEvent ( QPaintEvent * ev);

            virtual void startDrag ( Qt::DropActions supportedActions );

            // Skip internal dnd handling in QListWidget ---- how is one supposed to figure this out
            // without reading the QListWidget code !?
            virtual void dropEvent(QDropEvent* ev);

            virtual void dragMoveEvent(QDragMoveEvent* ev);

            virtual QStringList mimeTypes() const;

            virtual QMimeData* mimeData(const QList<QListWidgetItem*> items) const;

            int pageBefore(QPoint point);

        private:
            KPrViewModeSlidesSorter * m_viewModeSlidesSorter;
    };

private:
    KPrSlidesSorter * m_slidesSorter;
    QSize m_iconSize;
    QRect m_itemSize;
    bool m_sortNeeded;
    const int m_pageCount;
    bool m_dragingFlag;
    int m_lastItemNumber;
};

#endif // KPRVIEWMODESLIDESSORTER_H
