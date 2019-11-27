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
class KoViewItemContextBar;
class KPrSlidesSorterDocumentModel;
class KPrSlidesManagerView;
class KPrCustomSlideShowsModel;
class KPrCustomSlideShows;
class QToolButton;
class QComboBox;

/**
  * This view mode holds Slides Sorter view widget and
  * holds view and toolbar to manage custom slide shows
  * This class also manages all interaction between
  * the standard GUI and the slides sorter view and between
  * slides sorter view - custom slide shows view
  */
class KPrViewModeSlidesSorter : public KoPAViewMode
{
    Q_OBJECT
public:
    KPrViewModeSlidesSorter(KoPAView *view, KoPACanvasBase *canvas);
    ~KPrViewModeSlidesSorter() override;

    void paint(KoPACanvasBase *canvas, QPainter &painter, const QRectF &paintRect) override;
    void paintEvent(KoPACanvas *canvas, QPaintEvent *event);
    void tabletEvent(QTabletEvent *event, const QPointF &point) override;
    void mousePressEvent(QMouseEvent *event, const QPointF &point) override;
    void mouseDoubleClickEvent(QMouseEvent *event, const QPointF &point) override;
    void mouseMoveEvent(QMouseEvent *event, const QPointF &point) override;
    void mouseReleaseEvent(QMouseEvent *event, const QPointF &point) override;
    void shortcutOverrideEvent(QKeyEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event, const QPointF &point) override;

    void activate(KoPAViewMode *previousViewMode) override;
    void deactivate() override;

    /**
     * Change the KPrView active page
     * @param page pointer to the new active page
     */
    void updateActivePage(KoPAPageBase *page) override;

    void addShape(KoShape *shape) override;
    void removeShape(KoShape *shape) override;

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

    /**
     * Clear selection and select a list of slides on slides sorter view
     * @param slides list to be selected
     */
    void selectSlides(const QList<KoPAPageBase *> &slides);

    /** Set active custom slide show */
    void setActiveCustomSlideShow(int index);

protected:

    /**
     * Setter of the icon size
     *
     * @param size which is a QSize
     */
    void setIconSize(const QSize &size);

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
    KPrSlidesManagerView *m_slidesSorterView;
    KPrSlidesManagerView *m_customSlideShowView;
    KPrSlidesSorterDocumentModel *m_slidesSorterModel;
    int m_zoom;
    QWidget *m_centralWidget;
    KPrCustomSlideShowsModel *m_customSlideShowModel;
    QSize m_iconSize;
    bool m_editCustomSlideShow;
    QToolButton *m_buttonAddCustomSlideShow;
    QToolButton *m_buttonDelCustomSlideShow;
    QToolButton *m_buttonAddSlideToCurrentShow;
    QToolButton *m_buttonDelSlideFromCurrentShow;
    QComboBox *m_customSlideShowsList;
    KoViewItemContextBar *m_slidesSorterItemContextBar;

private Q_SLOTS:
    /** Changes the view active page to match the slides sorter current index*/
    void updateActivePageToCurrentIndex();

    /** Updates custom slide Shows list */
    void updateCustomSlideShowsList();

    /** Changes the view active page to match the slides sorter item selected*/
    void itemClicked(const QModelIndex);

    /** delete the current selected slides*/
    void deleteSlide();

    /** add a new slide after the current active page*/
    void addSlide();

    /** Item context bar actions */
    void contextBarDuplicateSlide();
    void contextBarDeleteSlide();
    void contextBarStartSlideshow();

    /** Rename current slide on Slides Sorter View */
    void renameCurrentSlide();

    /** cut the current selected slides*/
    void editCut();

    /** copy the current selected slides*/
    void editCopy();

    /** paste slides in slides sorter view*/
    void editPaste();

    /** update the zoom of the Slides Sorter view*/
    void updateZoom(KoZoomMode::Mode mode, qreal zoom);

    /** Changes the slides sorter current index to match view active page*/
    void updateToActivePageIndex();

    /** Hide KPrViewModeSlidesSorter and return to normal view*/
    void activateNormalViewMode();

    /** Provides a custom context menu for the slides sorter view*/
    void slidesSorterContextMenu(QContextMenuEvent *event);

    /** Provides a custom context menu for the slides sorter view*/
    void customSlideShowsContextMenu(QContextMenuEvent *event);

    /** Updates the UI according to the custom Show selected */
    void customShowChanged(int showNumber);

    /** Delete selected slides from the current custom slide show */
    void deleteSlidesFromCustomShow();

    /** Add slides selected on Slides Sorter view to the current custom slide show */
    void addSlideToCustomShow();

    /** Add a new slides custom Show */
    void addCustomSlideShow();

    /** Remove a new slides custom Show */
    void removeCustomSlideShow();

    /** Renames current custom slide show */
    void renameCustomSlideShow();

    /** Enable standard edit actions for Slides Sorter View */
    void enableEditActions();

    /** Disable standard edit actions for Slides Sorter View */
    void disableEditActions();

    /** Enable edit buttons for Custom Slide Shows List*/
    void enableEditCustomShowButtons();

    /** Disable edit buttons for Custom Slide Shows View and List*/
    void disableEditCustomShowButtons();

    /** Enable/Disable add and remove slides buttons for Custom Slide Show View */
    void manageAddRemoveSlidesButtons();

    /** Select pages on custom slide show view */
    void selectCustomShowPages(int start, int count);
};

#endif // KPRVIEWMODESLIDESSORTER_H
