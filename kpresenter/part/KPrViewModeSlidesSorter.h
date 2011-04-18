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

#include <QListView>
#include <KoPAViewMode.h>
#include <KoZoomMode.h>

class KoPAView;
class KoPACanvas;
class KoPAPageBase;
class KPrSlidesSorterDocumentModel;

class KPrViewModeSlidesSorter : public KoPAViewMode
{
    Q_OBJECT
public:
    KPrViewModeSlidesSorter(KoPAView *view, KoPACanvas *canvas);
    ~KPrViewModeSlidesSorter();

    void paint(KoPACanvasBase* canvas, QPainter& painter, const QRectF &paintRect);
    void paintEvent(KoPACanvas * canvas, QPaintEvent* event);
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
    void updateDocumentModel();
    void activateNormalViewMode();
    void updateToActivePageIndex();

    void addShape( KoShape *shape );
    void removeShape( KoShape *shape );
    QList<KoPAPageBase*> extractSelectedSlides();

    /**
     * The icon size
     *
     * @return the icon size defined before
     */
    QSize iconSize() const;

protected:

    /**
     * Fills the editor with presentation slides and ordored them in the KPrSlidesSorter
     */
    void populate();

    /**
     * Moves a page from pageNumber to pageAfterNumber
     *
     * @param slides a list with pages to move
     * @param pageAfterNumber the number of the place the page should move to
     */
    void movePages( const QList<KoPAPageBase *> &slides, int pageAfterNumber );

    /**
     * The count of the page
     *
     * @return the count of the page
     */
    int pageCount() const;

    /**
     * The rect of an items, essentialy used to have the size of the full icon
     *
     * @return the rect of the item
     */
    QRect itemSize() const;

    /**
     * Setter of the size with a rect
     *
     * @param size which is a QRect
     */
    void setItemSize(QRect size);

    /**
     * Permit to know if a slide is draging
     *
     * @return boolean
     */
    bool isDraging() const;

    /**
     * Setter for the draging flag
     *
     * @param flag boolean
     */
    void setDragingFlag(bool flag = true);

    /**
     * Return the last item number it were on
     *
     * @return the last item number it was on
     */
    int lastItemNumber() const;

    /**
     * Setter of the last item number it were on
     *
     * @param number of the item number it is on
     */
    void setLastItemNumber(int number);

    /**
     * Setter of the icon size
     *
     * @param size which is a QSize
     */
    void setIconSize(QSize size);

    /**
     * This class manage the QListWidget itself.
     * Use all the getters and setters of the KPrViewModeSlidesSorter.
     * Most of the functions are Qt overrides to have the wished comportment.
     */
    class KPrSlidesSorter : public QListView {
        public:
            KPrSlidesSorter (KPrViewModeSlidesSorter * viewModeSlidesSorter, QWidget * parent = 0)
                : QListView(parent)
                , m_viewModeSlidesSorter(viewModeSlidesSorter)
            {
                setViewMode(QListView::IconMode);
                setFlow(QListView::LeftToRight);
                setWrapping(TRUE);
                setResizeMode(QListView::Adjust);
                setDragEnabled(true);
                setAcceptDrops(true);
                setDropIndicatorShown(true);
            };

            ~KPrSlidesSorter();

            virtual void paintEvent ( QPaintEvent * ev);

            virtual void mouseDoubleClickEvent(QMouseEvent *event);

            virtual void contextMenuEvent(QContextMenuEvent *event);

            virtual void keyPressEvent(QKeyEvent *event);

            virtual void startDrag ( Qt::DropActions supportedActions );

            virtual void dropEvent(QDropEvent* ev);

            virtual void dragMoveEvent(QDragMoveEvent* ev);

            virtual void dragEnterEvent(QDragEnterEvent *event);

            int pageBefore(QPoint point);

        private:
            KPrViewModeSlidesSorter * m_viewModeSlidesSorter;
    };

private:
    KPrSlidesSorter * m_slidesSorter;
    KPrSlidesSorterDocumentModel * m_documentModel;
    QSize m_iconSize;
    QRect m_itemSize;
    bool m_sortNeeded;
    const int m_pageCount;
    bool m_dragingFlag;
    int m_lastItemNumber;

private slots:
    void updateDocumentDock();
    void updateModel();
    void updatePageAdded();
    void itemClicked(const QModelIndex);
    void deleteSlide();
    void addSlide();
    void editCut();
    void editCopy();
    void editPaste();
    void updateZoom(KoZoomMode::Mode mode, qreal zoom);

signals:
    void pageChanged(KoPAPageBase *page);
};

#endif // KPRVIEWMODESLIDESSORTER_H
