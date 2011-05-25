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

//This view mode holds Slides Sorter view widget and
//TODO: holds view and toolbar to manage custom slides shows
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

    /**
     * Change the KPrView active page
     * @param page pointer to the new active page
     */
    void updateActivePage( KoPAPageBase *page );

    void addShape( KoShape *shape );
    void removeShape( KoShape *shape );

    /**
      * Return a list with the pointer to the selected slides
      *
      * @return a list of KoPAPageBase pointers to the slides sorter selected slides.
      */
    QList<KoPAPageBase*> extractSelectedSlides();

    /**
     * The icon size
     *
     * @return the icon size defined before
     */
    QSize iconSize() const;

    //Add a delete key feature to the slides sorter view
    bool eventFilter(QObject *watched, QEvent *event);

protected:

    /**
     * Fills the editor with presentation slides and ordored them in the KPrSlidesSorter
     */
    void populate();

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
    int m_zoom;
    KPrSelectionManager *m_selectionManagerSlidesSorter;

public slots:
    void editPaste();

private slots:
    /** Changes the view active page to match the slides sorter current index*/
    void updateActivePageToCurrentIndex();

    /** Update the slides sorter document model*/
    void updateSlidesSorterDocumentModel();

    /** Changes the view active page to match the slides sorter item selected*/
    void itemClicked(const QModelIndex);

    /** delete the current selected slides*/
    void deleteSlide();

    /** add a new slide after the current active page*/
    void addSlide();

    /** cut the current selected slides*/
    void editCut();

    /** copy the current selected slides*/
    void editCopy();

    /** update the zoom of the Slides Sorter view*/
    void updateZoom(KoZoomMode::Mode mode, qreal zoom);

    /** Changes the slides sorter current index to match view active page*/
    void updateToActivePageIndex();

    /** Hide KPrViewModeSlidesSorter and return to normal view*/
    void activateNormalViewMode();

    /** Provides a custom context menu for the slides sorter view*/
    void slidesSorterContextMenu(QContextMenuEvent* event);
};

#endif // KPRVIEWMODESLIDESSORTER_H
