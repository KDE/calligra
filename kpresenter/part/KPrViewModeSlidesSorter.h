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
class KPrSlidesManagerView;
class KPrSelectionManager;

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

    void addShape( KoShape *shape );
    void removeShape( KoShape *shape );
    QList<KoPAPageBase*> extractSelectedSlides();

    /**
     * The icon size
     *
     * @return the icon size defined before
     */
    QSize iconSize() const;

    bool eventFilter(QObject *watched, QEvent *event);

protected:

    /**
     * Fills the editor with presentation slides and ordored them in the KPrSlidesSorter
     */
    void populate();

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
     * load the last zoom value used
     */
    void loadZoomConfig();

    /**
     * Setter of the zoom value
     *
     * @param zoom percent
     */
    void setZoom(int zoom);

    /**
     * Return the last zoom stored
     *
     * @return the last zoom stored
     */
    int zoom();

    /**
      * save zoom value
      */
    void saveZoomConfig(int zoom);

private:
    KPrSlidesManagerView * m_slidesSorter;
    KPrSlidesSorterDocumentModel * m_documentModel;
    QSize m_iconSize;
    QRect m_itemSize;
    bool m_sortNeeded;
    const int m_pageCount;
    bool m_dragingFlag;
    int m_lastItemNumber;
    int m_zoom;
    KPrSelectionManager *m_selectionManagerSlidesSorter;
public slots:
    void editPaste();

private slots:
    void updateDocumentDock();
    void updateModel();
    void itemClicked(const QModelIndex);
    void deleteSlide();
    void addSlide();
    void editCut();
    void editCopy();
    void updateZoom(KoZoomMode::Mode mode, qreal zoom);
    void updateToActivePageIndex();
    void activateNormalViewMode();
    void slidesSorterContextMenu(QContextMenuEvent* event);

signals:
    void pageChanged(KoPAPageBase *page);
};

#endif // KPRVIEWMODESLIDESSORTER_H
