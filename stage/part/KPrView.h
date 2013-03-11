/* This file is part of the KDE project
   Copyright (C) 2006-2007 Thorsten Zachmann <zachmann@kde.org>
   Copyright (C) 2009-2010 Benjamin Port <port.benjamin@gmail.com>

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

#ifndef KPRVIEW_H
#define KPRVIEW_H

#include "stage_export.h"
#include <QObject>
#include <KoPAView.h>

class KPrDocument;
class KPrPart;
class KPrViewAdaptor;
class KPrViewModeNotes;
class KPrViewModeSlidesSorter;
class KPrViewModePresentation;
class KPrViewModePresenterView;
class KActionMenu;
class KoPAPageBase;

class STAGE_EXPORT KPrView : public KoPAView
{
    Q_OBJECT
    friend class KPrConfigureSlideShowDialog;
public:
    explicit KPrView(KPrPart *part, KPrDocument *document, QWidget *parent = 0);
    ~KPrView();

    using KoPAViewBase::viewConverter;
    virtual KoViewConverter * viewConverter( KoPACanvasBase * canvas);

    /**
     * Get the document object the view was initialised with
     */
    KPrDocument * kprDocument() const;

    /**
     * Get the view's dbus adaptor
     */
    virtual KPrViewAdaptor * dbusObject() const;

    /**
     * Get the presentation view mode
     */
    KPrViewModePresentation * presentationMode() const;

    /**
     * Get the slides sorter view mode
     */
    KPrViewModeSlidesSorter *slidesSorter() const;

    /**
     * Find whether the presentation view mode is active
     */
    bool isPresentationRunning() const;

    /**
      * Load zoom configuration
      */
    void initZoomConfig();

    /**
      * Restore zoom configuration
      */
    void restoreZoomConfig();

    /**
      * Save zoom value
      */
    void saveZoomConfig(KoZoomMode::Mode zoomMode, int zoom);

    /**
     * Setter of the zoom
     *
     * @param zoom percent
     * @param zoom mode
     */
    void setZoom(KoZoomMode::Mode zoomMode, int zoom);

    /**
     * Return the last zoom stored
     *
     * @return the last zoom stored
     */
    int zoom();

    /**
     * Return the last zoom mode stored
     *
     * @return the last zoom mode stored
     */
    KoZoomMode::Mode zoomMode();

public slots:
    /**
     * Activate the presentation view mode
     */
    void startPresentation();

    /**
     * Activate the presentation view mode from the first slide
     */
    void startPresentationFromBeginning();

    /**
     * Stop the presentation and activate the previously active view mode.
     */
    void stopPresentation();

    void showNormal();

    /** Change current view mode using a given index */
    void changeViewByIndex(int index);

    void replaceActivePage(KoPAPageBase *page, KoPAPageBase *newActivePage);

protected:
    void initGUI();
    void initActions();

protected slots:
    void createAnimation();
    void showNotes();
    void showSlidesSorter();
    void editCustomSlideShows();
    void configureSlideShow();
    void configurePresenterView();
    void exportToHtml();
    void insertPictures();
    void drawOnPresentation();
    void highlightPresentation();
    void blackPresentation();
    void showStatusBar(bool toggled);
    /// called if the zoom changed
    void zoomChanged(KoZoomMode::Mode mode, qreal zoom);

private:
    KPrPart *m_part;
    KActionMenu *m_actionStartPresentation;
    KAction *m_actionCreateAnimation;
    KAction *m_actionViewModeNormal;
    KAction *m_actionViewModeNotes;
    KAction *m_actionViewModeSlidesSorter;
    KAction *m_actionEditCustomSlideShows;
    KAction *m_actionExportHtml;
    KAction *m_actionInsertPictures;
    KAction *m_actionDrawOnPresentation;
    KAction *m_actionHighlightPresentation;
    KAction *m_actionBlackPresentation;

    KPrViewModePresentation *m_presentationMode;
    KoPAViewMode *m_normalMode;
    KPrViewModeNotes *m_notesMode;
    KPrViewModeSlidesSorter *m_slidesSorterMode;

    KPrViewAdaptor *m_dbus;
    int m_zoom;
    KoZoomMode::Mode m_zoomMode;

    virtual KoPrintJob *createPdfPrintJob();
};

#endif /* KPRVIEW_H */
